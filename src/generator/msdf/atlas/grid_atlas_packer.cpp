#include <generator/msdf/atlas/grid_atlas_packer.hpp>

// std
#include <algorithm>

namespace qtxt {
    static int FloorPOT(int x) {
        int y = 1;
        while (x >= y && y << 1) {
            y <<= 1;
        }
        return y >> 1;
    }

    static int CeilPOT(int x) {
        int y = 1;
        while (x > y && y << 1) {
            y << 1;
        }
        return y;
    }

    static bool SquareConstraint(DimensionsConstraint constraint) {
        switch (constraint) {
            case DimensionsConstraint::SQUARE:
            case DimensionsConstraint::EVEN_SQUARE:
            case DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE:
            case DimensionsConstraint::POWER_OF_TWO_SQUARE:
                return true;
            case DimensionsConstraint::NONE:
            case DimensionsConstraint::POWER_OF_TWO_RECTANGLE:
                return false;
        }
        return true;
    }

    void GridAtlasPacker::LowerToConstraint(int& width, int& height, DimensionsConstraint constraint) {
        if (SquareConstraint(constraint)) {
            width = height = std::min(width, height);
        }
        switch (constraint) {
            case DimensionsConstraint::NONE:
            case DimensionsConstraint::SQUARE:
                break;
            case DimensionsConstraint::EVEN_SQUARE:
                width &= ~1;
                height &= ~1;
                break;
            case DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE:
                width &= ~3;
                height &= ~3;
                break;
            case DimensionsConstraint::POWER_OF_TWO_RECTANGLE:
            case DimensionsConstraint::POWER_OF_TWO_SQUARE:
                if (width > 0) {
                    width = FloorPOT(width);
                }
                if (height > 0) {
                    height = FloorPOT(height);
                }
                break;
        }
    }

    void GridAtlasPacker::RaiseToConstraint(int& width, int& height, DimensionsConstraint constraint) {
        if (SquareConstraint(constraint)) {
            width = height = std::max(width, height);
        }
        switch (constraint) {
            case DimensionsConstraint::NONE:
            case DimensionsConstraint::SQUARE:
                break;
            case DimensionsConstraint::EVEN_SQUARE:
                width += width & 1;
                height += height & 1;
                break;
            case DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE:
                width += -width & 3;
                width += -height & 3;
                break;
            case DimensionsConstraint::POWER_OF_TWO_RECTANGLE:
            case DimensionsConstraint::POWER_OF_TWO_SQUARE:
                if (width > 0) {
                    width = CeilPOT(width);
                }
                if (height > 0) {
                    height = CeilPOT(height);
                }
                break;

        }
    }

    int GridAtlasPacker::Pack(GlyphGeometry *glyphs, int count) {
        if (!count) {
            return 0;
        }
        GridAtlasPacker initial(*this);
        int cellCount = 0;
        if (columns > 0 && rows > 0) {
            cellCount = columns * rows;
        }
        else {
            for (const GlyphGeometry* glyph = glyphs, *end = glyphs + count; glyph < end; ++glyph) {
                if (!glyph->isWhitespace()) {
                    ++cellCount;
                }
            }
            if (columns > 0) {
                rows = (cellCount + columns - 1) / columns;
            }
            else if (rows > 0) {
                columns = (cellCount + rows - 1) / rows;
            }
            else if (width > 0 && cellWidth > 0) {
                columns = (width + spacing) / cellWidth;
                rows = (cellCount + columns - 1) / columns;
            }
        }

        if (width < 0 && cellWidth > 0 && columns > 0) {
            width = columns * cellWidth;
        }
        if (height < 0 && cellHeight > 0 && rows > 0) {
            height = rows * cellHeight;
        }
        if (width != initial.width || height != initial.height) {
            RaiseToConstraint(width, height, dimensionsConstraint);
        }

        if (cellWidth < 0 && width > 0 && columns > 0) {
            cellWidth = (width + spacing) / columns;
        }
        if (cellHeight < 0 && height > 0 && rows > 0) {
            cellHeight = (height + spacing) / rows;
        }
        if (cellWidth != initial.cellWidth || cellHeight != initial.cellHeight) {
            bool positiveCellWidth = cellWidth > 0, positiveCellHeight = cellHeight > 0;
            LowerToConstraint(cellWidth, cellHeight, cellDimensionsConstraint);
            if ((cellWidth == 0 && positiveCellWidth) || (cellHeight == 0 && positiveCellHeight)) {
                return -1;
            }  
        }

        if ((cellWidth > 0 && cellWidth - spacing - 1 <= -2 * pxRange.lower) || (cellHeight > 0 && cellHeight - spacing - 1 <= -2 * pxRange.lower)) {
            return -1;
        }

        Bounds maxBounds = { };
        double maxWidth = 0, maxHeight = 0;

        if (scale <= 0) {
            if (pxRange.lower != pxRange.upper && miterLimit > 0) {
                if (cellWidth > 0 || cellHeight > 0) {
                    scale = ScaleToFit(glyphs, count, cellWidth, cellHeight, maxBounds, maxWidth, maxHeight);
                    if (scale < minScale) {
                        scale = minScale;
                        cutoff = true;
                        maxBounds = GetMaxBounds(maxWidth, maxHeight, glyphs, count, scale, -(unitRange.lower + pxRange.lower / scale));
                    }
                } else if (width > 0 && height > 0) {
                    double bestAlignedScale = 0;
                    int bestCols = 0, bestAlignedCols = 0;
                    for (int q = (int)sqrt(cellCount) + 1; q > 0; --q) {
                        int cols = q;
                        int rows = (cellCount + cols - 1) / cols;
                        int tWidth = (width + spacing) / cols;
                        int tHeight = (height + spacing) / rows;
                        LowerToConstraint(tWidth, tHeight, cellDimensionsConstraint);
                        if (tWidth > 0 && tHeight > 0) {
                            double curScale = ScaleToFit(glyphs, count, tWidth, tHeight, maxBounds, maxWidth, maxHeight);
                            if (curScale > scale) {
                                scale = curScale;
                                bestCols = cols;
                            }
                            if (cols*tWidth == width && curScale > bestAlignedScale) {
                                bestAlignedScale = curScale;
                                bestAlignedCols = cols;
                            }
                        }
                        cols = (cellCount + q - 1) / q;
                        rows = (cellCount + cols - 1) / cols;
                        tWidth = (width + spacing) / cols;
                        tHeight = (height + spacing) / rows;
                        LowerToConstraint(tWidth, tHeight, cellDimensionsConstraint);
                        if (tWidth > 0 && tHeight > 0) {
                            double curScale = ScaleToFit(glyphs, count, tWidth, tHeight, maxBounds, maxWidth, maxHeight);
                            if (curScale > scale) {
                                scale = curScale;
                                bestCols = cols;
                            }
                            if (cols * tWidth == width && curScale > bestAlignedScale) {
                                bestAlignedScale = curScale;
                                bestAlignedCols = cols;
                            }
                        }
                    }
                    if (!bestCols) {
                        return -1;
                    }
                    if (bestAlignedScale >= minScale && (alignedColumnsBias + 1) * bestAlignedScale >= scale) {
                        scale = bestAlignedScale;
                        bestCols = bestAlignedCols;
                    }

                    columns = bestCols;
                    rows = (cellCount + columns - 1) / columns;
                    cellWidth = (width + spacing) / columns;
                    cellHeight = (height + spacing) / rows;
                    LowerToConstraint(cellWidth, cellHeight, cellDimensionsConstraint);
                    scale = ScaleToFit(glyphs, count, cellWidth, cellHeight, maxBounds, maxWidth, maxHeight);
                    if (scale < minScale) {
                        scale = -1;
                    }
                }

                if (scale <= 0) {
                    maxBounds = GetMaxBounds(maxWidth, maxHeight, glyphs, count, minScale, -(unitRange.lower + pxRange.lower / minScale));
                    cellWidth = (int)ceil(maxWidth) + spacing + 1;
                    cellHeight = (int)ceil(maxHeight) + spacing + 1;
                    RaiseToConstraint(cellWidth, cellHeight, cellDimensionsConstraint);
                    scale = ScaleToFit(glyphs, count, cellWidth, cellHeight, maxBounds, maxWidth, maxHeight);
                    if (scale < minScale) {
                        maxBounds = GetMaxBounds(maxWidth, maxHeight, glyphs, count, scale = minScale, -(unitRange.lower + pxRange.lower / minScale));
                    }
                }

                if (initial.rows < 0 && initial.cellHeight < 0) {
                    int optimalCellWidth = cellWidth, optimalCellHeight = (int)ceil(maxHeight) + spacing + 1;
                    RaiseToConstraint(optimalCellWidth, optimalCellHeight, cellDimensionsConstraint);
                    if (optimalCellHeight < cellHeight && optimalCellWidth <= cellWidth) {
                        cellWidth = optimalCellWidth;
                        cellHeight = optimalCellHeight;
                    }
                }

            } else {
                Padding pxPadding = innerPxPadding + outerPxPadding;
                maxBounds = GetMaxBounds(maxWidth, maxHeight, glyphs, count, 1, -unitRange.lower);
                Pad(maxBounds, -pxPadding);
                maxWidth -= pxPadding.l + pxPadding.r;
                maxHeight -= pxPadding.b + pxPadding.t;
                int hSlack = 0, vSlack = 0;
                if (pxAlignOriginX && !hFixed) {
                    maxWidth -= 1;
                    hSlack = 1;
                }
                if (pxAlignOriginY && !vFixed) {
                    maxHeight -= 1;
                    vSlack = 1;
                }

                double extraPxWidth = -2 * pxRange.lower + pxPadding.l + pxPadding.r;
                double extraPxHeight = -2 * pxRange.lower + pxPadding.b + pxPadding.t;
                double hScale = 0, vScale = 0;
                if (cellWidth > 0) {
                    hScale = (cellWidth - hSlack - spacing - extraPxWidth - 1) / maxWidth;
                }
                if (cellHeight > 0) {
                    vScale = (cellHeight - vSlack - spacing - extraPxHeight - 1) / maxHeight;
                }
                if (hScale || vScale) {
                    if (hScale && vScale) {
                        scale = std::min(hScale, vScale);
                    } else {
                        scale = hScale + vScale;
                    }
                    if (scale < minScale) {
                        scale = minScale;
                        cutoff = true;
                    }
                } else if (width > 0 && height > 0) {
                    double bestAlignedScale = 0;
                    int bestCols = 0, bestAlignedCols = 0;
                    for (int cols = 1; cols < width; ++cols) {
                        int rows = (cellCount + cols - 1) / cols;
                        int tWidth = (width + spacing) / cols;
                        int tHeight = (height + spacing) / rows;
                        LowerToConstraint(tWidth, tHeight, cellDimensionsConstraint);
                        if (tWidth > 0 && tHeight > 0) {
                            hScale = (tWidth - hSlack - spacing - extraPxWidth - 1) / maxWidth;
                            vScale = (tHeight - vSlack - spacing - extraPxHeight - 1) / maxHeight;
                            double curScale = std::min(hScale, vScale);
                            if (curScale > scale) {
                                scale = curScale;
                                bestCols = cols;
                            }
                            if (cols*tWidth == width && curScale > bestAlignedScale) {
                                bestAlignedScale = curScale;
                                bestAlignedCols = cols;
                            }
                        }
                    }
                    if (!bestCols) {
                        return -1;
                    }
                    if (bestAlignedScale >= minScale && (alignedColumnsBias + 1) * bestAlignedScale >= scale) {
                        scale = bestAlignedScale;
                        bestCols = bestAlignedCols;
                    }

                    columns = bestCols;
                    rows = (cellCount + columns - 1) / columns;
                    cellWidth = (width + spacing) / columns;
                    cellHeight = (height + spacing) / rows;
                    LowerToConstraint(cellWidth, cellHeight, cellDimensionsConstraint);
                    if (scale < minScale) {
                        scale = -1;
                    }
                }

                if (scale <= 0) {
                    cellWidth = (int)ceil(minScale * maxWidth + extraPxWidth) + hSlack + spacing + 1;
                    cellHeight = (int)ceil(minScale * maxHeight + extraPxHeight) + vSlack + spacing + 1;
                    RaiseToConstraint(cellWidth, cellHeight, cellDimensionsConstraint);
                    hScale = (cellWidth - hSlack - spacing - extraPxWidth - 1) / maxWidth;
                    vScale = (cellHeight - vSlack - spacing - extraPxHeight - 1) / maxHeight;
                    scale = std::min(hScale, vScale);
                }

                if (initial.rows < 0 && initial.cellHeight < 0) {
                    int optimalCellWidth = cellWidth, optimalCellHeight = (int)ceil(scale * maxHeight + extraPxHeight) + vSlack + spacing + 1;
                    RaiseToConstraint(optimalCellWidth, optimalCellHeight, cellDimensionsConstraint);
                    if (optimalCellHeight < cellHeight && optimalCellWidth <= cellWidth) {
                        cellWidth = optimalCellWidth;
                        cellHeight = optimalCellHeight;
                    }
                }
                maxBounds.l *= scale, maxBounds.b *= scale;
                maxBounds.r *= scale, maxBounds.t *= scale;
                maxWidth *= scale, maxHeight *= scale;
                Pad(maxBounds, pxPadding);
                maxWidth += pxPadding.l + pxPadding.r;
                maxHeight += pxPadding.b + pxPadding.t;

            }
        } else {
            maxBounds = GetMaxBounds(maxWidth, maxHeight, glyphs, count, scale, -(unitRange.lower + pxRange.lower / scale));
            int optimalCellWidth = (int)ceil(maxWidth) + spacing + 1;
            int optimalCellHeight = (int)ceil(maxHeight) + spacing + 1;
            if (cellWidth < 0 || cellHeight < 0) {
                cellWidth = optimalCellWidth;
                cellHeight = optimalCellHeight;
                RaiseToConstraint(cellWidth, cellHeight, cellDimensionsConstraint);
            } else if (cellWidth < optimalCellWidth || cellHeight < optimalCellHeight) {
                cutoff = true;
            }
        }

        // Compute fixed origin
        if (hFixed) {
            if (pxAlignOriginX) {
                int sl = (int)floor(maxBounds.l - 0.5);
                int sr = (int)ceil(maxBounds.r + 0.5);
                fixedX = (-sl + (cellWidth - spacing - (sr - sl)) / 2) / scale;
            } else {
                fixedX = (-maxBounds.l + 0.5 * (cellWidth - spacing - maxWidth)) / scale;
            }
        }
        if (vFixed) {
            if (pxAlignOriginY) {
                int sb = (int)floor(maxBounds.b - 0.5);
                int st = (int)ceil(maxBounds.t + 0.5);
                fixedY = (-sb + (cellHeight - spacing - (st - sb)) / 2) / scale;
            } else
                fixedY = (-maxBounds.b + 0.5 * (cellHeight - spacing - maxHeight)) / scale;
        }

        if (width < 0 || height < 0) {
            if (columns <= 0) {
                double bestRating = -1;
                for (int q = (int)sqrt(cellCount) + 1; q > 0; --q) {
                    int cols = q;
                    int rows = (cellCount + cols - 1) / cols;
                    int curWidth = cols*cellWidth, curHeight = rows * cellHeight;
                    RaiseToConstraint(curWidth, curHeight, dimensionsConstraint);
                    double rating = DimensionsRating(curWidth, curHeight, cols*cellWidth == curWidth);
                    if (rating < bestRating || bestRating < 0) {
                        bestRating = rating;
                        columns = cols;
                    }
                    rows = q;
                    cols = (cellCount + rows - 1) / rows;
                    curWidth = cols*cellWidth, curHeight = rows * cellHeight;
                    RaiseToConstraint(curWidth, curHeight, dimensionsConstraint);
                    rating = DimensionsRating(curWidth, curHeight, cols * cellWidth == curWidth);
                    if (rating < bestRating || bestRating < 0) {
                        bestRating = rating;
                        columns = cols;
                    }
                }
                rows = (cellCount + columns - 1) / columns;
            }
            width = columns * cellWidth, height = rows * cellHeight;
            RaiseToConstraint(width, height, dimensionsConstraint);
            if (dimensionsConstraint != DimensionsConstraint::NONE && initial.cellWidth < 0 && initial.cellHeight < 0) {
                cellWidth = initial.cellWidth;
                cellHeight = initial.cellHeight;
                columns = initial.columns;
                rows = initial.rows;
                scale = initial.scale;
                return Pack(glyphs, count);
            }
        }

        if (columns < 0) {
            columns = (width + spacing) / cellWidth;
            rows = (cellCount + columns - 1) / columns;
        }
        if (rows * cellHeight > height) {
            rows = height / cellHeight;
        }

        GlyphGeometry::GlyphAttributes attribs = { };
        attribs.scale = scale;
        attribs.range = unitRange + pxRange / scale;
        attribs.innerPadding = innerUnitPadding + 1/  scale * innerPxPadding;
        attribs.outerPadding = outerUnitPadding + 1 / scale * outerPxPadding;
        attribs.miterLimit = miterLimit;
        attribs.pxAlignOriginX = pxAlignOriginX;
        attribs.pxAlignOriginY = pxAlignOriginY;
        int col = 0, row = 0;
        for (GlyphGeometry *glyph = glyphs, *end = glyphs+count; glyph < end; ++glyph) {
            if (!glyph->isWhitespace()) {
                glyph->FrameBox(attribs, cellWidth - spacing, cellHeight - spacing, hFixed ? &fixedX : nullptr, vFixed ? &fixedY : nullptr);
                glyph->PlaceBox(col * cellWidth, height - (row + 1) * cellHeight);
                if (++col >= columns) {
                    if (++row >= rows) {
                        return end - glyph - 1;
                    }
                    col = 0;
                }
            }
        }

        return 0;
    }

    void GridAtlasPacker::SetFixedOrigin(bool horizontal, bool vertical) {
        hFixed = horizontal, vFixed = vertical;
    }

    void GridAtlasPacker::SetCellDimensions(int width, int height) {
        cellWidth = width, cellHeight = height;
    }

    void GridAtlasPacker::UnsetCellDimensions() {
        cellWidth = -1, cellHeight = -1;
    }

    void GridAtlasPacker::SetCellDimensionsConstraint(DimensionsConstraint dimensionsConstraint) {
        cellDimensionsConstraint = dimensionsConstraint;
    }

    void GridAtlasPacker::SetColumns(int columns) {
        this->columns = columns;
    }

    void GridAtlasPacker::SetRows(int rows) {
        this->rows = rows;
    }

    void GridAtlasPacker::UnsetColumns() {
        columns = -1;
    }

    void GridAtlasPacker::UnsetRows() {
        rows = -1;
    }

    void GridAtlasPacker::SetDimensions(int width, int height) {
        this->width = width, this->height = height;
    }

    void GridAtlasPacker::UnsetDimensions() {
        width = -1, height = -1;
    }

    void GridAtlasPacker::SetDimensionsConstraint(DimensionsConstraint dimensionsConstraint) {
        this->dimensionsConstraint = dimensionsConstraint;
    }

    void GridAtlasPacker::SetSpacing(int spacing) {
        this->spacing = spacing;
    }

    void GridAtlasPacker::SetScale(double scale) {
        this->scale = scale;
    }

    void GridAtlasPacker::SetMinimumScale(double minScale) {
        this->minScale = minScale;
    }

    void GridAtlasPacker::SetUnitRange(Range unitRange) {
        this->unitRange = unitRange;
    }

    void GridAtlasPacker::SetPixelRange(Range pxRange) {
        this->pxRange = pxRange;
    }

    void GridAtlasPacker::SetMiterLimit(double miterLimit) {
        this->miterLimit = miterLimit;
    }

    void GridAtlasPacker::SetOriginPixelAlignment(bool align) {
        pxAlignOriginX = align, pxAlignOriginY = align;
    }

    void GridAtlasPacker::SetOriginPixelAlignment(bool alignX, bool alignY) {
        pxAlignOriginX = alignX, pxAlignOriginY = alignY;
    }

    void GridAtlasPacker::SetInnerUnitPadding(const Padding& padding) {
        innerUnitPadding = padding;
    }

    void GridAtlasPacker::SetOuterUnitPadding(const Padding& padding) {
        outerUnitPadding = padding;
    }

    void GridAtlasPacker::SetInnerPixelPadding(const Padding& padding) {
        innerPxPadding = padding;
    }

    void GridAtlasPacker::SetOuterPixelPadding(const Padding& padding) {
        outerPxPadding = padding;
    }

    void GridAtlasPacker::GetDimensions(int& width, int& height) const {
        width = this->width, height = this->height;
    }

    void GridAtlasPacker::GetCellDimensions(int& width, int& height) const {
        width = cellWidth, height = cellHeight;
    }

    int GridAtlasPacker::GetColumns() const {
        return columns;
    }

    int GridAtlasPacker::GetRows() const {
        return rows;
    }

    double GridAtlasPacker::GetScale() const {
        return scale;
    }

    Range GridAtlasPacker::GetPixelRange() const {
        return pxRange + scale * unitRange;
    }

    void GridAtlasPacker::GetFixedOrigin(double& x, double& y) {
        x = fixedX - 0.5 / scale;
        y = fixedY - 0.5 / scale;
    }

    bool GridAtlasPacker::HasCutoff() const {
        return cutoff;
    }
}