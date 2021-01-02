/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef UNCERTAINTYKERNEL_H
#define UNCERTAINTYKERNEL_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Core/TVector.h>

namespace Tigon {
namespace Representation {
class BoxConstraintsData;
}
}

using namespace Tigon::Representation;

namespace CODeM {

class LIGER_TIGON_EXPORT UncertaintyKernel
{
public:
    UncertaintyKernel(Tigon::TVector<double> inputs,
                      Tigon::TVector<double> outputs,
                      BoxConstraintsDataSPtr box);
    UncertaintyKernel(Tigon::TVector<double> inputs,
                      Tigon::TVector<double> outputs,
                      BoxConstraintsDataSPtr box,
                      double lb,
                      double ub);
    UncertaintyKernel(Tigon::TVector<double> inputs,
                      Tigon::TVector<double> outputs,
                      BoxConstraintsDataSPtr box,
                      Tigon::TVector<double> ideal,
                      Tigon::TVector<double> antiIdeal);
    UncertaintyKernel(Tigon::TVector<double> inputs,
                      Tigon::TVector<double> outputs,
                      BoxConstraintsDataSPtr box,
                      double lb,
                      double ub,
                      Tigon::TVector<double> ideal,
                      Tigon::TVector<double> antiIdeal);
    UncertaintyKernel(Tigon::TVector<double> outputs,
                      double lb,
                      double ub,
                      Tigon::TVector<double> ideal,
                      Tigon::TVector<double> antiIdeal);
    ~UncertaintyKernel();

    double remoteness();
    double symmetry();
    double oComponent(int idx) const;
    double dComponent(int idx) const;

    Tigon::TVector<double> direction() const;

private:
    void defineIdealAndAntiIdeal(Tigon::TVector<double> ideal,
                                 Tigon::TVector<double> antiIdeal);
    // use normalised 2-norm values in objective space
    void defineDirectedObjectiveBoundaries(double lb, double ub);
    // set the lb to 0 and the ub to the directed boxed interval length
    void defineDirectedObjectiveBoundaries();

    // The design and objective values of the IMapping are not normalised
    Tigon::TVector<double>       m_inputs;
    Tigon::TVector<double>      m_outputs;
    BoxConstraintsDataSPtr  m_box;
    Tigon::TVector<double>        m_ideal;
    Tigon::TVector<double>    m_antiIdeal;
    Tigon::TVector<double>    m_direction;
    double              m_distance;
    // normalised 2-norm values
    double                    m_ub;
    double                    m_lb;

    void calcDirectionAndDistance();
};

} // namespace GECCO2016

#endif // UNCERTAINTYKERNEL_H
