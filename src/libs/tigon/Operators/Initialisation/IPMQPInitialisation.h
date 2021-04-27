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
#ifndef IPMQPINITIALISATION_H
#define IPMQPINITIALISATION_H
#include <tigon/Operators/Initialisation/IInitialisation.h>
#include <tigon/Representation/Constraints/LinearConstraintsData.h>

// Qt Includes
#include <QString>
#include <QStringList>

namespace Tigon {
namespace Representation {
class ElementProperties;
class ElementPropertiesFactory;
}
}

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IPMQPInitialisation : public IInitialisation
{
    Q_OBJECT

public:
    IPMQPInitialisation();
    IPMQPInitialisation(Tigon::Representation::IPSet* ipset);
    virtual ~IPMQPInitialisation();

    void evaluateNode();

    // Information about the node.
    QString     name();
    QString     description();
    QStringList propertyNames();
    QStringList propertyDescriptions();

private:
    ElementPropertiesFactory* m_fct;
};

} // namespace Operators
} // namespace Tigon

#endif // IPMQPINITIALISATION_H
