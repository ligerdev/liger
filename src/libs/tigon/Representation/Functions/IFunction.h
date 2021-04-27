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
#pragma once

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

#include <tigon/Core/TObject.h>
#include <tigon/Representation/Properties/FunctionProperties.h>
#include <tigon/Representation/Properties/ElementProperties.h>

namespace Tigon {
namespace Representation {
class IElement;
class ElementPropertiesFactory;
class FunctionPropertiesFactory;
}
}

namespace Tigon {
namespace Representation {

/*!
 * \brief The IFunction class
 * \todo Refactor this class.
 */
class LIGER_TIGON_EXPORT IFunction : public TObject
{
    HANDLE_READ_PROPERTIES_BEGIN(TObject)
    READ(NumberOfInputs, TP_nInputs)
    READ(NumberOfOutputs, TP_nOutputs)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(TObject)
    WRITE(NumberOfInputs, int, TP_defineNInputs)
    WRITE(NumberOfOutputs, int, TP_defineNOutputs)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Representation::IFunction)

public:
    IFunction();
    virtual ~IFunction();

    // Properties
    int  TP_nInputs()         const;
    int  TP_nOutputs()        const;
    void TP_defineNInputs(int  num);
    void TP_defineNOutputs(int num);

    TString name()        const;
    TString description() const;
    TString ID()          const;
    FunctionType type()   const;
    TString typeStr()     const;
    TString path()        const;

    virtual void definePath(const TString &path);

    // Information about the function(s)
    FunctionProperties functionProperties()     const;
    TVector<ElementProperties> inputPrpts()     const;
    TVector<ElementProperties> outputPrpts()    const;
    BoxConstraintsDataSPtr     boxConstraints() const;
    TVector<OptimizationType>  optTypes()       const;

    virtual void evaluate(const TVector<IElementSPtr> &inputs,
                          const TVector<IElementSPtr> &outputs) = 0;
    /*!
     * \brief Batch function evaluation if supported. This function accepts
     * all input vectors, and performs function evaluation all at once.
     * \param Matrix of inputs. Every row represents a candidate input vector.
     * \param Matrix of outputs. Every row represents a new solution calculated
     * from the corresponding input vector.
     *
     * \note Not all function types support batch evaluation. Currently, only
     * MatlabFunction supports this feature to boost its performance.
     */
    virtual void batchEvaluate(const TVector<TVector<IElementSPtr> > &inputs,
                               const TVector<TVector<IElementSPtr> > &outputs);
    
    // A way to override the default properties
    void defineInputPrpts(const TVector<ElementProperties>  &iProp,
                          BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr());
    void defineOutputPrpts(const TVector<ElementProperties>  &oProp);
    void defineboxConstraints(BoxConstraintsDataSPtr box);

    bool parallelizable();
    bool isNumInputsModifiable() const;
    bool isNumOutputsModifiable() const;

protected:
    // Default properties defined by the concrete sub-classes
    virtual void defineInputPrpts()  = 0;
    virtual void defineOutputPrpts() = 0;

    void createFunctionProperties(TString funcName,
                                  TString funcDescription,
                                  FunctionType type = FunctionType::Other);
    void createInputProperties(const TStringList &elemNames,
                               const TStringList &elemDescription,
                               const TVector<ElementType> &elemVecType,
                               const TStringList &elemUnits,
                               const TVector<IElement> &lowerBounds,
                               const TVector<IElement> &upperBounds);
    void createOutputProperties(const TStringList &elemNames,
                                const TStringList &elemDescription,
                                const TVector<ElementType> &elemVecType,
                                const TStringList &elemUnits,
                                const TVector<OptimizationType> & oTypes);

    void defineParallelisable(bool p = true);
    void defineIsNumInputsModifiable(bool isNumInputsModifiable);
    void defineIsNumoutputsModifiable(bool isNumOutputsModifiable);


private:
    int                        m_nInputs;
    int                        m_nOutputs;
    TVector<ElementProperties> m_inputPrpts;
    TVector<ElementProperties> m_outputPrpts;
    FunctionProperties         m_funPrpts;
    BoxConstraintsDataSPtr     m_box;

    ElementPropertiesFactory*  m_elFact;
    FunctionPropertiesFactory* m_funPropFct;
    bool m_parallel;
    bool m_isNumInputsModifiable;
    bool m_isNumoutputsModifiable;
};

} // namespace Representation
} // namespace Tigon
