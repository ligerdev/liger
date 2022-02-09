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
#ifndef EXAMPLERANDSELECTION_H
#define EXAMPLERANDSELECTION_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT ExampleRandSelection : public IFiltration
{
    Q_OBJECT
    Q_PROPERTY(int PoolSize READ TP_poolSize WRITE TP_definePoolSize)

public:
    ExampleRandSelection();
    ExampleRandSelection(Tigon::Representation::IPSet* ipset);
    virtual ~ExampleRandSelection();

    void evaluateNode();

    // Information about the node.
    QString     name();
    QString     description();
    QStringList propertyNames();
    QStringList propertyDescriptions();

    // Properties
    void TP_definePoolSize(int n);
    int  TP_poolSize()   const;

private:
    int m_poolSize;
    QString m_name;
    QString m_description;

};

} // namespace Operators
} // namespace Tigon

#endif // EXAMPLERANDSELECTION_H
