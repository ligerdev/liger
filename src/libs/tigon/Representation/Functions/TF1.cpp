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
#include <tigon/Representation/Functions/TF1.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>

REGISTER_IFUNCTION_FACTORY(TF1)

namespace Tigon {
namespace Representation {

TF1::TF1()
{
    TP_defineNInputs(5);
    TP_defineNOutputs(2);
    TString name("TF1");
    TString description("TF1 test problem. It is parametric two-objective "
                        "problem where the dimension of the decision space is "
                        "user-defined.\n"
                        "The problem contains a 2-dimensional parameter that "
                        "can take values from the set {0.0,0.1,...,1.0}.\n"
                        "The version of this problem in here allows the "
                        "parameter to take values in the interval [0,1]\n"
                        "The test problem is taken from:\n"
                        "H. Kaji, K. Ikeda and H. Kita, 'Acceleration of "
                        "parametric Multi-objective Optimization by an "
                        "initialization technique for Multi-Objective "
                        "Evolutionary Algorithms,' IEEE Congress on Evolutionary "
                        "Computation, 2008, pp. 2291-2297.");

    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
    defineIsNumoutputsModifiable(false);
}

TF1::TF1(const TF1& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
    defineIsNumoutputsModifiable(false);
}

TF1::~TF1()
{

}

void TF1::evaluate(const TVector<IElementSPtr> &inputs,
                     const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs()) &&
            (TP_nInputs() > 4) && (TP_nOutputs() == 2)) {

        int n = TP_nInputs()-2; // number of decision variables

        TVector<double> iReal = IElementVecToRealVec(inputs);
        TVector<double> x(iReal.begin(),iReal.begin()+n);
        TVector<double> p(iReal.begin()+n,iReal.end());

        double sum1=0.0;
        for(int i=0; i<2; i++) {
            sum1 += std::pow(x[i]-p[i]*p[i],2.0);
        }

        double sum2=0.0;
        double sum3=0.0;
        for(int i=3; i<n; i++) {
            sum2 += std::pow(x[i]-1.0,2.0);
            sum3 += std::pow(x[i]+1.0,2.0);
        }

        TVector<double> oReal(2);

        oReal[0] = 1.0/(n-2.0) * (sum1+sum2);
        oReal[1] = 1.0/(n-2.0) * (sum1+sum3);

        for(size_t i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(oReal[i]);
        }
    } else {
        throw TException(className(), IncorrectProblemFormatException);
    }
}

void TF1::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds(TP_nInputs());
    TVector<IElement>    upperBounds(TP_nInputs());

    for(int i=0; i<TP_nInputs()-2; i++) {
        lowerBounds[i] = IElement(RealType, -2.0);
        upperBounds[i] = IElement(RealType, 2.0);
    }

    for(int i=TP_nInputs()-2; i<TP_nInputs(); i++) {
        lowerBounds[i] = IElement(RealType, 0.0);
        upperBounds[i] = IElement(RealType, 1.0);
    }

    for(int i=0; i<TP_nInputs()-2; i++) {
        varNames.push_back("x" + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        varDescriptions.push_back("Decision variable n."+std::to_string(i));
    }

    // parameter
    for(int i=TP_nInputs()-2; i<TP_nInputs(); i++) {
        varNames.push_back("u"+ std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        varDescriptions.push_back("Parameter n."+std::to_string(i));
    }

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void TF1::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;

    varNames << "f1" << "f2";
    varDescriptions << "f1" << "f2";

    typeVec << RealType << RealType;
    varUnits << "" << "";
    TVector<OptimizationType> optTypes({Minimization, Minimization});
    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

} // namespace Representation
} // namespace Tigon
