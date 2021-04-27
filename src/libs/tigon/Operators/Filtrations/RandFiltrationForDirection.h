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
#ifndef RANDFILTRATIONFORDIRECTION_H
#define RANDFILTRATIONFORDIRECTION_H
#include <tigon/Operators/Filtrations/FiltrationForDirection.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT RandFiltrationForDirection : public FiltrationForDirection
{
    
    /*
    Q_PROPERTY(double RatioOfNewSolutions
               READ  TP_ratioOfNewSolutions
               WRITE TP_defineRatioOfNewSolutions)
    Q_PROPERTY(int   NewSolutionsPerOutputSet
               READ  TP_newSolutionsPerOutputSet
               WRITE TP_defineNewSolutionsPerOutputSet)
    */
    DECLARE_CLASS(Tigon::Operators::RandFiltrationForDirection)

public:
    RandFiltrationForDirection();
    RandFiltrationForDirection(Tigon::Representation::IPSet* ipset);
    virtual ~RandFiltrationForDirection();
    void    initialise();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    


    // Properties
    /*
    void  TP_defineRatioOfNewSolutions(double    r);
    double TP_ratioOfNewSolutions()           const;
    void  TP_defineNewSolutionsPerOutputSet(int n);
    int   TP_newSolutionsPerOutputSet()      const;
    */

private:
    TString m_name;
    TString m_description;

    /*
    double   m_ratioOfSolutions;
    int     m_newSolutionsPerOutputSet;
    */
};

} // namespace Operators
} // namespace Tigon

#endif // RANDFILTRATIONFORDIRECTION_H
