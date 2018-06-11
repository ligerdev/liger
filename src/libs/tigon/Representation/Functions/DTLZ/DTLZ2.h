#ifndef DTLZ2_H
#define DTLZ2_H

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT DTLZ2 : public IFunction
{
    DECLARE_CLASS(Tigon::Representation::DTLZ2)
public:
    DTLZ2();
    DTLZ2(const DTLZ2& func);
    ~DTLZ2();

    void evaluate(const TVector<IElementSPtr> &inputs, const TVector<IElementSPtr> &outputs);

private:
    void defineInputPrpts();
    void defineOutputPrpts();
};

} // namespace Representation
} // namespace Tigon

#endif // DTLZ2_H
