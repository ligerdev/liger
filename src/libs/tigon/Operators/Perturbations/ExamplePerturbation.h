/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef EXAMPLEPERTURBATION_H
#define EXAMPLEPERTURBATION_H
#include <tigon/Operators/Perturbations/IPerturbation.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT ExamplePerturbation : public IPerturbation
{
    Q_OBJECT

public:
    ExamplePerturbation();
    ExamplePerturbation(Tigon::Representation::IPSet* ipset);
    virtual ~ExamplePerturbation();

    void evaluateNode();

    // Information about the node.
    QString     name();
    QString     description();
    QStringList propertyNames();
    QStringList propertyDescriptions();

private:


};

} // namespace Operators
} // namespace Tigon

#endif // EXAMPLEPERTURBATION_H
