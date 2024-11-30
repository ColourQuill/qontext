#include <generator/msdf/core/edge_coloring.hpp>

// std
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cfloat>
#include <vector>
#include <queue>

// qarbon
#include <qarbon.hpp>

// msdf
#include <generator/msdf/core/arithmetic.hpp>
#include <generator/msdf/core/edge_segments.hpp>
#include <generator/msdf/core/edge_holder.hpp>
#include <generator/msdf/core/contour.hpp>
#include <generator/msdf/core/shape.hpp>

using vec2 = qbn::vec2<double>;

namespace qtxt {
    static int SymmetricalTrichotomy(int position, int n) {
        return int(3 + 2.875 * position / (n - 1) - 1.4375 + 0.5) - 3;
    }

    static bool IsCorner(const vec2& aDir, const vec2& bDir, double crossThreshold) {
        return qbn::dot(aDir, bDir) <= 0 || fabs(qbn::cross(aDir, bDir)) > crossThreshold;
    }

    static double EstimateEdgeLength(const EdgeSegment* edge) {
        double len = 0;
        vec2 prev = edge->Point(0);
        for (int i = 1; i <= MSDF_EDGE_LENGTH_PRECISION; ++i) {
            vec2 cur = edge->Point(1.0 / MSDF_EDGE_LENGTH_PRECISION * i);
            len += qbn::length(cur - prev);
            prev = cur;
        }
        return len;
    }

    static int SeedExtract2(unsigned long long& seed) {
        int v = int(seed) & 1;
        seed >>= 1;
        return v;
    }

    static int SeedExtract3(unsigned long long& seed) {
        int v = int(seed % 3);
        seed /= 3;
        return v;
    }

    static EdgeColor InitColor(unsigned long long& seed) {
        static const EdgeColor colors[3] = {CYAN, MAGENTA, YELLOW};
        return colors[SeedExtract3(seed)];
    }

    static void SwitchColor(EdgeColor& color, unsigned long long& seed) {
        int shifted = color << (1 + SeedExtract2(seed));
        color = EdgeColor((shifted | shifted >> 3) & WHITE);
    }

    static void SwitchColor(EdgeColor& color, unsigned long long& seed, EdgeColor banned) {
        EdgeColor combined = EdgeColor(color & banned);
        if (combined == RED || combined == GREEN || combined == BLUE) {
            color = EdgeColor(combined ^ WHITE);
        } else {
            SwitchColor(color, seed);
        }
    }

    void EdgeColoringSimple(Shape &shape, double angleThreshold, unsigned long long seed) {
        double crossThreshold = sin(angleThreshold);
        EdgeColor color = InitColor(seed);
        std::vector<int> corners;
        for (Contour& contour : shape.contours) {
            if (contour.edges.empty()) {
                continue;
            }
            { // Identify corners
                corners.clear();
                vec2 prevDirection = contour.edges.back()->Direction(1);
                int index = 0;
                for (const EdgeHolder& edge : contour.edges) {
                    if (IsCorner(qbn::norm(prevDirection), qbn::norm(edge->Direction(0)), crossThreshold)) {
                        corners.push_back(index);
                    }
                    prevDirection = edge->Direction(1);
                    ++index;
                }
            }

            // Smooth contour
            if (corners.empty()) {
                SwitchColor(color, seed);
                for (EdgeHolder& edge : contour.edges) {
                    edge->color = color;
                }
            }
            // "Teardrop" case
            else if (corners.size() == 1) {
                EdgeColor colors[3];
                SwitchColor(color, seed);
                colors[0] = color;
                colors[1] = WHITE;
                SwitchColor(color, seed);
                colors[2] = color;
                int corner = corners[0];
                if (contour.edges.size() >= 3) {
                    int m = (int)contour.edges.size();
                    for (int i = 0; i < m; ++i) {
                        contour.edges[(corner + i) % m]->color = colors[1 + SymmetricalTrichotomy(i, m)];
                    }
                } else if (contour.edges.size() >= 1) {
                    // Less than three edge segments for three colors => edges must be split
                    EdgeSegment *parts[7] = { };
                    contour.edges[0]->SplitInThirds(parts[0 + 3 * corner], parts[1 + 3 * corner], parts[2 + 3 * corner]);
                    if (contour.edges.size() >= 2) {
                        contour.edges[1]->SplitInThirds(parts[3 - 3 * corner], parts[4 - 3 * corner], parts[5 - 3 * corner]);
                        parts[0]->color = parts[1]->color = colors[0];
                        parts[2]->color = parts[3]->color = colors[1];
                        parts[4]->color = parts[5]->color = colors[2];
                    } else {
                        parts[0]->color = colors[0];
                        parts[1]->color = colors[1];
                        parts[2]->color = colors[2];
                    }
                    contour.edges.clear();
                    for (int i = 0; parts[i]; ++i) {
                        contour.edges.push_back(EdgeHolder(parts[i]));
                    }
                }
            }
            // Multiple corners
            else {
                int cornerCount = (int) corners.size();
                int spline = 0;
                int start = corners[0];
                int m = (int)contour.edges.size();
                SwitchColor(color, seed);
                EdgeColor initialColor = color;
                for (int i = 0; i < m; ++i) {
                    int index = (start + i) % m;
                    if (spline + 1 < cornerCount && corners[spline + 1] == index) {
                        ++spline;
                        SwitchColor(color, seed, EdgeColor((spline == cornerCount - 1) * initialColor));
                    }
                    contour.edges[index]->color = color;
                }
            }
        }
    }

    struct EdgeColoringInkTrapCorner {
        int index;
        double prevEdgeLengthEstimate;
        bool minor;
        EdgeColor color;
    };

    void EdgeColoringInkTrap(Shape &shape, double angleThreshold, unsigned long long seed) {
        typedef EdgeColoringInkTrapCorner Corner;
        double crossThreshold = sin(angleThreshold);
        EdgeColor color = InitColor(seed);
        std::vector<Corner> corners;
        for (Contour& contour : shape.contours) {
            if (contour.edges.empty()) {
                continue;
            }
            double splineLength = 0;
            { // Identify corners
                corners.clear();
                vec2 prevDirection = contour.edges.back()->Direction(1);
                int index = 0;
                for (const EdgeHolder& edge : contour.edges) {
                    if (IsCorner(qbn::norm(prevDirection), qbn::norm(edge->Direction(0)), crossThreshold)) {
                        Corner corner = {index, splineLength};
                        corners.push_back(corner);
                        splineLength = 0;
                    }
                    splineLength += EstimateEdgeLength(edge);
                    prevDirection = edge->Direction(1);
                    ++index;
                }
            }

            // Smooth contour
            if (corners.empty()) {
                SwitchColor(color, seed);
                for (EdgeHolder& edge: contour.edges) {
                    edge->color = color;
                }
            }
            // "Teardrop" case
            else if (corners.size() == 1) {
                EdgeColor colors[3];
                SwitchColor(color, seed);
                colors[0] = color;
                colors[1] = WHITE;
                SwitchColor(color, seed);
                colors[2] = color;
                int corner = corners[0].index;
                if (contour.edges.size() >= 3) {
                    int m = (int)contour.edges.size();
                    for (int i = 0; i < m; ++i) {
                        contour.edges[(corner + i) % m]->color = colors[1 + SymmetricalTrichotomy(i, m)];
                    }
                } else if (contour.edges.size() >= 1) {
                    // Less than three edge segments for three colors => edges must be split
                    EdgeSegment *parts[7] = { };
                    contour.edges[0]->SplitInThirds(parts[0 + 3 * corner], parts[1 + 3 * corner], parts[2 + 3 * corner]);
                    if (contour.edges.size() >= 2) {
                        contour.edges[1]->SplitInThirds(parts[3 - 3 * corner], parts[4 - 3 * corner], parts[5 - 3 * corner]);
                        parts[0]->color = parts[1]->color = colors[0];
                        parts[2]->color = parts[3]->color = colors[1];
                        parts[4]->color = parts[5]->color = colors[2];
                    } else {
                        parts[0]->color = colors[0];
                        parts[1]->color = colors[1];
                        parts[2]->color = colors[2];
                    }
                    contour.edges.clear();
                    for (int i = 0; parts[i]; ++i) {
                        contour.edges.push_back(EdgeHolder(parts[i]));
                    }
                }
            }
            // Multiple corners
            else {
                int cornerCount = (int)corners.size();
                int majorCornerCount = cornerCount;
                if (cornerCount > 3) {
                    corners.begin()->prevEdgeLengthEstimate += splineLength;
                    for (int i = 0; i < cornerCount; ++i) {
                        if (
                            corners[i].prevEdgeLengthEstimate > corners[(i+1)%cornerCount].prevEdgeLengthEstimate &&
                            corners[(i + 1) % cornerCount].prevEdgeLengthEstimate < corners[(i + 2) % cornerCount].prevEdgeLengthEstimate
                        ) {
                            corners[i].minor = true;
                            --majorCornerCount;
                        }
                    }
                }
                EdgeColor initialColor = BLACK;
                for (int i = 0; i < cornerCount; ++i) {
                    if (!corners[i].minor) {
                        --majorCornerCount;
                        SwitchColor(color, seed, EdgeColor(!majorCornerCount * initialColor));
                        corners[i].color = color;
                        if (!initialColor) {
                            initialColor = color;
                        }
                    }
                }
                for (int i = 0; i < cornerCount; ++i) {
                    if (corners[i].minor) {
                        EdgeColor nextColor = corners[(i + 1) % cornerCount].color;
                        corners[i].color = EdgeColor((color & nextColor) ^ WHITE);
                    } else {
                        color = corners[i].color;
                    }
                }
                int spline = 0;
                int start = corners[0].index;
                color = corners[0].color;
                int m = (int)contour.edges.size();
                for (int i = 0; i < m; ++i) {
                    int index = (start + i) % m;
                    if (spline+1 < cornerCount && corners[spline + 1].index == index)
                        color = corners[++spline].color;
                    contour.edges[index]->color = color;
                }
            }
        }
    }

    #define MAX_RECOLOR_STEPS 16
    #define EDGE_DISTANCE_PRECISION 16

    static double EdgeToEdgeDistance(const EdgeSegment& a, const EdgeSegment& b, int precision) {
        if (a.Point(0) == b.Point(0) || a.Point(0) == b.Point(1) || a.Point(1) == b.Point(0) || a.Point(1) == b.Point(1)) {
            return 0;
        }
        double iFac = 1.0 / precision;
        double minDistance = qbn::length(b.Point(0) - a.Point(0));
        for (int i = 0; i <= precision; ++i) {
            double t = iFac * i;
            double d = fabs(a.GetSignedDistance(b.Point(t), t).distance);
            minDistance = min(minDistance, d);
        }
        for (int i = 0; i <= precision; ++i) {
            double t = iFac * i;
            double d = fabs(b.GetSignedDistance(a.Point(t), t).distance);
            minDistance = min(minDistance, d);
        }
        return minDistance;
    }

    static double SplineToSplineDistance(EdgeSegment* const* edgeSegments, int aStart, int aEnd, int bStart, int bEnd, int precision) {
        double minDistance = DBL_MAX;
        for (int ai = aStart; ai < aEnd; ++ai)
            for (int bi = bStart; bi < bEnd && minDistance; ++bi) {
                double d = EdgeToEdgeDistance(*edgeSegments[ai], *edgeSegments[bi], precision);
                minDistance = min(minDistance, d);
            }
        return minDistance;
    }

    static void ColorSecondDegreeGraph(int* coloring, const int* const* edgeMatrix, int vertexCount, unsigned long long seed) {
        for (int i = 0; i < vertexCount; ++i) {
            int possibleColors = 7;
            for (int j = 0; j < i; ++j) {
                if (edgeMatrix[i][j]) {
                    possibleColors &= ~(1 << coloring[j]);
                }
            }
            int color = 0;
            switch (possibleColors) {
                case 1:
                    color = 0;
                    break;
                case 2:
                    color = 1;
                    break;
                case 3:
                    color = SeedExtract2(seed); // 0 or 1
                    break;
                case 4:
                    color = 2;
                    break;
                case 5:
                    color = (int)!SeedExtract2(seed) << 1; // 2 or 0
                    break;
                case 6:
                    color = SeedExtract2(seed) + 1; // 1 or 2
                    break;
                case 7:
                    color = (SeedExtract3(seed) + i) % 3; // 0 or 1 or 2
                    break;
            }
            coloring[i] = color;
        }
    }

    static int VertexPossibleColors(const int* coloring, const int* edgeVector, int vertexCount) {
        int usedColors = 0;
        for (int i = 0; i < vertexCount; ++i) {
            if (edgeVector[i]) {
                usedColors |= 1 << coloring[i];
            }
        }
            
        return 7 & ~usedColors;
    }

    static void UncolorSameNeighbors(std::queue<int>& uncolored, int* coloring, const int* const* edgeMatrix, int vertex, int vertexCount) {
        for (int i = vertex + 1; i < vertexCount; ++i) {
            if (edgeMatrix[vertex][i] && coloring[i] == coloring[vertex]) {
                coloring[i] = -1;
                uncolored.push(i);
            }
        }
        for (int i = 0; i < vertex; ++i) {
            if (edgeMatrix[vertex][i] && coloring[i] == coloring[vertex]) {
                coloring[i] = -1;
                uncolored.push(i);
            }
        }
    }

    static bool TryAddEdge(int* coloring, int* const* edgeMatrix, int vertexCount, int vertexA, int vertexB, int* coloringBuffer) {
        static const int FIRST_POSSIBLE_COLOR[8] = { -1, 0, 1, 0, 2, 2, 1, 0 };
        edgeMatrix[vertexA][vertexB] = 1;
        edgeMatrix[vertexB][vertexA] = 1;
        if (coloring[vertexA] != coloring[vertexB]) {
            return true;
        }
        int bPossibleColors = VertexPossibleColors(coloring, edgeMatrix[vertexB], vertexCount);
        if (bPossibleColors) {
            coloring[vertexB] = FIRST_POSSIBLE_COLOR[bPossibleColors];
            return true;
        }
        memcpy(coloringBuffer, coloring, sizeof(int)*vertexCount);
        std::queue<int> uncolored;
        {
            int *coloring = coloringBuffer;
            coloring[vertexB] = FIRST_POSSIBLE_COLOR[7&~(1<<coloring[vertexA])];
            UncolorSameNeighbors(uncolored, coloring, edgeMatrix, vertexB, vertexCount);
            int step = 0;
            while (!uncolored.empty() && step < MAX_RECOLOR_STEPS) {
                int i = uncolored.front();
                uncolored.pop();
                int possibleColors = VertexPossibleColors(coloring, edgeMatrix[i], vertexCount);
                if (possibleColors) {
                    coloring[i] = FIRST_POSSIBLE_COLOR[possibleColors];
                    continue;
                }
                do {
                    coloring[i] = step++ % 3;
                } while (edgeMatrix[i][vertexA] && coloring[i] == coloring[vertexA]);
                UncolorSameNeighbors(uncolored, coloring, edgeMatrix, i, vertexCount);
            }
        }
        if (!uncolored.empty()) {
            edgeMatrix[vertexA][vertexB] = 0;
            edgeMatrix[vertexB][vertexA] = 0;
            return false;
        }
        memcpy(coloring, coloringBuffer, sizeof(int)*vertexCount);
        return true;
    }

    static int cmpDoublePtr(const void *a, const void *b) {
        return sign(**reinterpret_cast<const double* const*>(a) - **reinterpret_cast<const double* const*>(b));
    }

    void EdgeColoringByDistance(Shape& shape, double angleThreshold, unsigned long long seed) {
        std::vector<EdgeSegment*> edgeSegments;
        std::vector<int> splineStarts;

        double crossThreshold = sin(angleThreshold);
        std::vector<int> corners;
        for (Contour& contour : shape.contours)
            if (!contour.edges.empty()) {
                // Identify corners
                corners.clear();
                vec2 prevDirection = contour.edges.back()->Direction(1);
                int index = 0;
                for (const EdgeHolder& edge : contour.edges) {
                    if (IsCorner(qbn::norm(prevDirection), qbn::norm(edge->Direction(0)), crossThreshold)) {
                        corners.push_back(index);
                    } 
                    prevDirection = edge->Direction(1);
                    ++index;
                }

                splineStarts.push_back((int)edgeSegments.size());
                // Smooth contour
                if (corners.empty()) {
                    for (EdgeHolder& edge : contour.edges) {
                        edgeSegments.push_back(&*edge);
                    }
                }
                    
                // "Teardrop" case
                else if (corners.size() == 1) {
                    int corner = corners[0];
                    if (contour.edges.size() >= 3) {
                        int m = (int)contour.edges.size();
                        for (int i = 0; i < m; ++i) {
                            if (i == m / 2) {
                                splineStarts.push_back((int) edgeSegments.size());
                            }
                            if (SymmetricalTrichotomy(i, m)) {
                                edgeSegments.push_back(&*contour.edges[(corner + i) % m]);
                            } else {
                                contour.edges[(corner + i) % m]->color = WHITE;
                            }
                        }
                    } else if (contour.edges.size() >= 1) {
                        // Less than three edge segments for three colors => edges must be split
                        EdgeSegment* parts[7] = { };
                        contour.edges[0]->SplitInThirds(parts[0 + 3 * corner], parts[1 + 3 * corner], parts[2 + 3 * corner]);
                        if (contour.edges.size() >= 2) {
                            contour.edges[1]->SplitInThirds(parts[3 - 3 * corner], parts[4 - 3 * corner], parts[5 - 3 * corner]);
                            edgeSegments.push_back(parts[0]);
                            edgeSegments.push_back(parts[1]);
                            parts[2]->color = parts[3]->color = WHITE;
                            splineStarts.push_back((int) edgeSegments.size());
                            edgeSegments.push_back(parts[4]);
                            edgeSegments.push_back(parts[5]);
                        } else {
                            edgeSegments.push_back(parts[0]);
                            parts[1]->color = WHITE;
                            splineStarts.push_back((int) edgeSegments.size());
                            edgeSegments.push_back(parts[2]);
                        }
                        contour.edges.clear();
                        for (int i = 0; parts[i]; ++i) {
                            contour.edges.push_back(EdgeHolder(parts[i]));
                        }
                    }
                }
                // Multiple corners
                else {
                    int cornerCount = (int)corners.size();
                    int spline = 0;
                    int start = corners[0];
                    int m = (int)contour.edges.size();
                    for (int i = 0; i < m; ++i) {
                        int index = (start + i) % m;
                        if (spline + 1 < cornerCount && corners[spline + 1] == index) {
                            splineStarts.push_back((int)edgeSegments.size());
                            ++spline;
                        }
                        edgeSegments.push_back(&*contour.edges[index]);
                    }
                }
            }
        splineStarts.push_back((int)edgeSegments.size());

        int segmentCount = (int)edgeSegments.size();
        int splineCount = (int)splineStarts.size()-1;
        if (!splineCount) {
            return;
        }

        std::vector<double> distanceMatrixStorage(splineCount * splineCount);
        std::vector<double*> distanceMatrix(splineCount);
        for (int i = 0; i < splineCount; ++i) {
            distanceMatrix[i] = &distanceMatrixStorage[i*splineCount];
        }
        const double* distanceMatrixBase = &distanceMatrixStorage[0];

        for (int i = 0; i < splineCount; ++i) {
            distanceMatrix[i][i] = -1;
            for (int j = i + 1; j < splineCount; ++j) {
                double dist = SplineToSplineDistance(&edgeSegments[0], splineStarts[i], splineStarts[i + 1], splineStarts[j], splineStarts[j + 1], EDGE_DISTANCE_PRECISION);
                distanceMatrix[i][j] = dist;
                distanceMatrix[j][i] = dist;
            }
        }

        std::vector<const double*> graphEdgeDistances;
        graphEdgeDistances.reserve(splineCount*(splineCount - 1) / 2);
        for (int i = 0; i < splineCount; ++i) {
            for (int j = i+1; j < splineCount; ++j) {
                graphEdgeDistances.push_back(&distanceMatrix[i][j]);
            }
        }
            
        int graphEdgeCount = (int)graphEdgeDistances.size();
        if (!graphEdgeDistances.empty()) {
            qsort(&graphEdgeDistances[0], graphEdgeDistances.size(), sizeof(const double*), &cmpDoublePtr);
        }

        std::vector<int> edgeMatrixStorage(splineCount * splineCount);
        std::vector<int*> edgeMatrix(splineCount);
        for (int i = 0; i < splineCount; ++i) {
            edgeMatrix[i] = &edgeMatrixStorage[i * splineCount];
        }
        int nextEdge = 0;
        for (; nextEdge < graphEdgeCount && !*graphEdgeDistances[nextEdge]; ++nextEdge) {
            int elem = (int)(graphEdgeDistances[nextEdge] - distanceMatrixBase);
            int row = elem / splineCount;
            int col = elem % splineCount;
            edgeMatrix[row][col] = 1;
            edgeMatrix[col][row] = 1;
        }

        std::vector<int> coloring(2 * splineCount);
        ColorSecondDegreeGraph(&coloring[0], &edgeMatrix[0], splineCount, seed);
        for (; nextEdge < graphEdgeCount; ++nextEdge) {
            int elem = (int) (graphEdgeDistances[nextEdge]-distanceMatrixBase);
            TryAddEdge(&coloring[0], &edgeMatrix[0], splineCount, elem/splineCount, elem%splineCount, &coloring[splineCount]);
        }

        const EdgeColor colors[3] = {YELLOW, CYAN, MAGENTA};
        int spline = -1;
        for (int i = 0; i < segmentCount; ++i) {
            if (splineStarts[spline+1] == i) {
                ++spline;
            }
            edgeSegments[i]->color = colors[coloring[spline]];
        }
    }
} 