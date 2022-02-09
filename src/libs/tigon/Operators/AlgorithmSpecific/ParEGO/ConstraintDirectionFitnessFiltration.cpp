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
#include <tigon/Operators/AlgorithmSpecific/ParEGO/ConstraintDirectionFitnessFiltration.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>

#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Utils/NormalisationUtils.h>

#include <tigon/ExceptionHandling/TException.h>


namespace Tigon {
namespace Operators {

ConstraintDirectionFitnessFiltration::ConstraintDirectionFitnessFiltration()
{
    initialise();
}

ConstraintDirectionFitnessFiltration::
ConstraintDirectionFitnessFiltration(Tigon::Representation::IPSet* ipset)
    : DirectionFitnessFiltration(ipset)
{
    initialise();
}

ConstraintDirectionFitnessFiltration::~ConstraintDirectionFitnessFiltration()
{

}

void ConstraintDirectionFitnessFiltration::evaluateNode()
{
    int nConstraints = constraintVecSize();
    if(nConstraints==0) {
        DirectionFitnessFiltration::evaluateNode();
        return;
    }

    ISet* iSet  = inputSet(0);
    if(iSet == nullptr) {
        // ERROR
        return;
    }
    int sSize = iSet->size();


    ISet* oSet = outputSet(0);
    if(oSet == nullptr) {
        oSet = appendOutputSet();
    } else {
        oSet->clearMappings();
    }

    if((TP_maxSolutions() >= sSize) || (TP_maxSolutions() <= 0)) {
        oSet->append(iSet);
        return;
    }

    // At this stage the number of solutions in the input set exceeds the
    // maximum number of solutions

    // Count number of feasible solutions
    int nFeasibleSolutions = 0;
    for(auto sol : iSet->all()) {
        if(sol->isFeasible()) {
            nFeasibleSolutions++;
        }
    }

    /// 1. All solutions are feasible (use DirectionFitnessFiltration criterion)
    if(sSize==nFeasibleSolutions) {
        DirectionFitnessFiltration::evaluateNode();
        return;
    }

    // At this stage there are infeasibile solutions

    TVector<double> threshVec  = IElementVecToRealVec(thresholdVec());
    int halfMax = static_cast<int>(std::ceil(TP_maxSolutions()/2.0));

    /// 2. All solutions are infeasible
    if(nFeasibleSolutions == 0) {

        // Select half of the required solutions based on how infeasible they are
        TVector<int> sortIdx = sortViolation(iSet, threshVec);

        for(auto idx : mid(sortIdx, 0, halfMax)) {
            oSet->append(iSet->at(idx));
        }

        // keep the rest of the solutions
        ISet* tempSet = new ISet();
        for(auto idx : mid(sortIdx, halfMax)) {
            tempSet->append(iSet->at(idx));
        }

        // Apply criterion from DirectionFitnessFiltration to remaining solutions
        int remaining = TP_maxSolutions() - oSet->size();
        TVector<int> selIdx = selection(tempSet, remaining);
        for(auto idx : selIdx) {
            oSet->append(tempSet->at(idx));
        }
        delete tempSet;
        return;
    }

    /// 3. Mix of feasible and infeasible solutions
    int nInfeasibleSolutions = sSize - nFeasibleSolutions;

    // Create two sets, one containing feasible solutions and the other
    // containing infeasible solutions
    ISet* feasibleSolutions = new ISet();
    ISet* inFeasibleSolutions = new ISet();
    for(auto sol : iSet->all()) {
        if(sol->isFeasible()) {
            feasibleSolutions->append(sol);
        } else {
            inFeasibleSolutions->append(sol);
        }
    }

    // A) Number of infeasible solutions is higher than or equal to halfMax, and
    //    number of feasible solutions ishigher than or equal to halfMax
    if( (nInfeasibleSolutions >= halfMax) && (nFeasibleSolutions >= halfMax) )
    {
        if(nFeasibleSolutions == halfMax) {
            // select all feasible solutions
            oSet->append(feasibleSolutions);
        } else {
            // From the feasible set select halfMax solutions
            // based on criterion from DirectionFitnessFiltration
            TVector<int> selIdx = selection(feasibleSolutions, halfMax);
            for(auto idx : selIdx) {
                oSet->append(feasibleSolutions->at(idx));
            }
        }

        // From the set of infeasible solutions select the remaining solutions
        // based on how infeasible they are (less infeasible first)
        int remaining = TP_maxSolutions() - oSet->size();
        TVector<int> sortIdx = sortViolation(inFeasibleSolutions, threshVec);
        for(auto idx : mid(sortIdx, 0, remaining)) {
            oSet->append(inFeasibleSolutions->at(idx));
        }
    }
    // B) Number of infeasible solutions is less than or equal to halfMax, and
    //    the number of feasible solutions is higher than or equal to halfMax
    else if( (nInfeasibleSolutions <= halfMax) && (nFeasibleSolutions >= halfMax) )
    {
        // Select all infeasible solutions
        oSet->append(inFeasibleSolutions);

        // The remaining solutions are chosen from the feasible set
        // based on criterion from DirectionFitnessFiltration
        int remaining = TP_maxSolutions() - oSet->size();
        TVector<int> selIdx = selection(feasibleSolutions, remaining);
        for(auto idx : selIdx) {
            oSet->append(feasibleSolutions->at(idx));
        }
    }
    // C) Number of infeasible solutions is higher than or equal to halfMax, and
    //    the number of feasible solutions is less than or equal to halfMax
    else if( (nInfeasibleSolutions >= halfMax) && (nFeasibleSolutions <= halfMax) )
    {
        // Select all feasible solutions
        oSet->append(feasibleSolutions);

        // From the set of infeasible solutions select the remaining solutions
        // based on how infeasible they are (less infeasible first)
        int remaining = TP_maxSolutions() - oSet->size();
        TVector<int> sortIdx = sortViolation(inFeasibleSolutions, threshVec);
        for(auto idx : mid(sortIdx, 0, remaining)) {
            oSet->append(inFeasibleSolutions->at(idx));
        }
    } else {
        throw TException(this->className(), UnrecognisedOption);
    }

    delete feasibleSolutions;
    delete inFeasibleSolutions;
}

TVector<int> ConstraintDirectionFitnessFiltration::
sortViolation(ISet* set, const TVector<double>& threshVec)
{
    TVector<double> cViolation;
    cViolation.reserve(set->size());
    for(auto sol : set->all()) {
        double violation = solutionConstraintViolation(sol, threshVec);
        cViolation.push_back(violation);
    }
    return indSort(cViolation);
}

void ConstraintDirectionFitnessFiltration::initialise()
{

}

TString ConstraintDirectionFitnessFiltration::name()
{
    return TString("Constraint Direction Fitness Filtration");
}

TString ConstraintDirectionFitnessFiltration::description()
{
    return TString("Sorts the output set according to fitness and "
                   "direction of the solutions.\n Half of the "
                   "requested size is sorted according to fitness. "
                   "the remaining solutions are sorted according to "
                   "closeness of their direction vector to the current "
                   "direction vector.");
}

} // namespace Operators
} // namespace Tigon
