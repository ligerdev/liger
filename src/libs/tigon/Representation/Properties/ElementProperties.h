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
#ifndef ELEMENTPROPERTIES_H
#define ELEMENTPROPERTIES_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {
class IElement;
class ElementPropertiesFactory;
using namespace std;


class LIGER_TIGON_EXPORT ElementProperties
{
public:
    ElementProperties();
    ElementProperties(const ElementProperties& el)
        : d(el.d)
    {}
    ~ElementProperties();

    void    setIdx(int idx);
    void    setType(ElementType type);
    void    setUnits(const TString &unit);
    void    setOptimizationType(OptimizationType type);
    int              idx()         const;
    TString          ID()          const;
    TString          name()        const;
    TString          unit()       const;
    TString          description() const;
    ElementType      type()        const;
    OptimizationType optimizationType() const;

private:
    friend class ElementPropertiesFactory;

    // The first three constructors are here for completeness, however,
    // only the last is being used, i.e. (int elidx, TString name, TString desc)
    ElementProperties(int elidx);
    ElementProperties(int elidx, const TString &name);
    ElementProperties(int elidx, const TString &name,
                      const TString &desc);
    ElementProperties(int elidx, const TString &name,
                      const TString &desc, ElementType type);
    ElementProperties(int elidx, const TString &name,
                      const TString &desc, ElementType type,
                      OptimizationType otype);

    void setElementID(const TString &id);

    ElementPropertiesDataSPtr d;
};

} // namespace Representation
} // namespace Tigon

#endif // ELEMENTPROPERTIES_H
