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
#ifndef SURROGATEBASEDOPTIMIZERWITHCONSTRAINTHANDLING_H
#define SURROGATEBASEDOPTIMIZERWITHCONSTRAINTHANDLING_H
#include <tigon/Operators/Composites/SurrogateBasedOptimizer.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT ConstrainedSurrogateBasedOptimizer
        : public SurrogateBasedOptimizer
{
    HANDLE_READ_PROPERTIES_BEGIN(IComposite)
    READ(DisableConstraintHandling, TP_disableConstraintHandling)
    READ(NumberIterations, TP_numberIterations)
    READ(MultiObjectivization, TP_multiObjectivization)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IComposite)
    WRITE(DisableConstraintHandling, bool, TP_defineDisableConstraintHandling)
    WRITE(NumberIterations, int, TP_defineNumberIterations)
    WRITE(MultiObjectivization, bool, TP_defineMultiObjectivization)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::ConstrainedSurrogateBasedOptimizer)

public:
    ConstrainedSurrogateBasedOptimizer();
    ConstrainedSurrogateBasedOptimizer(Representation::IPSet* ipset);
    ~ConstrainedSurrogateBasedOptimizer();

    void evaluateNode();

    // Properties
    bool TP_disableConstraintHandling() const;
    void TP_defineDisableConstraintHandling(bool flag);

    int  TP_numberIterations() const;
    void TP_defineNumberIterations(int iter);

    bool TP_multiObjectivization() const;
    void TP_defineMultiObjectivization(bool isMultiObjectivizationUsedOrNot);

    // Information about the node
    TString     name();
    TString     description();

private:
    void initialise();

    ProblemSPtr setupSingleObjective_CEIProblem(
            IFunctionSPtr ei, const TVector<IFunctionSPtr>& vpf);
    ProblemSPtr setupMultiObjective_CEIProblem(
            IFunctionSPtr ei, const TVector<IFunctionSPtr>& vpf);

    /*
     * Runs NSGA-II for a fixed number of iterations
     */
    IMappingSPtr runMultiObjOptimizer(ProblemSPtr prob);

    bool m_disableConstraintHandling;
    int  m_numberIterations;
    bool m_multiObjectivization;
};

} // namespace Operators
} // namespace Tigon

#endif // SURROGATEBASEDOPTIMIZERWITHCONSTRAINTHANDLING_H
