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
#ifndef QTIGONENGINEFACTORY_H
#define QTIGONENGINEFACTORY_H
#include <designer/iengine/ienginefactory.h>
#include <qtigon/qtigon_global.h>
#include <qtigon/qtigonconstants.h>

namespace Designer{
class IEngine;
}

namespace QTigon {
class QTIGON_EXPORT QTigonEngineFactory : public Designer::IEngineFactory
{
public:
    QTigonEngineFactory();
    Designer::IEngine* createEngine();
};

}  // namespace QTigon

#endif // QTIGONENGINEFACTORY_H
