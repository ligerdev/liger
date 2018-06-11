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
#ifndef TIGONOPERATORFACTORY_H
#define TIGONOPERATORFACTORY_H

#include <tigon/tigon_global.h>
#include <tigon/Engine/IPSetFactory.h>

namespace Tigon {
namespace Representation {
class IPSet;
}

class LIGER_TIGON_EXPORT TigonOperatorFactory
{

public:
    static TigonOperatorFactory* instance();
    ~TigonOperatorFactory();
    /*!
     * \brief createOperator
     * \param name Name of the operator
     * \param parent Parent of the operator
     * \return Operator or Algorithm
     */
    Representation::IPSet* createOperator(const TString &name,
                                          Representation::IPSet* parent);
    /*!
     * \brief This function is used to register a specific ipset facotry to the
     *        registery.
     * \param name Name of the IPSet Object.
     * \param fact Pointer to the corresponding factory.
     */
    void registerOperatorFactory(const TString &name, IPSetFactory* fact);

private:
    TigonOperatorFactory();
    /*!
     * \brief Factory registry
     */
    TMap<TString, IPSetFactory*>  m_ipsetFactoryRegistry;
};

} // namespace Tigon
#endif // TIGONOPERATORFACTORY_H
