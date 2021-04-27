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
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Representation/Properties/FunctionPropertiesFactory.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Representation {

IFunction::IFunction()
    : m_nInputs(0)
    , m_nOutputs(0)
    , m_elFact(ElementPropertiesFactory::instance())
    , m_funPropFct(FunctionPropertiesFactory::instance())
    , m_parallel(false)
    , m_isNumInputsModifiable(true)
    , m_isNumoutputsModifiable(true)
{
    addProperty("NumberOfInputs", "Number of Inputs", getTType(int));
    addProperty("NumberOfOutputs", "Number of Outputs", getTType(int));
}

IFunction::~IFunction()
{

}

int IFunction::TP_nInputs() const
{
    return m_nInputs;
}

int IFunction::TP_nOutputs() const
{
    return m_nOutputs;
}

void IFunction::TP_defineNInputs(int num)
{
    if(num > 0) {
        m_nInputs = num;
    }
    defineInputPrpts();
}

void IFunction::TP_defineNOutputs(int num)
{
    if(num > 0) {
        m_nOutputs = num;
    }
    defineOutputPrpts();
}


FunctionProperties IFunction::functionProperties() const
{
    return m_funPrpts;
}

TString IFunction::name() const
{
    return m_funPrpts.name();
}

TString IFunction::description() const
{
    return m_funPrpts.description();
}

TString IFunction::ID() const
{
    return m_funPrpts.ID();
}

FunctionType IFunction::type() const
{
    return m_funPrpts.type();
}

TString IFunction::typeStr() const
{
    return m_funPrpts.typeStr();
}

TString IFunction::path() const
{
    return m_funPrpts.path();
}

void IFunction::definePath(const TString &path)
{
    m_funPrpts.setFunctionPath(path);
}

TVector<ElementProperties> IFunction::inputPrpts()  const
{
    return m_inputPrpts;
}

TVector<ElementProperties> IFunction::outputPrpts() const
{
    return m_outputPrpts;
}

BoxConstraintsDataSPtr IFunction::boxConstraints() const
{
    return m_box;
}

TVector<OptimizationType> IFunction::optTypes() const
{
    TVector<OptimizationType> res;
    std::transform(m_outputPrpts.cbegin(), m_outputPrpts.cend(),
                   std::back_inserter(res),
                   [](const ElementProperties& p){return p.optimizationType();});
    return res;
}

void IFunction::batchEvaluate(const TVector<TVector<IElementSPtr> > &inputs,
                              const TVector<TVector<IElementSPtr> > &outputs)
{
    T_UNUSED(inputs)
            T_UNUSED(outputs)

            throw TException(className(),
                             FunctionNotAvailable);
}

void IFunction::defineboxConstraints(BoxConstraintsDataSPtr box)
{
    if(box && (box->size() == TP_nInputs())) {
        m_box = box->clone();
    }
}

bool IFunction::parallelizable()
{
    // Parallel property is not locked, parallel property
    // should only be changed in the constructor
    return m_parallel;
}

void IFunction::defineInputPrpts(const TVector<ElementProperties> &iProp,
                                 BoxConstraintsDataSPtr box)
{
    if(m_nInputs != iProp.size()) {
        return;
    }

    m_inputPrpts = iProp;

    defineboxConstraints(box);
}

void IFunction::defineOutputPrpts(const TVector<ElementProperties> &oProp)
{
    if(m_nOutputs != oProp.size()) {
        return;
    }

    m_outputPrpts = oProp;
}

void IFunction::createFunctionProperties(TString funcName,
                                         TString funcDescription,
                                         FunctionType type)
{
    m_funPrpts = m_funPropFct->createFunctionProperties(funcName,
                                                        funcDescription,
                                                        type);
}

void IFunction::createInputProperties(const TStringList &elemNames,
                                      const TStringList &elemDescription,
                                      const TVector<ElementType> &elemVecType,
                                      const TStringList &elemUnits,
                                      const TVector<IElement> &lowerBounds,
                                      const TVector<IElement> &upperBounds)
{
    if(m_nInputs != elemNames.size()       ||
            m_nInputs != elemDescription.size() ||
            m_nInputs != elemVecType.size()     ||
            m_nInputs != elemUnits.size()) {
        return;
    }

    m_inputPrpts.clear();
    for(int i = 0; i < m_nInputs; i++) {
        ElementProperties newEle = m_elFact->createElementProperties(i,
                                                                     elemNames[i],
                                                                     elemDescription[i],
                                                                     elemVecType[i]);
        newEle.setUnits(elemUnits[i]);
        m_inputPrpts.push_back(newEle);
    }

    if(!m_box) {
        m_box = BoxConstraintsDataSPtr(new BoxConstraintsData);
    }
    m_box->defineLowerBounds(lowerBounds);
    m_box->defineUpperBounds(upperBounds);
}

void IFunction::createOutputProperties(const TStringList &elemNames,
                                       const TStringList &elemDescription,
                                       const TVector<ElementType> &elemVecType,
                                       const TStringList &elemUnits,
                                       const TVector<OptimizationType> &oTypes)
{
    if(m_nOutputs != elemNames.size() ||
            m_nOutputs != elemDescription.size() ||
            m_nOutputs != elemVecType.size() ||
            m_nOutputs != elemUnits.size() ||
            ((m_nOutputs != oTypes.size()) && (!oTypes.empty())) ) {
        return;
    }

    m_outputPrpts.clear();
    for(int i = 0; i < m_nOutputs; i++) {
        ElementProperties newEle =
                m_elFact->createElementProperties(i, elemNames[i],
                                                  elemDescription[i],
                                                  elemVecType[i], oTypes[i]);
        newEle.setUnits(elemUnits[i]);
        m_outputPrpts.push_back(newEle);
    }
}

//! Use in the constructor to indicate if the evaluate method can be parallelized
void IFunction::defineParallelisable(bool p)
{
    m_parallel = p;
}

bool IFunction::isNumOutputsModifiable() const
{
    return m_isNumoutputsModifiable;
}

void IFunction::defineIsNumoutputsModifiable(bool isNumoutputsModifiable)
{
    m_isNumoutputsModifiable = isNumoutputsModifiable;
}

bool IFunction::isNumInputsModifiable() const
{
    return m_isNumInputsModifiable;
}

void IFunction::defineIsNumInputsModifiable(bool isNumInputsModifiable)
{
    m_isNumInputsModifiable = isNumInputsModifiable;
}

} // namespace Representation
} // namespace Tigon
