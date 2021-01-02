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
#ifndef PROBLEMPROPERTIESFACTORY_H
#define PROBLEMPROPERTIESFACTORY_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {
class ProblemProperties;

class LIGER_TIGON_EXPORT ProblemPropertiesFactory
{
public:
    static ProblemPropertiesFactory* instance();
    ProblemProperties createProblemProperties();
    ProblemProperties createProblemProperties(TString name, TString desc);
    ProblemProperties createProblemProperties(TString name, TString desc, ProblemType ptype);

private:
    ProblemPropertiesFactory();
    ~ProblemPropertiesFactory();

    static ProblemPropertiesFactory* m_instance;

};

} // namespace Represenation
} // namespace Tigon

#endif // PROBLEMPROPERTIESFACTORY_H
