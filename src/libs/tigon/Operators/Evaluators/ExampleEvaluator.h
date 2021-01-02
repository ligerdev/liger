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
#ifndef EXAMPLEEVALUATOR_H
#define EXAMPLEEVALUATOR_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Evaluators/IEvaluator.h>


// Qt Includes
#include <QString>
#include <QStringList>

namespace Tigon {
namespace Operators {
using namespace Tigon::Representation;

class LIGER_TIGON_EXPORT ExampleEvaluator : public IEvaluator
{
    Q_OBJECT

public:
    ExampleEvaluator();
    ExampleEvaluator(Tigon::Representation::IPSet *ipset);
    ~ExampleEvaluator();

    void evaluateNode();

    // Information about the node.
    QString     name();
    QString     description();
    QStringList propertyNames();
    QStringList propertyDescriptions();

private:
    QString m_name;
    QString m_description;
};

} // namespace Operators
} // namespace Tigon

#endif /* EXAMPLEEVALUATOR_H */
