/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include <tigon/Representation/Functions/CTP/CTPProblems.h>


#include <boost/math/constants/constants.hpp>
using namespace boost::math::constants;
using namespace Tigon;

namespace CTP {

TVector<double> CTP1(const TVector<double> &x)
{
    double g = 1.0 + x[1];

    TVector<double> y(4, 0.0);

    // objectives
    y[0] = x[0];
    y[1] = g*std::exp(-x[0]/g);

    // contraints
    y[2] = (-1.0)*(y[1] - (0.858*std::exp(-0.541*y[0])));
    y[3] = (-1.0)*(y[1] - (0.728*std::exp(-0.295*y[0])));

    return y;
}

TVector<double> CTP2(const TVector<double> &x)
{
    double theta = -0.2*pi<double>();
    double a = 0.2;
    double b = 10.0;
    double c = 1.0;
    double d = 6.0;
    double e = 1.0;
    double g = 1.0 + x[1];

    TVector<double> y(3, 0.0);

    // objectives
    y[0] = x[0];
    y[1] = g*(1.0-x[0]/g);

    double exp1 = (y[1]-e)*std::cos(theta) - y[0]*std::sin(theta);
    double exp2 = (y[1]-e)*std::sin(theta) + y[0]*std::cos(theta);
    exp2 = b*pi<double>()*std::pow(exp2,c);
    exp2 = std::abs(std::sin(exp2));
    exp2 = a*std::pow(exp2,d);

    // contraint
    y[2] = (-1.0)*(exp1-exp2);

    return y;
}

TVector<double> CTP3(const TVector<double> &x)
{
    double theta = -0.2*pi<double>();
    double a = 0.1;
    double b = 10.0;
    double c = 1.0;
    double d = 0.5;
    double e = 1.0;
    double g = 1.0 + x[1];

    TVector<double> y(3, 0.0);

    // objectives
    y[0] = x[0];
    y[1] = g*(1.0-x[0]/g);

    double exp1 = (y[1]-e)*std::cos(theta) - y[0]*std::sin(theta);
    double exp2 = (y[1]-e)*std::sin(theta) + y[0]*std::cos(theta);
    exp2 = b*pi<double>()*std::pow(exp2,c);
    exp2 = std::abs(std::sin(exp2));
    exp2 = a*std::pow(exp2,d);

    // contraint
    y[2] = (-1.0)*(exp1-exp2);

    return y;
}

TVector<double> CTP4(const TVector<double> &x)
{
    double theta = -0.2*pi<double>();
    double a = 0.75;
    double b = 10.0;
    double c = 1.0;
    double d = 0.5;
    double e = 1.0;
    double g = 1.0 + x[1];

    TVector<double> y(3, 0.0);

    // objectives
    y[0] = x[0];
    y[1] = g*(1.0-x[0]/g);

    double exp1 = (y[1]-e)*std::cos(theta) - y[0]*std::sin(theta);
    double exp2 = (y[1]-e)*std::sin(theta) + y[0]*std::cos(theta);
    exp2 = b*pi<double>()*std::pow(exp2,c);
    exp2 = std::abs(std::sin(exp2));
    exp2 = a*std::pow(exp2,d);

    // contraint
    y[2] = (-1.0)*(exp1-exp2);

    return y;
}

TVector<double> CTP5(const TVector<double> &x)
{
    double theta = -0.2*pi<double>();
    double a = 0.1;
    double b = 10.0;
    double c = 2.0;
    double d = 0.5;
    double e = 1.0;
    double g = 1.0 + x[1];

    TVector<double> y(3, 0.0);

    // objectives
    y[0] = x[0];
    y[1] = g*(1.0-x[0]/g);

    double exp1 = (y[1]-e)*std::cos(theta) - y[0]*std::sin(theta);
    double exp2 = (y[1]-e)*std::sin(theta) + y[0]*std::cos(theta);
    exp2 = b*pi<double>()*std::pow(exp2,c);
    exp2 = std::abs(std::sin(exp2));
    exp2 = a*std::pow(exp2,d);

    // contraint
    y[2] = (-1.0)*(exp1-exp2);

    return y;
}

TVector<double> CTP6(const TVector<double> &x)
{
    double theta = 0.1*pi<double>();
    double a = 40.0;
    double b = 0.5;
    double c = 1.0;
    double d = 2.0;
    double e = -2.0;
    double g = 1.0 + x[1];

    TVector<double> y(3, 0.0);

    // objectives
    y[0] = x[0];
    y[1] = g*(1.0-x[0]/g);

    double exp1 = (y[1]-e)*std::cos(theta) - y[0]*std::sin(theta);
    double exp2 = (y[1]-e)*std::sin(theta) + y[0]*std::cos(theta);
    exp2 = b*pi<double>()*std::pow(exp2,c);
    exp2 = std::abs(std::sin(exp2));
    exp2 = a*std::pow(exp2,d);

    // contraint
    y[2] = (-1.0)*(exp1-exp2);

    return y;
}

TVector<double> CTP7(const TVector<double> &x)
{
    double theta = -0.05*pi<double>();
    double a = 40.0;
    double b = 5.0;
    double c = 1.0;
    double d = 6.0;
    double e = 0.0;
    double g = 1.0 + x[1];

    TVector<double> y(3, 0.0);

    // objectives
    y[0] = x[0];
    y[1] = g*(1.0-x[0]/g);

    double exp1 = (y[1]-e)*std::cos(theta) - y[0]*std::sin(theta);
    double exp2 = (y[1]-e)*std::sin(theta) + y[0]*std::cos(theta);
    exp2 = b*pi<double>()*std::pow(exp2,c);
    exp2 = std::abs(std::sin(exp2));
    exp2 = a*std::pow(exp2,d);

    // contraint
    y[2] = (-1.0)*(exp1-exp2);

    return y;
}

}  // namespace CTP

