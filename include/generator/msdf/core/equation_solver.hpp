#ifndef __QONTEXT_EQUATION_SOLVER_HPP__
#define __QONTEXT_EQUATION_SOLVER_HPP__

namespace qtxt {
    int SolveQuadratic(double x[2], double a, double b, double c);
    int SolveCubic(double x[3], double a, double b, double c, double d);
}

#endif