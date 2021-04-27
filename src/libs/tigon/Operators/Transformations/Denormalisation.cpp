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
#include <tigon/Operators/Transformations/Denormalisation.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Problems/Problem.h>


namespace Tigon {
namespace Operators {

Denormalisation::Denormalisation()
{
    initialise();
}

Denormalisation::Denormalisation(Tigon::Representation::IPSet *ipset)
    : ITransformation(ipset)
{
    initialise();
}

Denormalisation::~Denormalisation()
{

}

void Denormalisation::initialise()
{
    //m_propertyNames.push_back("Input bouds type");
    //m_propertyDescriptions.push_back("The method to define the input bounds:/n"
    //                              "from global box constraints or user defined.");
    //m_propertyNames.push_back("Output bouds type");
    //m_propertyDescriptions.push_back("The method to define the output bounds:/n"
    //                              "from global ideal and anti ideal vectors "
    //                              "or user defined.");

    addOutputTag(Tigon::TDenormalised);

    m_inputBounds   = BoxConstraintsDataSPtr(new BoxConstraintsData);
    m_outputBounds  = BoxConstraintsDataSPtr(new BoxConstraintsData);
    m_fitnessBounds = BoxConstraintsDataSPtr(new BoxConstraintsData);

    TP_defineInputBoundsType(Tigon::UserDefinedBounds);
    TP_defineOutputBoundsType(Tigon::UserDefinedBounds);
}

void Denormalisation::evaluateNode()
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
            break;

        default:
            break;
        }

        if(m_inputBounds->size() > 0) {
            for(int i=0; i<iSet->size(); i++) {
                scaleBackFromUnitBox(oSet->at(i)->decisionVec(), m_inputBounds);
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
            break;

        default:
            break;
        }

        if(m_outputBounds->size() > 0) {
            for(int i=0; i<iSet->size(); i++) {
                scaleBackFromUnitBox(oSet->at(i)->objectiveVec(), m_outputBounds);
            }
        }

        // fitness
        if(m_fitnessBounds->size() > 0) {
            for(int i=0; i<iSet->size(); i++) {
                IElementSPtr normCost = oSet->at(i)->cost();
                scaleBackFromUnitInterval(*normCost,
                                          m_fitnessBounds->lowerBound(0),
                                          m_fitnessBounds->upperBound(0));
            }
        }
    }
}

Tigon::NormalisationBoundsType Denormalisation::TP_inputBoundsType() const
{
    return m_inputBoundsType;
}

Tigon::NormalisationBoundsType Denormalisation::TP_outputBoundsType() const
{
    return m_outputBoundsType;
}

void Denormalisation::TP_defineInputBoundsType(Tigon::NormalisationBoundsType t)
{
    m_inputBoundsType = t;
}

void Denormalisation::TP_defineOutputBoundsType(Tigon::NormalisationBoundsType t)
{
    m_outputBoundsType = t;
}

// General functions
BoxConstraintsDataSPtr Denormalisation::inputBounds() const
{
    return m_inputBounds;
}

BoxConstraintsDataSPtr Denormalisation::outputBounds() const
{
    return m_outputBounds;
}

BoxConstraintsDataSPtr Denormalisation::FitnessBounds() const
{
    return m_fitnessBounds;
}

void Denormalisation::defineInputBounds(BoxConstraintsDataSPtr box)
{
    if(box) {
        m_inputBounds.reset();
        m_inputBounds = box;
    }
}

void Denormalisation::defineOutputBounds(BoxConstraintsDataSPtr box)
{
    if(box) {
        m_outputBounds.reset();
        m_outputBounds = box;
    }
}

void Denormalisation::defineFitnessBounds(BoxConstraintsDataSPtr box)
{
    if(box) {
        m_fitnessBounds.reset();
        m_fitnessBounds = box;
    };
}

void Denormalisation::defineGlobalInputBounds()
{
    m_inputBounds->defineLowerBounds(boxConstraints()->lowerBounds());
    m_inputBounds->defineUpperBounds(boxConstraints()->upperBounds());
}

void Denormalisation::defineGlobalOutputBounds()
{
    m_outputBounds->defineLowerBounds(copyIElementVecFromPointers(idealVec()));
    m_outputBounds->defineUpperBounds(copyIElementVecFromPointers(antiIdealVec()));
}

TString Denormalisation::name()
{
    return TString("de-normalisation");
}

TString Denormalisation::description()
{
    return TString("de-normalises inputs, outputs and fitness from the range "
                   "[0 1] to the original boundaries.\n"
                   "The upper and lower bounds can be user defined values or "
                   "global bouandaries.\nTo disable denormalisation "
                   "of one component (input/output/fitness) set it to user "
                   "defined, and provide empty bounaries (this is the default "
                   "for all components).");
}

} // namespace Operators
} // namespace Tigon
