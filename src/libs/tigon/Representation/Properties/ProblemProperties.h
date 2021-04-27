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
#ifndef PROBLEMPROPERTIES_H
#define PROBLEMPROPERTIES_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Properties/ProblemPropertiesData.h>

namespace Tigon {
namespace Representation {
class ProblemPropertiesFactory;

class LIGER_TIGON_EXPORT ProblemProperties
{
public:
    ProblemProperties();
    ProblemProperties(const ProblemProperties& el);

    ~ProblemProperties();

    void  setType(ProblemType type);
    TString     ID()          const;
    TString     name()        const;
    TString     description() const;
    ProblemType type()        const;

private:
    friend class ProblemPropertiesFactory;
    // The first three constructors are here for completeness, however,
    // only the last is being used, i.e. (TString name, TString desc, ProblemType ptype)
    ProblemProperties(TString name);
    ProblemProperties(TString name, TString desc);
    ProblemProperties(TString name, TString desc, ProblemType ptype);

    void setProblemID(TString id);

    ProblemPropertiesDataSPtr d;
};

} // namespace Representation
} // namespace Tigon

#endif // PROBLEMPROPERTIES_H
