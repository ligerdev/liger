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
#ifndef IPROCESSNODEFACTORY_H
#define IPROCESSNODEFACTORY_H

#include <designer/designer_global.h>
#include <designer/iengine/iprocessnode.h>
#include <QObject>

namespace Designer {

class DESIGNER_EXPORT IProcessNodeFactory : public QObject
{
    Q_OBJECT

public:
    IProcessNodeFactory(QObject* parent = 0) : QObject(parent) {}
    virtual ~IProcessNodeFactory() {}

    virtual IProcessNode* createProcessNode() { return new IProcessNode(); }
    virtual QString className() { return QString("Designer::IProcessNode"); }
    virtual bool isSelected() { return false; }
    virtual void unSelect()   { }

public slots:
    virtual void processButtonTriggered() {}
};

} // namespace Designer


#endif // IPROCESSNODEFACTORY_H
