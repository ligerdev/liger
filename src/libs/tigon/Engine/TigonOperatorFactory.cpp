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
#include <tigon/Engine/TigonOperatorFactory.h>
#include <tigon/Tigon.h>

namespace Tigon {

TigonOperatorFactory::TigonOperatorFactory()
{

}

TigonOperatorFactory* TigonOperatorFactory::instance()
{
    static TigonOperatorFactory instance;
    return &instance;
}

TigonOperatorFactory::~TigonOperatorFactory()
{
    m_ipsetFactoryRegistry.clear();
}

Representation::IPSet* TigonOperatorFactory::createOperator(const TString &name,
                                                            Representation::IPSet* parent)
{
    IPSet* op = 0;
    if(mapHasKey(m_ipsetFactoryRegistry, name)) {
        op = m_ipsetFactoryRegistry[name]->create(parent);
    }
    return op;
}

void TigonOperatorFactory::registerOperatorFactory(const TString &name,
                                                   IPSetFactory* fact)
{
    if((!name.empty()) && fact) {
        m_ipsetFactoryRegistry[name] = fact;
    }
}

} // namespace Tigon

