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
#include <qtigon/engine/qtigonenginefactory.h>
#include <qtigon/engine/qtigonengine.h>
#include <designer/iengine/iengine.h>

using namespace QTigon;

QTigonEngineFactory::QTigonEngineFactory()
{

}

Designer::IEngine* QTigonEngineFactory::createEngine()
{
    return new QTigonEngine();
}

