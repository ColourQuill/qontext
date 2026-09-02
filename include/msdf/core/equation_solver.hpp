#ifndef __QONTEXT_EQUATION_SOLVER_HPP__
#define __QONTEXT_EQUATION_SOLVER_HPP__

namespace qtxt {
    int solveQuadratic(double x[2], double a, double b, double c);
    int solveCubic(double x[3], double a, double b, double c, double d);
}

#endif