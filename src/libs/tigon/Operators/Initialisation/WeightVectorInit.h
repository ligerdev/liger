/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef WEIGHTVECTORINIT_H
#define WEIGHTVECTORINIT_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Initialisation/IInitialisation.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT WeightVectorInit : public IInitialisation
{
    HANDLE_READ_PROPERTIES_BEGIN(IInitialisation)
    READ(NumberPointsPerDimension, TP_numberPointsPerDimension)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IInitialisation)
    WRITE(NumberPointsPerDimension, int, TP_defineNumberPointsPerDimension)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::WeightVectorInit)

public:
    WeightVectorInit();
    WeightVectorInit(Representation::IPSet* ipset);
    ~WeightVectorInit();

    void initialise();

    void evaluateNode();

    // Information about the node
    TString     name();
    TString     description();

    /**************/
    /* Properties */
    /**************/

    // Number of points per dimension (H)
    void TP_defineNumberPointsPerDimension(int H);
    int TP_numberPointsPerDimension()   const;

    void TP_defineRegularisationMethod(Tigon::RegularisationApproach r);
    Tigon::RegularisationApproach TP_regularisationMethod() const;

    void TP_defineDistanceMeasure(DistanceMeasure distance);
    DistanceMeasure TP_distanceMeasure() const;

private:
    int                        m_numberPointsPerDimension;
    int                             m_fixedPopulationSize;
    Tigon::RegularisationApproach  m_regularisationMethod;
    TVector<TVector<double>>                     m_refSet;
    DistanceMeasure                     m_distanceMeasure;
};

} // namespace Operators
} // namespace Tigon

#endif // WEIGHTVECTORINIT_H
