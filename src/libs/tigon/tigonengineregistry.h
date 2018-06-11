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

#pragma once

/*!
 * \brief Macro for ipset/ifunction factory. Include this file if you want to register your
 *        operator/algorithm/function to tigon engine.
 *        To registry a operator/algorithm, include <tigon/tigonengineregistry.h>
 *        and then at the end of your cpp file, append the following macro
 *        REGISTER_IPSET_FACTORY(nameSpace, className), e.g.
 *        REGISTER_IPSET_FACTORY(Operators, RandomInit)
 *        registers Tigon::Operators::RandomInit class.
 *
 *        To registry a function, include <tigon/tigonengineregistry.h>
 *        and then at the end of your cpp file, append the following macro
 *        REGISTER_IFUNCTION_FACTORY(className), e.g.
 *        REGISTER_IFUNCTION_FACTORY(DTLZ1)
 *        registers Tigon::Representation::DTLZ1 class.
 */
#include <tigon/Engine/TigonOperatorFactory.h>
#include <tigon/Engine/IPSetFactory.h>
#include <tigon/Engine/TigonFunctionFactory.h>
#include <tigon/Engine/IFunctionFactory.h>
#define REGISTER_IPSET_FACTORY(nameSpace, className) \
    namespace Tigon { \
    namespace Representation { \
    class IPSet; \
    } \
    } \
    class LIGER_TIGON_EXPORT className##Factory : public Tigon::IPSetFactory \
    { \
    public: \
        className##Factory() \
        { \
            Tigon::TigonOperatorFactory::instance()->registerOperatorFactory( \
                                         "Tigon::"#nameSpace"::"#className, this); \
        } \
        ~className##Factory() \
        { \
          \
        } \
        Tigon::Representation::IPSet* create(Tigon::Representation::IPSet* parent) \
        { \
            return new Tigon::nameSpace::className(parent); \
        } \
    }; \
    static className##Factory global_##className##Factory;

/// Factory method for functions
#define FUNCTION_FACTORY(className) \
    namespace Tigon { \
    namespace Representation { \
    class IFunction; \
    } \
    } \
    class LIGER_TIGON_EXPORT className##Factory : public Tigon::IFunctionFactory \
    { \
    public: \
        className##Factory() \
        { \
            Tigon::TigonFunctionFactory::instance()->registerFunctionFactory( \
                                         "Tigon::Representation::"#className, this); \
        } \
        ~className##Factory() \
        { \
          \
        } \
        Tigon::Representation::IFunction* create() \
        { \
            return new Tigon::Representation::className(); \
        } \
    };

#define REGISTER_IFUNCTION_FACTORY(className) \
    FUNCTION_FACTORY(className) \
    static className##Factory global_##className##Factory;
