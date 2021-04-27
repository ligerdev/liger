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
#include <tigon/Operators/Transformations/Normalisation.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Problems/Problem.h>


namespace Tigon {
namespace Operators {

Normalisation::Normalisation()
{
    initialise();
}

Normalisation::Normalisation(Tigon::Representation::IPSet *ipset)
    : ITransformation(ipset)
{
    initialise();
}

Normalisation::~Normalisation()
{

}

void Normalisation::initialise()
{
    //m_propertyNames.push_back("Input bouds type");
    //m_propertyDescriptions.push_back("The method to define the input bounds:/n"
    //                              "from global box constraints, user defined "
    //                              "or extreme values in the set.");
    //m_propertyNames.push_back("Output bouds type");
    //m_propertyDescriptions.push_back("The method to define the output bounds:/n"
    //                              "from global ideal and anti ideal vectors, "
    //                              "user defined or extreme values in the set.");
    //m_propertyNames.push_back("Fitness bouds type");
    //m_propertyDescriptions.push_back("The method to define the scalar fitness bounds:/n"
    //                              "user defined or extreme values in the set.");

    addOutputTag(Tigon::TNormalised);

    m_inputBounds   = BoxConstraintsDataSPtr(new BoxConstraintsData);
    m_outputBounds  = BoxConstraintsDataSPtr(new BoxConstraintsData);
    m_fitnessBounds = BoxConstraintsDataSPtr(new BoxConstraintsData);

    TP_defineInputBoundsType(Tigon::UserDefinedBounds);
    TP_defineOutputBoundsType(Tigon::UserDefinedBounds);
    TP_defineFitnessBoundsType(Tigon::UserDefinedBounds);
}

void Normalisation::evaluateNode()
{
    clearOutputSets();

    while(hasNextInputSet()) {
        ISet* iSet = nextInputSet();
        ISet* oSet = appendOutputSet();

        // define the Generic Mappings
        if(iSet->size() > 0) {
            int nInputs = iSet->at(0)->decisionVec().size();
            int nOutputs = iSet->at(0)->objectiveVec().size();
            TVector<ElementType> inTypes(nInputs, Tigon::RealType);
            TVector<ElementType> outTypes(nOutputs, Tigon::RealType);

            ProblemSPtr genericProblem = ProblemSPtr(new Problem);
            genericProblem->defineDVecTypes(inTypes);
            genericProblem->defineOVecTypes(outTypes);

            for(int i=0; i<iSet->size(); i++) {
                IMappingSPtr imap = createGenericMapping(oSet, genericProblem);

                for(int j=0; j<nInputs; j++) {
                    imap->defineDecisionVar(j, *(iSet->at(i)->decisionVar(j)));
                }

                for(int j=0; j<nOutputs; j++) {
                    imap->defineObjectiveVar(j, *(iSet->at(i)->objectiveVar(j)));
                }

                imap->defineCost(*(iSet->at(i)->cost()));
            }
            genericProblem.reset();
        }

        // inputs
        switch (m_inputBoundsType) {
        case Tigon::UserDefinedBounds:
            break;

        case Tigon::GlobalBounds:
            defineGlobalInputBounds();
            break;

        case Tigon::CurrentDataBounds:
            defineInputBoundsFromSet();
            break;

        default:
            break;
        }

        if(m_inputBounds->size() > 0) {
            for(int i=0; i<iSet->size(); i++) {
                normaliseToUnitBoxInMemory(oSet->at(i)->decisionVec(), m_inputBounds);
            }
        }

        // outputs
        switch (m_outputBoundsType) {
        case Tigon::UserDefinedBounds:
            break;

        case Tigon::GlobalBounds:
            defineGlobalOutputBounds();
            break;

        case Tigon::CurrentDataBounds:
            defineOutputBoundsFromSet();
            break;

        default:
            break;
        }

        if(m_outputBounds->size() > 0) {
            for(int i=0; i<iSet->size(); i++) {
                normaliseToUnitBoxInMemory(oSet->at(i)->objectiveVec(), m_outputBounds);
            }
        }

        //fitness
        switch (m_fitnessBoundsType) {
        case Tigon::UserDefinedBounds:
            break;

        case Tigon::GlobalBounds:
            break;

        case Tigon::CurrentDataBounds:
            defineFitnessBoundsFromSet();
            break;

        default:
            break;
        }

        if(m_fitnessBounds->size() > 0) {
            for(int i=0; i<iSet->size(); i++) {
                IElementSPtr normCost = oSet->at(i)->cost();
                normaliseToUnitInterval(*normCost,
                                        m_fitnessBounds->lowerBound(0),
                                        m_fitnessBounds->upperBound(0));
            }
        }
    }
}

Tigon::NormalisationBoundsType Normalisation::TP_inputBoundsType() const
{
    return m_inputBoundsType;
}

Tigon::NormalisationBoundsType Normalisation::TP_outputBoundsType() const
{
    return m_outputBoundsType;
}

Tigon::NormalisationBoundsType Normalisation::TP_fitnessBoundsType() const
{
    return m_fitnessBoundsType;
}

void Normalisation::TP_defineInputBoundsType(Tigon::NormalisationBoundsType t)
{
    m_inputBoundsType = t;
}

void Normalisation::TP_defineOutputBoundsType(Tigon::NormalisationBoundsType t)
{
    m_outputBoundsType = t;
}

void Normalisation::TP_defineFitnessBoundsType(Tigon::NormalisationBoundsType t)
{
    m_fitnessBoundsType = t;
}

// General functions
BoxConstraintsDataSPtr Normalisation::inputBounds() const
{
    return m_inputBounds;
}

BoxConstraintsDataSPtr Normalisation::outputBounds() const
{
    return m_outputBounds;
}

BoxConstraintsDataSPtr Normalisation::FitnessBounds() const
{
    return m_fitnessBounds;
}

void Normalisation::defineInputBounds(BoxConstraintsDataSPtr box)
{
    if(box) {
        m_inputBounds = box;
    }
}

void Normalisation::defineOutputBounds(BoxConstraintsDataSPtr box)
{
    if(box) {
        m_outputBounds = box;
    }
}

void Normalisation::defineFitnessBounds(BoxConstraintsDataSPtr box)
{
    if(box) {
        m_fitnessBounds = box;
    }
}

void Normalisation::defineInputBoundsFromSet()
{
    m_inputBounds->clear();

    ISet* iSet = currentInputSet();
    if((iSet == nullptr) || (iSet->size() == 0)) {
        // ERROR
        return;
    }

    // find min and max values
    int n = decisionVecSize();
    TVector<IElement> lBounds = createIElementVector(decisionVecTypes());
    TVector<IElement> uBounds = createIElementVector(decisionVecTypes());
    resetToMax(lBounds);
    resetToMin(uBounds);

    for(int i=0; i<n; i++) {
        for(int j=0; j<iSet->size(); j++) {
            lBounds[i] = tMin(lBounds[i], *(iSet->at(j)->decisionVar(i)));
            uBounds[i] = tMax(uBounds[i], *(iSet->at(j)->decisionVar(i)));
        }
    }

    // update
    m_inputBounds->defineLowerBounds(lBounds);
    m_inputBounds->defineUpperBounds(uBounds);
}

void Normalisation::defineOutputBoundsFromSet()
{
    m_outputBounds->clear();

    ISet* iSet = currentInputSet();
    if((iSet == nullptr) || (iSet->size() == 0)) {
        // ERROR
        return;
    }

    // find min and max values
    int n = objectiveVecSize();
    TVector<IElement> lBounds = createIElementVector(objectiveVecTypes());
    TVector<IElement> uBounds = createIElementVector(objectiveVecTypes());
    resetToMax(lBounds);
    resetToMin(uBounds);

    for(int i=0; i<n; i++) {
        for(int j=0; j<iSet->size(); j++) {
            lBounds[i] = tMin(lBounds[i], *(iSet->at(j)->objectiveVar(i)));
            uBounds[i] = tMax(uBounds[i], *(iSet->at(j)->objectiveVar(i)));
        }
    }

    // update
    m_outputBounds->defineLowerBounds(lBounds);
    m_outputBounds->defineUpperBounds(uBounds);
}

void Normalisation::defineFitnessBoundsFromSet()
{
    m_fitnessBounds->clear();

    ISet* iSet = currentInputSet();
    if((iSet == nullptr) || (iSet->size() == 0)) {
        // ERROR
        return;
    }

    // find min and max values
    IElement lb(RealType, Tigon::Highest);
    IElement ub(RealType, Tigon::Lowest);
    for(int j=0; j<iSet->size(); j++) {
        lb = std::min(lb, *(iSet->at(j)->cost()));
        ub = std::max(ub, *(iSet->at(j)->cost()));
    }

    // update
    m_fitnessBounds->appendBounds(lb, ub);
}

void Normalisation::defineGlobalInputBounds()
{
    m_inputBounds->defineLowerBounds(boxConstraints()->lowerBounds());
    m_inputBounds->defineUpperBounds(boxConstraints()->upperBounds());
}

void Normalisation::defineGlobalOutputBounds()
{
    m_outputBounds->defineLowerBounds(copyIElementVecFromPointers(idealVec()));
    m_outputBounds->defineUpperBounds(copyIElementVecFromPointers(antiIdealVec()));
}

TString Normalisation::name()
{
    return TString("Normalisation");
}

TString Normalisation::description()
{
    return TString("Normalises inputs, outputs and fitness the range [0 1].\n"
                   "The upper and lower bounds can be the lowest and highest "
                   "values in the set, user defined values or global bouandaries.\n"
                   "To disable normalisation of one component (input/output/fitness) "
                   "set it to user defined, and provide empty bounaries (this "
                   "is the default for all components).");
}

} // namespace Operators
} // namespace Tigon
