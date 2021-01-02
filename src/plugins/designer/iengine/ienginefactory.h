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
#ifndef IENGINEFACTORY_H
#define IENGINEFACTORY_H

#include <designer/designer_global.h>
#include <QObject>

namespace Designer {

class IEngine;

class DESIGNER_EXPORT IEngineFactory : public QObject
{
    Q_OBJECT

public:
    IEngineFactory(QObject* parent = 0) : QObject(parent) {}

    virtual IEngine* createEngine() = 0;
};

} // namespace Designer

#endif // IENGINEFACTORY_H
