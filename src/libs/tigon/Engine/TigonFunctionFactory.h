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
#ifndef TIGONFUNCTIONFACTORY_H
#define TIGONFUNCTIONFACTORY_H
#include <tigon/tigon_global.h>
#include <tigon/Engine/IFunctionFactory.h>
#include <map>

namespace Tigon {

class LIGER_TIGON_EXPORT TigonFunctionFactory
{
public:
    static TigonFunctionFactory* instance();
    ~TigonFunctionFactory();
    /*!
     * \brief createOperator
     * \param Name of the operator, including namespaces
     * \return Function
     */
    Representation::IFunction* createFunction(const TString &className);
    /*!
     * \brief This function is used to register a specific ifunction facotry to the
     *        registery.
     * \param name Name of the ifunction Object.
     * \param fact Pointer to the corresponding factory.
     */
    void registerFunctionFactory(const TString &name, IFunctionFactory* fact);
    IFunctionFactory* unregisterFactory(const TString &name);

    /*!
     * \brief registeredFunctions
     * \return Names of all registered functions
     */
    TStringList registeredFunctions() const;

private:
    TigonFunctionFactory();
    /*!
     * \brief Factory registry
     */
    std::map<TString, IFunctionFactory*>  m_ifunctionFactoryRegistry;
};

}
#endif // TIGONFUNCTIONFACTORY_H
