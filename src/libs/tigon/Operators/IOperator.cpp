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
#include <tigon/Operators/IOperator.h>
#include <tigon/Representation/PSets/IPSet.h>
#include <tigon/Representation/PSets/IPSetDecorator.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Operators {

IOperator::IOperator()
{
    IOperator::initialise();
}

IOperator::IOperator(Tigon::Representation::IPSet* ipset)
    : IPSetDecorator(ipset)
{
    IOperator::initialise();
}

IOperator::~IOperator()
{
    if(m_setsCreator) {
        updateSets();
        clearOutputSets();
    }
}

void IOperator::initialise()
{
    addProperty("OutputTags",
                TString("Information about the sets modified/created by the operator\n"
                        " (e.g., the operatoration done on the sets)."),
                typeid(TString).hash_code());

    addProperty("AdditionalOutputTags",
                TString("Additional information about the sets modified by the operator\n"
                        " that was not available to the operator created the sets."),
                typeid(TString).hash_code());

    addProperty("InputTags",
                TString("Information about the sets need to be processed by the operator."),
                typeid(TString).hash_code());

    m_elementTypeMap[Tigon::ETReal]    = Tigon::RealType;
    m_elementTypeMap[Tigon::ETInteger] = Tigon::IntegerType;
    m_elementTypeMap[Tigon::ETNominal] = Tigon::NominalType;
    m_elementTypeMap[Tigon::ETOrdinal] = Tigon::OrdinalType;

    resetInputSetIterator();
    resetOutputSetIterator();

    m_setsCreator = false;

    m_shuffleInputSet = false;
    m_shuffleOutputSet = false;
}


/// Properties
TStringList IOperator::outputTags() const
{
    return m_outputTags;
}

void IOperator::defineOutputTags(const TStringList &tags)
{
    m_outputTags = tags;
}

TString IOperator::TP_outputTags() const
{
    return join(m_outputTags, TagDelimiter);
}

void IOperator::TP_defineOutputTags(const TString &tags)
{
    defineOutputTags(split(tags, TagDelimiter));
}

TStringList IOperator::additionalOutputTags() const
{
    return m_addOutputTags;
}

void IOperator::defineAdditionalOutputTags(const TStringList &tags)
{
    m_addOutputTags = tags;
}

TString IOperator::TP_additionalOutputTags() const
{
    return join(m_addOutputTags, TagDelimiter);
}

void IOperator::TP_defineAdditionalOutputTags(const TString &tags)
{
    defineAdditionalOutputTags(split(tags, TagDelimiter));
}

TStringList IOperator::inputTags() const
{
    return m_inputTags;
}

void IOperator::defineInputTags(const TStringList &tags)
{
    m_inputTags = tags;
}

TString IOperator::TP_inputTags() const
{
    return join(m_inputTags,TagDelimiter);
}

void IOperator::TP_defineInputTags(const TString &tags)
{
    defineInputTags(split(tags,TagDelimiter));
}

TStringList IOperator::supportedElementTypes() const
{
    return m_supportedElementTypes;
}

void IOperator::defineSupportedElementTypes(const TStringList &types)
{
    m_supportedElementTypes = types;
}

bool IOperator::shufleInputSet() const
{
    return m_shuffleInputSet;
}

void IOperator::defineShuffleInputSet(bool p)
{
    m_shuffleInputSet = p;
}

bool IOperator::shufleOutputSet() const
{
    return m_shuffleOutputSet;
}

void IOperator::defineShuffleOutputSet(bool p)
{
    m_shuffleOutputSet = p;
}

/// Input and Output Sets Tags

void IOperator::addOutputTag(const TString &tag)
{
    if(!contains(m_outputTags, tag)) {
        m_outputTags.push_back(tag);
    }
}

void IOperator::removeOutputTag(const TString &tag)
{
    removeAll(m_outputTags, tag);
}

void IOperator::clearOutputTags()
{
    m_outputTags.clear();
}

void IOperator::addAdditionalOutputTag(const TString &tag)
{
    if(!contains(m_addOutputTags, tag)) {
        m_addOutputTags.push_back(tag);
    }
}

void IOperator::removeAdditionalOutputTag(const TString &tag)
{
    removeAll(m_addOutputTags, tag);
}

void IOperator::clearAdditionalOutputTags()
{
    m_addOutputTags.clear();
}

void IOperator::addInputTag(const TString &tag)
{
    if(!contains(m_inputTags, tag)) {
        m_inputTags.push_back(tag);
    }
}

void IOperator::removeInputTag(const TString &tag)
{
    removeAll(m_inputTags, tag);
}

void IOperator::clearInputTags()
{
    m_inputTags.clear();
}

/// Output Sets
ISet* IOperator::currentOutputSet() const
{
    return m_currentOutputSet;
}

ISet* IOperator::outputSet(int idx)
{
    resetOutputSetIterator();
    if(idx >= 0 && idx < m_outputSets.size()) {
        m_outputSetIterator = idx;
        m_currentOutputSet = m_outputSets.at(idx);
    }
    return m_currentOutputSet;
}

TVector<ISet*> IOperator::outputSets() const
{
    return m_outputSets;
}

ISet* IOperator::appendOutputSet() {
    m_setsCreator = true;
    m_outputSetIterator = m_outputSets.size();
    m_currentOutputSet = appendSet(m_outputTags);
    m_outputSets.push_back(m_currentOutputSet);
    return m_currentOutputSet;
}

ISet* IOperator::appendOutputSet(ISet* existingSet)
{
    m_setsCreator = true;
    m_outputSetIterator = m_outputSets.size();
    m_currentOutputSet = existingSet->clone();
    appendSet(m_currentOutputSet, m_outputTags);
    m_outputSets.push_back(m_currentOutputSet);
    return m_currentOutputSet;
}

bool IOperator::removeOutputSet(int idx)
{
    if(!isInTRange(idx, m_outputSets.size())) {
        return false;
    }
    ISet* sSet = outputSet(idx);
    bool removed = removeSet(sSet);
    if(removed) {
        m_outputSets.erase(m_outputSets.begin()+idx);
    }
    return removed;
}

void IOperator::clearOutputSets()
{
    while(!m_outputSets.empty()) {
        removeSet(m_outputSets.front());
        m_outputSets.erase(m_outputSets.begin());
    }
}

ISet* IOperator::nextOutputSet()
{
    if(hasNextOutputSet()) {
        m_currentOutputSet = m_outputSets.at(++m_outputSetIterator);
    }
    return m_currentOutputSet;
}

bool IOperator::hasNextOutputSet() const
{
    return m_outputSetIterator < (static_cast<int>(m_outputSets.size()) - 1);
}

void IOperator::resetOutputSetIterator()
{
    m_currentOutputSet  = nullptr;
    m_outputSetIterator = -1;
}

ISet* IOperator::peepNextOutputSet() const
{
    if(hasNextOutputSet()) {
        return m_outputSets.at(m_outputSetIterator+1);
    } else {
        return nullptr;
    }
}

/// Input Sets
ISet* IOperator::currentInputSet() const
{
    return m_currentInputSet;
}

ISet* IOperator::inputSet(int idx)
{
    resetInputSetIterator();
    if(isInTRange(idx,inputSets().size())) {
        m_inputSetIterator = idx;
        m_currentInputSet = m_inputSets.at(indexCycle(idx,inputSets().size()));
    }
    return m_currentInputSet;
}

TVector<ISet*> IOperator::inputSets() const
{
    return m_inputSets;
}

ISet* IOperator::nextInputSet()
{
    if(hasNextInputSet()) {
        m_currentInputSet = m_inputSets.at(++m_inputSetIterator);
    }
    return m_currentInputSet;
}

/// \todo this is not tested.
bool IOperator::hasNextInputSet() const
{
    return m_inputSetIterator < (static_cast<int>(m_inputSets.size()) - 1);
}

void IOperator::resetInputSetIterator()
{
    m_currentInputSet  = nullptr;
    m_inputSetIterator = -1;
}

ISet* IOperator::peepNextInputSet() const
{
    if(hasNextInputSet()) {
        return m_inputSets.at(m_inputSetIterator+1);
    } else {
        return nullptr;
    }
}

/// General functions
bool IOperator::isSupportedElementType(Tigon::ElementType type) const
{
    for(size_t idx = 0; idx < m_supportedElementTypes.size(); idx++) {
        if(m_elementTypeMap.at(m_supportedElementTypes[idx]) == type) {
            return true;
        }
    }
    return false;
}

bool IOperator::isSupportedElementType(IElementSPtr e) const
{
    return isSupportedElementType(e->type());
}

/// Protected methods
IMappingSPtr IOperator::createMapping()
{
    return createOptimizationMapping(m_currentOutputSet);
}

IMappingSPtr IOperator::cloneMapping(const IMappingSPtr other)
{
    return IPSet::cloneMapping(m_currentOutputSet, other);
}

void IOperator::updateSets()
{
    if(!m_outputTags.empty()) {
        m_outputSets = setsWithTags(m_outputTags);
        for(const TString & tag : m_addOutputTags) {
            for(ISet* set : m_outputSets) {
                tagSet(set, tag);
            }
        }

        if(m_shuffleOutputSet) {
            m_outputSets = Tigon::shuffle(m_outputSets);
        }
        resetOutputSetIterator();
    }

    if(!m_inputTags.empty()) {
        m_inputSets = setsWithTags(m_inputTags);
        if(m_shuffleInputSet) {
            m_inputSets = Tigon::shuffle(m_inputSets);
        }
        resetInputSetIterator();
    }
}

} // namespace Operators
} // namespace Tigon
