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
#include <tigon/Engine/TigonEngine.h>
#include <tigon/Engine/OptimizationLinearFlow.h>
#include <tigon/Representation/PSets/PSetBase.h>
#include <tigon/Operators/Formulations/IFormulation.h>
#include <tigon/Operators/Formulations/ProblemGenerator/ProblemGenerator.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Algorithms/IAlgorithm.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/TigonUtils.h>

#include <tigon/Engine/TigonOperatorFactory.h>
#include <tigon/Engine/TigonFunctionFactory.h>
#include <tigon/Factories/DistributionFactory.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Core/TObject.h>

#include <tinyxml2/tinyxml2.h>
using namespace tinyxml2;

namespace Tigon {

using namespace Representation;
using namespace Operators;
using namespace Algorithms;

TigonEngine::TigonEngine()
    : m_currentIndex(-1)
    , m_operatorFactory(TigonOperatorFactory::instance())
    , m_functionFactory(TigonFunctionFactory::instance())
    , m_distFactory(DistributionFactory::instance())
{ }

TigonEngine* TigonEngine::instance()
{
    static TigonEngine instance;
    return &instance;
}

TigonEngine::~TigonEngine()
{
    if(m_flows.size() > 0) {
        clearFlow();
        m_flows.clear();
    }
}

bool TigonEngine::loadFlowFromFile(const TString &file)
{
    bool res = false;
    XMLDocument xmlDoc;

    PSetBase* base = new PSetBase();
    OptimizationLinearFlow* workflow = new OptimizationLinearFlow();
    workflow->appendNode(base);

    if(xmlDoc.LoadFile(file.c_str()) == XML_SUCCESS) {
        XMLElement* dataElm = xmlDoc.FirstChildElement("LigerData");
        if(dataElm) {
            XMLElement* endOpElm = dataElm->LastChildElement("ProcessNode");
            XMLElement* opElm = dataElm->FirstChildElement("ProcessNode");
            while(opElm) {
                XMLElement* prptsElm = opElm->FirstChildElement("Properties");

                if(prptsElm) {
                    const char* attr = prptsElm->Attribute("ProcessType");
                    if(attr != 0) {
                        // proceed if ProcessType is defined
                        TString content(attr);

                        /// Master start node
                        if(content == "MASTER_START_NODE") {
                            loadTigonMasterStart(workflow, prptsElm);
                        }

                        /// Problem formulation
                        if(content == "PROBLEM_NODE") {
                            loadTigonProblem(workflow, prptsElm);
                        }
                        /// Algorithm related
                        if(content  == "ALGORITHM_NODE") {
                            loadTigonAlgorithm(workflow, prptsElm);
                        }
                        /// Operator related
                        if(content == "OPERATOR_NODE") {
                            loadTigonOperator(workflow, prptsElm);
                        }
                    }
                }

                if(opElm == endOpElm) {
                    // Break if reach the last operator
                    break;
                } else {
                    // Or find the next operator
                    opElm = opElm->NextSiblingElement("ProcessNode");
                }
            }
            res = true;
        }
    }
    xmlDoc.Clear();
    appendFlow(workflow);

    return res;
}

bool TigonEngine::loadMultipleFlowsFromFiles(const TStringList &files)
{
    for(size_t i = 0; i < files.size(); i++) {
        if(!loadFlowFromFile(files[i])) {
            return false;
        }
    }
    return true;
}

/// \todo
void TigonEngine::saveFlowToFile(const TString &file)
{
    OptimizationLinearFlow* cf = currentFlow();
    if(cf) {
        T_UNUSED(file)
    }

    return;
}

void TigonEngine::saveFlowsToFiles(const TStringList &files)
{
    if(files.size() >= m_flows.size()) {
        for(size_t i = 0; i < files.size(); i++) {
            saveFlowToFile(files[i]);
        }
    }
    return;
}

void TigonEngine::appendFlow(OptimizationLinearFlow *flow)
{
    m_flows.push_back(flow);
    // By default the current flow is the last added flow
    m_currentIndex = m_flows.size() - 1;
}

void TigonEngine::setCurrentIndex(int index)
{
    if(index < m_flows.size()) {
        m_currentIndex = index;
    }
}

TVector<OptimizationLinearFlow*> TigonEngine::workflows() const
{
    return m_flows;
}

OptimizationLinearFlow* TigonEngine::currentFlow() const
{
    if(m_currentIndex < m_flows.size() && m_currentIndex > -1) {
        return m_flows[m_currentIndex];
    } else {
        return 0;
    }
}

void TigonEngine::evaluateFlow(int index)
{
    if(index < m_flows.size()) {
        m_flows[index]->evaluate();
    }
}

void TigonEngine::clearFlow()
{
    clearPointerVector(m_flows);
    m_currentIndex = -1;
}

IPSet* TigonEngine::createOperator(const TString &name, IPSet *parent)
{
    return m_operatorFactory->createOperator(name, parent);
}

IFunction *TigonEngine::createFunction(const TString &name)
{
    return m_functionFactory->createFunction(name);
}

IDistribution *TigonEngine::createParametricDistribution(const DistributionType &type,
                                                         const TVector<double> &parameters)
{
    return m_distFactory->createParametricDist(type, parameters);
}

void TigonEngine::loadTigonOperator(OptimizationLinearFlow *workflow,
                                    XMLElement *xmlElm)
{
    IPSet* op = 0;
    /// Construct the Operator
    XMLElement* prptElm = xmlElm->FirstChildElement("TigonOperator");
    if(prptElm) {
        op = createOperator(prptElm->GetText(), workflow->currentNode());
#ifdef TIGON_DEBUG
        std::cout << "Loading operator " << prptElm->GetText() << std::endl;
#endif
        if(op) {
            workflow->appendNode(op);
        } else {
            throw TException("Tigon::TigonEngine",
                             Tigon::UnrecognisedTigonOperator);
        }
    }

    /// Load all properties
    prptElm = prptElm->NextSiblingElement();
    XMLElement* endPrptElm = xmlElm->LastChildElement();
    while(prptElm) {

        assignPropertyToObject(op, prptElm);

        if(prptElm == endPrptElm) {
            break;
        } else {
            prptElm = prptElm->NextSiblingElement();
        }
    }
}

void TigonEngine::loadTigonAlgorithm(OptimizationLinearFlow *workflow,
                                     XMLElement *xmlElm)
{
    IAlgorithm* alg = 0;

    /// Construct the algorithm
    XMLElement* algElm = xmlElm->FirstChildElement("TigonAlgorithm");
    if(algElm) {
        IPSet* node = createOperator(algElm->GetText(), workflow->currentNode());
        alg = static_cast<IAlgorithm*>(node);
        if(node) {
            workflow->appendNode(node);
        } else {
            throw TException("Tigon::TigonEngine", UnrecognisedTigonOperator);
        }
    } else {
        throw TException("Tigon::TigonEngine", UnrecognisedTigonOperator);
    }

    /// Parse all operator properties
    XMLElement* opElm = xmlElm->FirstChildElement("TigonOperator");
    XMLElement* endOpElm = xmlElm->LastChildElement("TigonOperator");
    int nOps = alg->operators().size();
    while(opElm) {
        XMLElement* prptElm = opElm->FirstChildElement("Name");
        XMLElement* endPrptElm = opElm->LastChildElement();

        if(prptElm && (prptElm != endPrptElm)) {
            TString opName = prptElm->GetText();
            for(int i=0; i<nOps; i++) {
                TObject* op = alg->operators()[i];
                if(op->className() == opName) {
                    /// Load all properties
                    while(prptElm != endPrptElm) {
                        // Increment property conuter
                        prptElm = prptElm->NextSiblingElement();
                        assignPropertyToObject(op, prptElm);
                    }
                    break;
                }
            }
        }

        if(opElm == endOpElm) {
            break;
        }

        opElm = opElm->NextSiblingElement("TigonOperator");
    }
}

void TigonEngine::loadTigonProblem(OptimizationLinearFlow *workflow,
                                   XMLElement *xmlElm)
{
    ProblemGenerator* pg = 0;

    /// Construct Problem Generator
    XMLElement* prptElm = xmlElm->FirstChildElement("TigonOperator");
    if(prptElm) {
        IPSet* op = createOperator(prptElm->GetText(), workflow->currentNode());
        pg = static_cast<ProblemGenerator*>(op);
        if(pg) {
            workflow->appendNode(pg);
        } else {
            throw TException("Tigon::TigonEngine",
                             Tigon::UnrecognisedTigonOperator);
        }
    }

    /// Load all properties
    prptElm = prptElm->NextSiblingElement();
    XMLElement* endPrptElm = xmlElm->LastChildElement();
    while(prptElm) {

        assignPropertyToObject(pg, prptElm);

        if(prptElm == endPrptElm) {
            break;
        } else {
            prptElm = prptElm->NextSiblingElement();
        }
    }

    /// Process problem
    ProblemDefinitionStatus status = pg->processFormulation();
    if(status != FullyDefined) {
        throw TException("Tigon::TigonEngine",
                         Tigon::IncorrectProblemFormatException);
    }
}

void TigonEngine::loadTigonMasterStart(OptimizationLinearFlow *workflow,
                                       XMLElement *xmlElm)
{
    T_UNUSED(workflow)

    XMLElement* randomSeedElem = xmlElm->FirstChildElement("randomSeed");
    if(randomSeedElem) {
        int seed = 0;
        if(randomSeedElem->QueryIntText(&seed) == XML_SUCCESS) {
            TRAND.defineSeed(seed);
#ifdef TIGON_DEBUG
            std::cout << "TigonEngine: Random seed: " << TRAND.seed() << std::endl;
#endif
        }
    }
}

void TigonEngine::assignPropertyToObject(TObject *object, XMLElement *xmlElm)
{
    if(xmlElm) {
        const char *name = xmlElm->Name();
        const char *text = xmlElm->GetText();
        if(name && text) {
#ifdef TIGON_DEBUG
        std::cout << " * setProperty " << name << ": " << text << std::endl;
#endif
            object->setProperty(TString(name), TString(text));
        }
    }
}

void TigonEngine::propertiesToXml(TObject *object, XMLElement *xmlElm)
{
    ///\todo Enable tigon to write a workflow to the disk;
    T_UNUSED(object)
    T_UNUSED(xmlElm)
}

} // namespace Tigon
