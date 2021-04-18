#ifndef TESTPROB_H
#define TESTPROB_H

/*=============================================
 * Do NOT change the following lines of codes
 *=============================================*/
#include <tigon/Tigon.h>

using namespace Tigon;
using namespace Tigon::Representation;

class TestProb : public IFunction
{
public:
    TestProb();
    TestProb(const TestProb& func)
    {
        TP_defineNInputs(func.TP_nInputs());
        TP_defineNOutputs(func.TP_nOutputs());
        createFunctionProperties(func.name(), func.description(), func.type());
    }

    ~TestProb() { }

    void evaluate(const TVector<IElementSPtr> &inputs, const TVector<IElementSPtr> &outputs);

private:
    void defineInputPrpts();
    void defineOutputPrpts();
};

// Exported functions
TIGON_FUNCTION_EXPORT IFunction* funcFromSharedLib()
{
    TestProb* func = new TestProb();
    return func;
}

/*=============================================
 * Do NOT change the above lines of codes
 *=============================================*/
#endif // TESTPROB_H
