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
#ifndef FUNCTIONPROPERTIES_H
#define FUNCTIONPROPERTIES_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Properties/FunctionPropertiesData.h>

namespace Tigon {
namespace Representation {
class FunctionPropertiesFactory;
using namespace std;

class LIGER_TIGON_EXPORT FunctionProperties
{
public:
    FunctionProperties();
    FunctionProperties(const FunctionProperties& fp)
        : d(fp.d)
    {}
    ~FunctionProperties();

    TString          ID()          const;
    TString          name()        const;
    TString          description() const;

    FunctionType     type()        const;
    /*!
     * \brief  Type of the function:
     * \return TString: Internal, Matlab, SharedLib
     */
    TString          typeStr()        const;
    /*!
     * \brief Path of the function.
     * \return It returns the actual path of the function if the function has a
     * type of Matlab or SharedLib. Otherwise, return the class Name of this function,
     * e.g Tigon::Representation::DTLZ1.
     */
    TString          path()        const;
    void             setFunctionPath(const TString &path);

private:
    friend class FunctionPropertiesFactory;

    // The first two constructors are here for completeness, however,
    // only the last is being used, i.e. (int fpidx, const TString &name,
    // const TString &desc)
    FunctionProperties(const TString &name);
    FunctionProperties(const TString &name, const TString &desc,
                       FunctionType type = FunctionType::Other);

    void setFunctionID(const TString &id);

    FunctionPropertiesDataSPtr d;
};

} // namespace Representation
} // namespace Tigon

#endif // FUNCTIONPROPERTIES_H
