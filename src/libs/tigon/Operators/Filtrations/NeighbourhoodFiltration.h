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
#ifndef NEIGHBOURHOODFILTRATION_H
#define NEIGHBOURHOODFILTRATION_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT NeighbourhoodFiltration : public IFiltration
{   
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    //READ(NeighbourhoodCriterion, TP_neighbourhoodCriterion)
    READ(NeighbourhoodRadius,  TP_neighbourhoodRadius)
    READ(MaxSolutions, TP_maxSolutions)
    READ(NeighbourhoodSize, TP_neighbourhoodSize)
    READ(IsClearOutputSets,  TP_isClearOutputSets)
    //READ(DistanceMeasureSelection,  TP_distanceMeasure)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    //WRITE(NeighbourhoodCriterion, Tigon::NeighbourhoodType,TP_defineNeighbourhoodCriterion)
    WRITE(NeighbourhoodRadius, double,TP_defineNeighbourhoodRadius)
    WRITE(MaxSolutions, int,TP_defineMaxSolutions)
    WRITE(NeighbourhoodSize, int,TP_defineNeighbourhoodSize)
    WRITE(IsClearOutputSets, bool ,TP_defineIsClearOutputSets)
    //WRITE(DistanceMeasureSelection, Tigon::DistanceMeasure,TP_defineDistanceMeasure)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::NeighbourhoodFiltration)

public:
    NeighbourhoodFiltration();
    NeighbourhoodFiltration(Tigon::Representation::IPSet* ipset);
    ~NeighbourhoodFiltration();

    void initialise();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    /**************/
    /* Properties */
    /**************/

    void TP_defineNeighbourhoodCriterion(NeighbourhoodType criterion);
    NeighbourhoodType TP_neighbourhoodCriterion() const;

    // properties for neighbourhood radius criterion
    void  TP_defineNeighbourhoodRadius(double r);
    double TP_neighbourhoodRadius()        const;

    void  TP_defineMaxSolutions(int n);
    int   TP_maxSolutions()      const;

    // property for neighbourhood size criterion
    void  TP_defineNeighbourhoodSize(int nSize);
    int   TP_neighbourhoodSize() const;

    void  TP_defineIsClearOutputSets(bool b);
    bool  TP_isClearOutputSets() const;

    void TP_defineDistanceMeasure(DistanceMeasure distance);
    DistanceMeasure TP_distanceMeasure() const;

private:

    TString m_name;
    TString m_description;

    NeighbourhoodType m_neighbourhoodCriterion;
    double            m_neighbourhoodRadius;
    int              m_maxSolutions;
    int              m_neighbourhoodSize;
    bool             m_isClearOutputSets;
    DistanceMeasure  m_distanceMeasure;

};

} // namespace Operators
} // namespace Tigon

#endif // NEIGHBOURHOODFILTRATION_H
