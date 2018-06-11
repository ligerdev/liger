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
#ifndef ELEMENTPROPERTIESFACTORY_H
#define ELEMENTPROPERTIESFACTORY_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {
class ElementProperties;

class LIGER_TIGON_EXPORT ElementPropertiesFactory
{
public:
    static ElementPropertiesFactory* instance();
    ElementProperties createElementProperties(int idx);
    ElementProperties createElementProperties(int idx, const TString &name,
                                              const TString &desc);
    ElementProperties createElementProperties(int idx, const TString &name,
                                              const TString &desc,
                                              ElementType type);
    ElementProperties createElementProperties(int idx, const TString &name,
                                              const TString &desc,
                                              ElementType type,
                                              OptimizationType otype);

private:
    TString generateID(const TString &str) const;
    ElementPropertiesFactory();
    ~ElementPropertiesFactory();

    static ElementPropertiesFactory* m_instance;

};

} // namespace Represenation
} // namespace Tigon

#endif // ELEMENTPROPERTIESFACTORY_H
