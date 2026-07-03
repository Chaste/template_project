/*

Copyright (c) 2005-2025, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <cmath>

#include "SbmlMath.hpp"

// Arithmetic ===================================

// divide
double sbmlmath::divide(double x, double y)
{
    return x / y;
}

// minus
double sbmlmath::minus(double x, double y)
{
    return x - y;
}

// plus
// times

// Logs and exponents ===========================

// log
double sbmlmath::log(double x)
{
    return std::log(x);
}

double sbmlmath::log(double b, double x)
{
    return std::log(x) / std::log(b);
}

// root
double sbmlmath::root(double n, double x)
{
    return std::pow(x, 1.0 / n);
}

// sqr
double sbmlmath::sqr(double x)
{
    return x * x;
}

// Logical ======================================

// and_
// or_

// not_
bool sbmlmath::not_(bool x)
{
    return !x;
}

// xor_

// Relational ===================================

// eq
// geq
// gt
// leq
// lt

// neq
bool sbmlmath::neq(double x, double y)
{
    return x != y;
}

// Trigonometry =================================

// cot, coth, acot, acoth
double sbmlmath::cot(double x)
{
    return 1.0 / std::tan(x);
}

double sbmlmath::coth(double x)
{
    return 1.0 / std::tanh(x);
}

double sbmlmath::acot(double x)
{
    return std::atan(1.0 / x);
}

double sbmlmath::acoth(double x)
{
    return std::atanh(1.0 / x);
}

// csc, csch, acsc, acsch
double sbmlmath::csc(double x)
{
    return 1.0 / std::sin(x);
}

double sbmlmath::csch(double x)
{
    return 1.0 / std::sinh(x);
}

double sbmlmath::acsc(double x)
{
    return std::asin(1.0 / x);
}

double sbmlmath::acsch(double x)
{
    return std::asinh(1.0 / x);
}

// sec, sech, asec, asech
double sbmlmath::sec(double x)
{
    return 1.0 / std::cos(x);
}

double sbmlmath::sech(double x)
{
    return 1.0 / std::cosh(x);
}

double sbmlmath::asec(double x)
{
    return std::acos(1.0 / x);
}

double sbmlmath::asech(double x)
{
    return std::acosh(1.0 / x);
}

// Other functions ==============================

// factorial
double sbmlmath::factorial(double x)
{
    return std::tgamma(x + 1.0);
}

// max
// min
// piecewise

// quotient
double sbmlmath::quotient(double numer, double denom)
{
    return std::trunc(numer / denom);
}
