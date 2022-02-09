/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
**
****************************************************************************/

#include <tigon/Representation/Functions/SingleObjective/ConstrainedProblems.h>
#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;
using namespace Tigon;

namespace ConstrainedProblems {

TVector<double> CBranin(const TVector<double> &x)
{
    TVector<double> b1 {-5.0, 10.0};
    TVector<double> b2 {0.0,  15.0};

    // rescale x1 from [0,1] to [-5, 10]
    // rescale x2 from [0,1] to [0, 15]
    double x1 = x[0] * (b1[1]-b1[0]) + b1[0];
    double x2 = x[1] * (b2[1]-b2[0]) + b2[0];
    double PI = pi<double>();
    double a = 1.0;
    double b = 5.1/(4.0*PI*PI);
    double c = 5.0/PI;
    double r = 6.0;
    double s = 10.0;
    double t = 1.0/(8.0*PI);

    double exp1 = x2 - b*x1*x1 + c*x1 - r;
    double exp2 = (1.0 - t)*std::cos(x1);

    TVector<double> y(2, 0.0);

    // objective
    y[0] = a*exp1*exp1 + s*exp2 + s;

    // contraint (use original scale)
    y[1] = 0.2 - x[0]*x[1];

    return y;
}

TVector<double> CBraninM1(const TVector<double> &x)
{
    TVector<double> b1 {-5.0, 10.0};
    TVector<double> b2 {0.0,  15.0};

    // rescale x1 from [0,1] to [-5, 10]
    // rescale x2 from [0,1] to [0, 15]
    double x1 = x[0] * (b1[1]-b1[0]) + b1[0];
    double x2 = x[1] * (b2[1]-b2[0]) + b2[0];
    double PI = pi<double>();
    double b = 5.1/(4.0*PI*PI);
    double c = 5.0/PI;
    double r = 6.0;
    double s = 10.0;
    double t = 1.0/(8.0*PI);

    double exp1 = x2 - b*x1*x1 + c*x1 - r;
    double exp2 = (1.0 - t)*std::cos(x1) + 1.0; // adds 1

    TVector<double> y(2, 0.0);

    // objective
    y[0] = exp1*exp1 + s*exp2 + 5.0*x1; // replaces s by 5x1

    // constraint (use original scale)
    y[1] = 0.2 - x[0]*x[1];

    return y;
}

TVector<double> Camel(const TVector<double> &x)
{
    double exp1 = ((4 - 2.1*std::pow(x[0],2) + (std::pow(x[0],4))/3)*std::pow(x[0],2));
    double exp2 = std::pow((1.5*x[1] - (std::cos(31*x[1]))/6),2);

    TVector<double> y(2, 0.0);

    // objectives
    y[0] = exp1 + x[0]*x[1] - 4 + 4*std::pow(x[1],2) ;

    // contraints
    y[1] = 1.5 - exp2 - x[0];

    return y;
}

TVector<double> Sasena(const TVector<double> &x)
{
    TVector<double> y(4, 0.0);

    // objectives
    y[0] = - std::pow((x[0] - 1.0), 2.0) - std::pow((x[1] - 0.5), 2.0);

    // contraints
    y[1] = std::pow((x[0] - 3.0), 2.0) +
            (std::pow((x[1] + 2.0), 2.0))*std::exp(-(std::pow(x[1], 7.0))) - 12.0;
    y[2] = 10.0*x[0] + x[1] - 7.0;
    y[3] = std::pow((x[0] - 0.5), 2.0) + std::pow((x[1] - 0.5), 2.0) - 0.2;
    return y;
}

TVector<double> BraninGomez(const TVector<double> &x)
{
    double exp1 = x[1] - ((5.1*std::pow(x[0],2))/(4*std::pow(pi<double>(),2))) + (5*x[0]/pi<double>()) - 6;
    double exp2 = 10*(1 - 1/(8*pi<double>()))*std::cos(x[0]);
    double exp3 = ((4 - 2.1*std::pow(x[0],2) + (std::pow(x[0],4))/3)*std::pow(x[0],2));

    TVector<double> y(2, 0.0);

    // objectives
    y[0] = std::pow(exp1,2) + exp2 + 1/(8*pi<double>());

    // contraints
    y[1] = 6 - exp3 - x[0]*x[1] + (4 - 4*std::pow(x[1],2))*std::pow(x[1],2) - 3*std::sin(6*(1 - x[0])) - 3*std::sin(6*(1 - x[1]));

    return y;
}

}  // namespace ConstrainedProblems
