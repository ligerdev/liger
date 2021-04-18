/*=============================================
 * Define your function in this file;
 *============================================= */

#include "testprob.h"

TestProb::TestProb()
{
    /*=============================================
     * Define the numbers of name, description, input and output - required
     *      name        : Name of the function
     *      description : Description of the function
     *      nInput      : Number of inputs, required
     *      nOutput     : Number of outputs, required
     *=============================================*/
    TString name("TestProb");
    TString description("Test problem for library loading");
    int nInput  = 3;
    int nOutput = 2;

    //=============================================
    TP_defineNInputs(nInput);
    TP_defineNOutputs(nOutput);
    createFunctionProperties(name, description, FunctionType::SharedLib);
}

void TestProb::defineInputPrpts()
{
    /*=============================================
     * Define your input (decision variables/parameters) in the following
     *      inputNames        : Input variable names,            required
     *      inputDescriptions : Input variable descriptions,     required
     *      typeVec         : Input variable types,            required
     *      inputUnits        : Input variable units,            required
     *      lowerBounds     : lowerbouds for input variables,  required
     *      upperBounds     : upperbounds for input variables, required
     *=============================================*/
    TStringList          inputNames;
    TStringList          inputDescriptions;
    TVector<ElementType> typeVec;
    TStringList          inputUnits;
    TVector<IElement>    lowerBounds;
    TVector<IElement>    upperBounds;

    /*=============================================
     * Do NOT touch the above lines of codes
     * Start implementing from here
     *=============================================*/

    for(int i = 0; i < TP_nInputs(); i++) {
        inputNames.push_back("Input_Var_" + std::to_string(i));
        typeVec.push_back(Tigon::RealType);
        inputUnits.push_back("");
        lowerBounds.push_back(IElement(0.0));
        upperBounds.push_back(IElement(1.0));
        if(i < TP_nOutputs()-1) {
            inputDescriptions.push_back("Direction variable");
        } else {
            inputDescriptions.push_back("Distance variable");
        }
    }

    /*=============================================
     * End implementing
     * Do Not touch the following lines of codes
     * =============================================*/
    createInputProperties(inputNames, inputDescriptions, typeVec, inputUnits,
                          lowerBounds, upperBounds);
}

void TestProb::defineOutputPrpts()
{
    /*=============================================
     * Define your output variables in the following
     *      outputNames        : Output variable names,            required
     *      outputDescriptions : Output variable descriptions,     required
     *      typeVec            : Output variable types,            required
     *      outputUnits        : Output variable units,            required
     *=============================================*/
    TStringList          outputNames;
    TStringList          outputDescriptions;
    TVector<ElementType> typeVec;
    TStringList          outputUnits;
    TVector<OptimizationType> optTypes;

    /*=============================================
     * Do NOT touch the above lines of codes
     * Start implementing from here
     *=============================================*/

    for(int i = 0; i < TP_nOutputs(); i++) {
        outputNames.push_back("Output_Var_" + std::to_string(i));
        outputDescriptions.push_back("Output_VarDesc_" + std::to_string(i));
        typeVec.push_back(Tigon::RealType);
        outputUnits.push_back("cm");
        optTypes.push_back(Minimization);
    }

    /*=============================================
     * End implementing
     * Do Not touch the following lines of codes
     * =============================================*/

    createOutputProperties(outputNames, outputDescriptions, typeVec, outputUnits, optTypes);
}

void TestProb::evaluate(const TVector<IElementSPtr> &inputs, const TVector<IElementSPtr> &outputs)
{
    /* =============================================
     * Implementation for evaluating functions
     * =============================================*/

    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs())
            && (TP_nInputs() > TP_nOutputs())) {
        int nVar = inputs.size();
        int nObj = outputs.size();
        TVector<double> iReal = IElementVecToRealVec(inputs);
        TVector<double> oReal(outputs.size());

        for(int i = 0; i < nObj; i++) {
            double sum = 0;
            for(int j = 0; j < nVar; j++) {
                sum = sum + iReal[j];
            }
            oReal[i] = sum / ((double) i+1);
        }

        for(int i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(oReal[i]);
        }
    }
}
