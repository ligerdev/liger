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
#ifndef FITNESSBASEDINIT_H
#define FITNESSBASEDINIT_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Initialisation/IInitialisation.h>


namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT FitnessBasedInit : public IInitialisation
{
    DECLARE_CLASS(Tigon::Operators::FitnessBasedInit)

public:
    FitnessBasedInit();
    FitnessBasedInit(Representation::IPSet* ipset);
    virtual ~FitnessBasedInit();

    void  defineOriginalSolutionsRatio(double ratio);
    double originalSolutionsRatio()            const;

    void evaluateNode();

    // Information about the node
    TString     name();
    TString     description();
    
private:
    void    initialise();
    double   m_existingRatio;
};

} // namespace Operators
} // namespace Tigon

#endif // FITNESSBASEDINIT_H
