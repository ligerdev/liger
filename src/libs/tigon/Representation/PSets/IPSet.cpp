/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/PSets/IPSet.h>
#include <tigon/Representation/PSets/IPSetPrivate.h>
#include <tigon/Representation/PSets/IPSetDecorator.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Mappings/IMappingOperations.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Properties/ElementProperties.h>
#include <tigon/Representation/Properties/ProblemProperties.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <tigon/Representation/Properties/ProblemPropertiesFactory.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Log/LogManager.h>

namespace Tigon {
namespace Representation {

IPSet::IPSet()
    : d(TSharedPointer<IPSetPrivate>(new IPSetPrivate()))
{
}

IPSet::IPSet(const IPSet& set)
{
    this->setInternal(set.getInternal());
}

IPSet::~IPSet()
{

}

IPSet* IPSet::clone()
{
    IPSet* ret = new IPSet();
    TSharedPointer<IPSetPrivate> nd = d->clone();
    ret->setInternal(nd);
    return ret;
}

void IPSet::defineLogManager(Log::LogManagerSPtr log)
{
    d->m_log.reset();
    d->m_log = log;
}

Log::LogManagerSPtr IPSet::log() const
{
    return d->m_log;
}

void IPSet::clearLog()
{
    d->m_log->clearLog();
}

/// Sets tags

TStringList IPSet::allTags() const
{
    TStringList lst = getMapKeys(d->m_tags);
    return lst;
}

bool IPSet::tagExists(const TString &tag) const
{
    return mapHasKey(d->m_tags, tag);
}

TVector<ISet*> IPSet::setsWithTags(const TStringList &tags) const
{
    TVector<ISet*> matchingSets;
    if(!tags.empty()) {
        matchingSets = setsWithTag(tags[0]);
        if(matchingSets.empty()) {
            return matchingSets;
        }

        for(size_t t=1; t<tags.size(); t++) {
            TVector<ISet*> newSets = setsWithTag(tags[t]);
            // Remove the set from the mathcingSet if it is not in the newSets
            matchingSets.erase(std::remove_if(matchingSets.begin(), matchingSets.end(),
                                              [newSets](ISet* set){return !vectorContains(newSets, set);}), matchingSets.end());
        }
    }
    return matchingSets;
}

TVector<ISet*> IPSet::setsWithTag(const TString &tag) const
{
    return d->m_tags[tag];
}

ISet* IPSet::setWithTags(const TStringList &tags, int idx) const
{
    TVector<ISet*> sets = setsWithTags(tags);
    if(sets.size() > idx) {
        return sets[idx];
    } else {
        return nullptr;
    }
}

ISet* IPSet::setWithTag(const TString &tag, int idx) const
{
    TVector<ISet*> sets = d->m_tags[tag];
    if(sets.size() > idx) {
        return sets[idx];
    } else {
        return nullptr;
    }
}

void IPSet::tagSet(ISet* sSet, const TString &tag)
{
    sSet->addTag(tag);

    if(indexOfSet(sSet) < 0) {
        appendSet(sSet);
    } else {
        // if the tag didn't exist, it will be created by operator[]
        if(!vectorContains(d->m_tags[tag], sSet)) {
            d->m_tags[tag].push_back(sSet);
        }
    }
}

bool IPSet::removeTagFromSet(ISet* sSet, const TString &tag)
{
    if(tagExists(tag)) {
        int nRemoved = removeAll(d->m_tags[tag], sSet);
        if(nRemoved > 0) {
            sSet->removeTag(tag);
            if(d->m_tags[tag].empty()) {
                d->m_tags.erase(tag);
            }
            return true;
        }
    }
    return false;
}

/// Sets

TVector<ISet*> IPSet::allSets() const
{
    return d->m_sets;
}

int IPSet::numberOfSets() const
{
    return d->m_sets.size();
}

void IPSet::clearSets()
{

    std::for_each(d->m_sets.begin(), d->m_sets.end(), [](ISet* elem) {
        delete elem;
    });
    d->m_sets.clear();
    d->m_tags.clear();
}

ISet* IPSet::set(int idx) const
{
    if(idx<0 || idx>d->m_sets.size()) {
        return nullptr;
    } else {
        return d->m_sets.at(idx);
    }
}

int IPSet::indexOfSet(ISet* sSet) const
{
    return vectorIndexOf(d->m_sets, sSet);
}

bool IPSet::appendSet(ISet* sSet)
{
    // only operates if the set doesn't already exist
    if(indexOfSet(sSet) < 0 ) {
        TStringList tags = sSet->tags();
        for(int i=0; i<tags.size(); i++) {
            d->m_tags[tags[i]].push_back(sSet);
        }
        d->m_sets.push_back(sSet);
        return true;
    } else {
        return false;
    }
}

void IPSet::appendSet(ISet* sSet, const TString &tag)
{
    TStringList tags = {tag};
    sSet->defineTags(tags);
    appendSet(sSet);
}

void IPSet::appendSet(ISet* sSet, const TStringList &tags)
{
    sSet->defineTags(tags);
    appendSet(sSet);
}

ISet* IPSet::appendSet(const TString &tag)
{
    ISet* newSet = new ISet(tag);
    appendSet(newSet);
    return newSet;
}

ISet *IPSet::appendSet(const TStringList &tags)
{
    ISet* newSet = new ISet(tags);
    appendSet(newSet);
    return newSet;
}

bool IPSet::removeSet(int idx)
{
    if( (idx < 0) || (idx > d->m_sets.size()) ) {
        return false;
    } else {
        // first remove the tags
        ISet* sSet = d->m_sets.at(idx);
        TStringList tags = sSet->tags();
        for(int i=0; i<tags.size(); i++) {
            removeTagFromSet(sSet, tags[i]);
        }
        // then remove the set
        delete sSet;
        d->m_sets.erase(d->m_sets.begin()+idx);
        return true;
    }
}

bool IPSet::removeSet(ISet* sSet)
{
    int idx = indexOfSet(sSet);
    return removeSet(idx);
}

bool IPSet::appendToSet(ISet* targetSet, IMappingSPtr imap)
{
    targetSet->append(imap);
    int idx = indexOfSet(targetSet);
    if(idx < 0) {
        appendSet(targetSet);
        return false;
    } else {
        return true;
    }
}

bool IPSet::isKeepArchive() const
{
    return d->m_keepArchive;
}

void IPSet::defineKeepArchive(bool b)
{
    d->m_keepArchive = b;
}

Tigon::ArchiveUpdateStatus IPSet::updateNonDominatedArchive(IMappingSPtr imap, bool weakOrEpsilon)
{
    Tigon::ArchiveUpdateStatus ret = Tigon::AddedToArchive;
    ISet* archive = setWithTag(Tigon::TNonDominatedArchive);
    if(archive == nullptr) {
        archive = appendSet(Tigon::TNonDominatedArchive);
        ret = Tigon::ReplacedInArchive;
    } else {
        // iterate backwards to allow removal of archived IMappings
        for(int i=archive->size()-1; i>=0; i--) {
            // the new IMapping weakly dominates the i^th archived IMapping
            tribool dom;
            if(weakOrEpsilon) {
                dom = *imap <= *(archive->at(i));
            }
            else {
                TVector <double> qimap = imap->doubleObjectiveVec();
                TVector <double> qarchive = archive->at(i)->doubleObjectiveVec();
                dom = epsilonDominance(qimap,qarchive,-1e-2);
            }
            if(dom.value == tribool::false_value) {
                return Tigon::UnchangedArchive;
            } else if(dom.value == tribool::true_value) {
                archive->remove(i);
                ret = Tigon::ReplacedInArchive;
            } else {
                tribool eq = (*imap == *(archive->at(i)));
                if(eq.value == tribool::true_value) {
                    return Tigon::UnchangedArchive;
                }
            }
        }
    }
    // if hasn't returned earlier, it is non-dominated
    archive->append(imap);
    return ret;
}

/// Problem
TString IPSet::problemID() const
{
    return d->m_problem->problemProperties().ID();
}

TString IPSet::problemName() const
{
    return d->m_problem->problemProperties().name();
}

TString IPSet::problemDescription() const
{
    return d->m_problem->problemProperties().description();
}

ProblemType IPSet::problemType() const
{
    return d->m_problem->problemProperties().type();
}

ProblemProperties IPSet::problemProperties() const
{
    return d->m_problem->problemProperties();
}

/// Evaluation Functions and Links
TVector<IFunctionSPtr> IPSet::functionVec() const
{
    return d->m_problem->functionVec();
}

TVector<TVector<int> > IPSet::function2VariablesMap() const
{
    return d->m_problem->f2dMap();
}

TVector<TVector<int> > IPSet::function2ObjectivesMap() const
{
    return d->m_problem->f2oMap();
}

TVector<TVector<int> > IPSet::function2ParametersMap() const
{
    return d->m_problem->f2pMap();
}

TVector<TVector<int> > IPSet::function2ConstraintsMap() const
{
    return d->m_problem->f2cMap();
}

TVector<TVector<int> > IPSet::function2UnusedMap() const
{
    return d->m_problem->f2uMap();
}

/// Decision Variable Vector(s)
int IPSet::decisionVecSize() const
{
    return d->m_problem->dPrpts().size();
}

TStringList IPSet::decisionVecIDs() const
{
    TStringList lst;
    TVector<ElementProperties> prpts = d->m_problem->dPrpts();
    std::transform(prpts.cbegin(), prpts.cend(), std::back_inserter(lst),
                   [](const ElementProperties& p){return p.ID();});

    return lst;
}

TStringList IPSet::decisionVecNames() const
{
    TStringList lst;
    TVector<ElementProperties> prpts = d->m_problem->dPrpts();
    std::transform(prpts.cbegin(), prpts.cend(), std::back_inserter(lst),
                   [](const ElementProperties& p){return p.name();});

    return lst;
}

TStringList IPSet::decisionVecDescriptions() const
{
    TStringList lst;
    TVector<ElementProperties> prpts = d->m_problem->dPrpts();
    std::transform(prpts.cbegin(), prpts.cend(), std::back_inserter(lst),
                   [](const ElementProperties& p){return p.description();});

    return lst;
}

TStringList IPSet::decisionVecUnits() const
{
    TStringList lst;
    TVector<ElementProperties> prpts = d->m_problem->dPrpts();
    std::transform(prpts.cbegin(), prpts.cend(), std::back_inserter(lst),
                   [](const ElementProperties& p){return p.unit();});

    return lst;
}

TVector<Tigon::ElementType> IPSet::decisionVecTypes() const
{
    TVector<Tigon::ElementType> types;
    TVector<ElementProperties> prpts = d->m_problem->dPrpts();
    types.reserve(prpts.size());
    std::transform(prpts.begin(), prpts.end(), std::back_inserter(types),
                   [](const ElementProperties& p) { return p.type();});
    return types;
}

TVector<ElementProperties> IPSet::decisionVecProperties() const
{
    return d->m_problem->dPrpts();
}

TVector<UncertaintyMapping*> IPSet::dVecUncertainties() const
{
    return d->m_problem->dVecUncertainties();
}

/// Parameter Vector
TVector<IElementSPtr> IPSet::parameterVector() const
{
    return d->m_problem->parameterVector();
}

int IPSet::parameterVecSize() const
{
    return d->m_problem->pPrpts().size();
}

TStringList IPSet::parameterVecIDs() const
{
    TStringList lst;
    TVector<ElementProperties> prpts = d->m_problem->pPrpts();
    lst.reserve(prpts.size());
    std::transform(prpts.begin(), prpts.end(), std::back_inserter(lst),
                   [](const ElementProperties & prpt){ return prpt.ID(); });

    return lst;
}

TStringList IPSet::parameterVecNames() const
{
    TStringList tmplist;
    TVector<ElementProperties> prpts = d->m_problem->pPrpts();
    tmplist.reserve(prpts.size());
    std::transform(prpts.begin(), prpts.end(), std::back_inserter(tmplist),
                   [](const ElementProperties& p) { return p.name();});
    return tmplist;
}

TStringList IPSet::parameterVecDescriptions() const
{
    TStringList tmplist;
    TVector<ElementProperties> prpts = d->m_problem->pPrpts();
    tmplist.reserve(prpts.size());
    std::transform(prpts.begin(), prpts.end(), std::back_inserter(tmplist),
                   [](const ElementProperties& p) { return p.description();});

    return tmplist;
}

TStringList IPSet::parameterVecUnits() const
{
    TStringList tmplist;
    TVector<ElementProperties> prpts = d->m_problem->pPrpts();
    tmplist.reserve(prpts.size());
    std::transform(prpts.begin(), prpts.end(), std::back_inserter(tmplist),
                   [](const ElementProperties& p) { return p.unit();});

    return tmplist;
}

TVector<Tigon::ElementType> IPSet::parameterVecTypes() const
{
    TVector<Tigon::ElementType> types;
    TVector<ElementProperties> prpts = d->m_problem->pPrpts();
    types.reserve(prpts.size());
    std::transform(prpts.begin(), prpts.end(), std::back_inserter(types),
                   [](const ElementProperties& p) { return p.type();});
    return types;
}

TVector<ElementProperties> IPSet::parameterVecProperties() const
{
    return d->m_problem->pPrpts();
}

/// Objective Vector(s)
int IPSet::objectiveVecSize() const
{
    return d->m_problem->oPrpts().size();
}

TStringList IPSet::objectiveVecIDs() const
{
    TStringList lst;
    int sz = d->m_problem->oPrpts().size();
    for(int i=0; i<sz; i++) {
        lst.push_back(d->m_problem->oPrpts()[i].ID());
    }

    return lst;
}

TStringList IPSet::objectiveVecNames() const
{
    TStringList tmplist;
    for(int i=0; i<d->m_problem->oPrpts().size(); i++) {
        tmplist.push_back(d->m_problem->oPrpts()[i].name());
    }

    return tmplist;
}

TStringList IPSet::objectiveVecDescriptions() const
{
    TStringList tmplist;
    for(int i=0; i<d->m_problem->oPrpts().size(); i++) {
        tmplist.push_back(d->m_problem->oPrpts()[i].description());
    }

    return tmplist;
}

TStringList IPSet::objectiveVecUnits() const
{
    TStringList lst;
    int sz = d->m_problem->oPrpts().size();
    for(int i=0; i<sz; i++) {
        lst.push_back(d->m_problem->oPrpts()[i].unit());
    }

    return lst;
}

TVector<Tigon::ElementType> IPSet::objectiveVecTypes() const
{
    TVector<Tigon::ElementType> types;
    for(int i=0; i<d->m_problem->oPrpts().size(); i++) {
        types.push_back(d->m_problem->oPrpts()[i].type());
    }
    return types;
}

TVector<OptimizationType> IPSet::objectiveVecOptimizationTypes() const
{
    TVector<OptimizationType> lst;
    int sz = d->m_problem->oPrpts().size();
    for(int i=0; i<sz; i++) {
        lst.push_back(d->m_problem->oPrpts()[i].optimizationType());
    }

    return lst;
}

TVector<ElementProperties> IPSet::objectiveVecProperties() const
{
    return d->m_problem->oPrpts();
}

TVector<TVector<UncertaintyMapping *> > IPSet::funcOutUncertainties() const
{
    return d->m_problem->funcOutUncertainties();
}

/// Constraints Vector
int IPSet::constraintVecSize() const
{
    return d->m_problem->cPrpts().size();
}

TStringList IPSet::constraintVecIDs() const
{
    TStringList lst;
    int sz = d->m_problem->cPrpts().size();
    for(int i=0; i<sz; i++) {
        lst.push_back(d->m_problem->cPrpts()[i].ID());
    }

    return lst;
}

TStringList IPSet::constraintVecNames() const
{
    TStringList tmplist;
    for(int i=0; i<d->m_problem->cPrpts().size(); i++) {
        tmplist.push_back(d->m_problem->cPrpts()[i].name());
    }

    return tmplist;
}

TStringList IPSet::constraintVecDescriptions() const
{
    TStringList tmplist;
    for(int i=0; i<d->m_problem->cPrpts().size(); i++) {
        tmplist.push_back(d->m_problem->cPrpts()[i].description());
    }

    return tmplist;
}

TStringList IPSet::constraintVecUnits() const
{
    TStringList lst;
    int sz = d->m_problem->cPrpts().size();
    for(int i=0; i<sz; i++) {
        lst.push_back(d->m_problem->cPrpts()[i].unit());
    }

    return lst;
}

TVector<Tigon::ElementType> IPSet::constraintVecTypes() const
{
    TVector<Tigon::ElementType> types;
    for(int i=0; i<d->m_problem->cPrpts().size(); i++) {
        types.push_back(d->m_problem->cPrpts()[i].type());
    }
    return types;
}

TVector<OptimizationType> IPSet::constraintVecOptimizationTypes() const
{
    TVector<OptimizationType> lst;
    int sz = d->m_problem->cPrpts().size();
    for(int i=0; i<sz; i++) {
        lst.push_back(d->m_problem->cPrpts()[i].optimizationType());
    }

    return lst;
}

TVector<ElementProperties> IPSet::constraintVecProperties() const
{
    return d->m_problem->cPrpts();
}

/// Unused Vector
int IPSet::unusedVecSize() const
{
    return d->m_problem->uPrpts().size();
}

TStringList IPSet::unusedVecIDs() const
{
    TStringList lst;
    int sz = d->m_problem->uPrpts().size();
    for(int i=0; i<sz; i++) {
        lst.push_back(d->m_problem->uPrpts()[i].ID());
    }

    return lst;
}

TStringList IPSet::unusedVecNames() const
{
    TStringList tmplist;
    for(int i=0; i<d->m_problem->uPrpts().size(); i++) {
        tmplist.push_back(d->m_problem->uPrpts()[i].name());
    }

    return tmplist;
}

TStringList IPSet::unusedVecDescriptions() const
{
    TStringList tmplist;
    for(int i=0; i<d->m_problem->uPrpts().size(); i++) {
        tmplist.push_back(d->m_problem->uPrpts()[i].description());
    }

    return tmplist;
}

TStringList IPSet::unusedVecUnits() const
{
    TStringList lst;
    int sz = d->m_problem->uPrpts().size();
    for(int i=0; i<sz; i++) {
        lst.push_back(d->m_problem->uPrpts()[i].unit());
    }

    return lst;
}

TVector<ElementType> IPSet::unusedVecTypes() const
{
    TVector<Tigon::ElementType> types;
    for(int i=0; i<d->m_problem->uPrpts().size(); i++) {
        types.push_back(d->m_problem->uPrpts()[i].type());
    }
    return types;
}

TVector<OptimizationType> IPSet::unusedVecOptimizationTypes() const
{
    TVector<OptimizationType> lst;
    int sz = d->m_problem->uPrpts().size();
    for(int i=0; i<sz; i++) {
        lst.push_back(d->m_problem->uPrpts()[i].optimizationType());
    }

    return lst;
}

TVector<ElementProperties> IPSet::unusedVecProperties() const
{
    return d->m_problem->uPrpts();
}

/// Box constraints
BoxConstraintsDataSPtr IPSet::boxConstraints() const
{
    return d->m_problem->boxConstraints();
}

/// Special Vectors in Objective (and constraint) Space

TVector<IElementSPtr> IPSet::idealVec() const
{
    return d->m_problem->idealVector();
}

TVector<IElementSPtr> IPSet::nadirVec() const
{
    return d->m_problem->nadirVector();
}

TVector<IElementSPtr> IPSet::antiIdealVec() const
{
    return d->m_problem->antiIdealVector();
}

TVector<IElementSPtr> IPSet::goalVec() const
{
    return d->m_problem->goalVector();
}

TVector<IElementSPtr> IPSet::thresholdVec() const
{
    return d->m_problem->thresholdVector();
}

TVector<double> IPSet::dirVec() const
{
    return d->m_direction;
}

TVector<bool> IPSet::isExternalParameters() const
{
    return d->m_problem->isExternalParameters();
}

int IPSet::numberExternalParameters() const
{
    return d->m_problem->numberExternalParameters();
}

void IPSet::defineIdealVec(const TVector<IElementSPtr>&ideal)
{
    d->m_problem->defineIdealVector(ideal);
}

void IPSet::defineNadirVec(const TVector<IElementSPtr>&nadir)
{
    d->m_problem->defineNadirVector(nadir);
}

void IPSet::defineAntiIdealVec(const TVector<IElementSPtr>&antiIdeal)
{
    d->m_problem->defineAntiIdealVector(antiIdeal);
}

bool IPSet::updateIdealNadirVec(IMappingSPtr newIMap)
{
    bool updated = updateIdealVec(newIMap);
    updated = (updateAntiIdealVec(newIMap) || updated);
    updated = (updateNadirVec(newIMap) || updated);
    return updated;
}

bool IPSet::updateIdealVec(IMappingSPtr newIMap)
{
    if(!newIMap->isOptimizationMapping()) {
        return false;
    }

    TVector<IElementSPtr> newOVec(newIMap->objectiveVec());

    return d->m_problem->updateIdeal(newOVec);
}

bool IPSet::updateAntiIdealVec(IMappingSPtr newIMap)
{
    if(!newIMap->isOptimizationMapping()) {
        return false;
    }

    TVector<IElementSPtr> newOVec(newIMap->objectiveVec());

    return d->m_problem->updateAntiIdeal(newOVec);
}

bool IPSet::updateNadirVec(IMappingSPtr newIMap)
{
    if(!newIMap->isOptimizationMapping()) {
        return false;
    }

    if(!(d->m_keepArchive)) {
        return false;
    }

    Tigon::ArchiveUpdateStatus archiveUpdated = updateNonDominatedArchive(newIMap, true);
    if(archiveUpdated == Tigon::UnchangedArchive) {
        return false;
    }
    bool updated = false;

    if(archiveUpdated == Tigon::AddedToArchive) {
        TVector<IElementSPtr> newOVec = newIMap->objectiveVec();
        updated = d->m_problem->updateNadir(newOVec);
    } else if(archiveUpdated == Tigon::ReplacedInArchive) {
        ISet* archive = setWithTag(Tigon::TNonDominatedArchive);
        TVector<IElementSPtr> oldNadir = d->m_problem->nadirVector();
        TVector<IElementSPtr> newNadir = cloneVector(oldNadir);
        resetToMin(newNadir);
        for(int j=0; j<archive->size(); j++) {
            TVector<IElementSPtr> archivedVec = archive->at(j)->objectiveVec();
            for(int i=0; i<objectiveVecSize(); i++) {
                if(*(archivedVec[i]) > *(newNadir[i])) {
                    newNadir[i]->defineValue(archivedVec[i]->value());
                }
            }
        }
        for(int i=0; i<objectiveVecSize(); i++) {
            if(*(newNadir[i]) != *(oldNadir[i])) {
                d->m_problem->defineNadirVector(newNadir);
                updated = true;
                break;
            }
        }
    }

    if(updated) {
        d->m_problem->processProblemDefinition();
    }
    return updated;
}

void IPSet::defineGoalVec(const TVector<IElementSPtr>&goals)
{
    d->m_problem->defineGoalVector(goals);
}

void IPSet::defineGoal(int idx, IElementSPtr goal)
{
    if(isInRange(idx, objectiveVecSize())) {
        TVector<IElementSPtr> goals = d->m_problem->goalVector();
        goals[idx]->defineValue(goal->value());
        d->m_problem->defineGoalVector(goals);
    }
}

void IPSet::defineThresholdVec(const TVector<IElementSPtr>&thresholds)
{
    d->m_problem->defineThresholdVector(thresholds);
}

void IPSet::defineThreshold(int idx, IElementSPtr threshold)
{
    if(isInRange(idx, constraintVecSize())) {
        TVector<IElementSPtr> thresholds = d->m_problem->thresholdVector();
        thresholds[idx]->defineValue(threshold->value());
    } else {
        //Error
    }
}

void IPSet::defineDirVec(const TVector<double> &dir)
{
    if(dir.size() != objectiveVecSize()) {
        // ERROR
        return;
    }
    for(int i=0; i<dir.size(); i++) {
        if(dir[i] < 0.0) {
            // ERROR
            return;
        }
    }

    d->m_direction = dir;
    toUnitVec(d->m_direction, 1.0);
}

/// IMapping creation and removal
IMappingSPtr IPSet::createOptimizationMapping(ISet* targetSet)
{
    IMappingSPtr imap = IMappingSPtr(new IMapping(d->m_problem, d->m_log));
    imap->defineOptimizationMapping(true);
    appendToSet(targetSet, imap);
    return imap;
}

IMappingSPtr IPSet::createGenericMapping(ISet* targetSet, ProblemSPtr prob)
{
    IMappingSPtr imap = IMappingSPtr(new IMapping(prob, d->m_log));
    imap->defineOptimizationMapping(false);
    appendToSet(targetSet, imap);
    return imap;
}

IMappingSPtr IPSet::cloneMapping(ISet* targetSet, IMappingSPtr imap)
{
    IMappingSPtr rmap;
    if(imap) {
        rmap = imap->clone();
    }

    appendToSet(targetSet, rmap);

    return rmap;
}

/// Iteration and evaluation budget
bool IPSet::isTerminate() const
{
    if(     ((d->m_budget <= 0) && (d->m_maxIter <= 0))                ||
            ((d->m_budget  > 0) && (d->m_usedBudget  >= d->m_budget))  ||
            ((d->m_maxIter > 0) && (d->m_currentIter >= d->m_maxIter))  ) {
        return true;
    } else {
        return false;
    }
}

int IPSet::currentIteration() const
{
    return d->m_currentIter;
}

void IPSet::incrementIteration()
{
    d->m_currentIter++;
}

void IPSet::setCurrentIteration(int citer)
{
    d->m_currentIter = citer;
}

int IPSet::remainingIteratoins() const
{
    return (d->m_maxIter) - (d->m_currentIter);
}

int IPSet::maxIteration() const
{
    return d->m_maxIter;
}

void IPSet::defineMaxIteration(int nIter)
{
    d->m_maxIter = nIter;
}

int IPSet::remainingBudget() const
{
    return d->m_budget - d->m_usedBudget;
}

int IPSet::budget() const
{
    return d->m_budget;
}

void IPSet::defineBudget(int budget)
{
    d->m_budget = budget;
}

void IPSet::decrementBudget(int cost)
{
    d->m_usedBudget += cost;
}

int IPSet::usedBudget() const
{
    return d->m_usedBudget;
}

void IPSet::resetBudgetCount(int c)
{
    d->m_usedBudget = c;
}

void IPSet::defineErrorState(TErrorState state)
{
    if(state == NoError) {
        // lower error flag from all IMappings
        for(int i = 0; i < d->m_sets.size(); i++) {
            ISet* s = d->m_sets[i];
            for(int j = 0; j < s->size(); j++) {
                s->at(j)->defineErrorFlag(false);
            }
        }
    }

    d->m_errorState = state;
}

TErrorState IPSet::errorState() const
{
    return d->m_errorState;
}

void IPSet::ignoreUnsuccessfulEvals()
{
    for(int i = 0; i < d->m_sets.size(); i++) {
        ISet* s = d->m_sets[i];
        for(int j = 0; j < s->size(); j++) {
            IMappingSPtr imap = s->at(j);
            if(!(imap->isSuccessfulEval())) {
                imap->defineEvaluated(false);
                imap->defineErrorFlag(false);
            }
        }
    }
}

void IPSet::removeInvalidIMappings()
{
    for(int i = 0; i < d->m_sets.size(); i++) {
        ISet* s = d->m_sets[i];
        for(int j = s->size()-1; j >=0 ; j--) {
            IMappingSPtr imap = s->at(j);
            if(!(imap->isSuccessfulEval())) {
                s->remove(j);
            }
        }
    }
}

void IPSet::deteriorateInvalidIMappings()
{
    for (ISet* s : d->m_sets) {
        for (IMappingSPtr imap : s->all()) {
            if(!(imap->isSuccessfulEval())) {
                imap->defineObjectiveVec(antiIdealVec());
            }
        }
    }
}

/// MultiPack - Data structure for arbitrary information passing
void IPSet::setMultiPack(const TMap<TString, TVector<double>> &mname)
{
    d->m_multiPack = mname;
}

void IPSet::insertMultiPack(const TString &name, const TVector<double> &data)
{
    d->m_multiPack[name] = data;
}

bool IPSet::appendMultiPack(const TString &name, const TVector<double> &data)
{
    bool status = false;
    bool keyExists = mapHasKey(d->m_multiPack, name);
    if(keyExists) {
        return status;
    } else {
        status = true;
        d->m_multiPack[name] = data;
    }
    return status;
}

bool IPSet::multiPackExists(const TString &name) const
{
    return mapHasKey(d->m_multiPack, name);
}

TVector<double> IPSet::multiPack(const TString &name) const
{
    bool status = multiPackExists(name);
    if(status) {
        return d->m_multiPack[name];
    }

    // keep the compiler from complaining
    return TVector<double>();
}

TMap<TString, TVector<double> > IPSet::multiPack() const
{
    return d->m_multiPack;
}

void IPSet::clearMutliPack()
{
    d->m_multiPack.clear();
}

/// IPSet chaining
void IPSet::setFromIPSet(const IPSet* ipset)
{
    setInternal(ipset->getInternal());
}

/// Private data access
TSharedPointer<IPSetPrivate> IPSet::getInternal() const
{
    return d;
}

void IPSet::setInternal(TSharedPointer<IPSetPrivate> nd)
{
    d = nd;
}

/// Initialization status
ProblemDefinitionStatus IPSet::isProblemDefined() const
{
    return d->m_problem->isProblemDefined();
}

/// Liger Engine Compatibility Operators
void IPSet::clearData()
{
    this->setInternal(TSharedPointer<IPSetPrivate>(new IPSetPrivate()));
}

IPSet& IPSet::operator=(const IPSet& data)
{
    this->setInternal(data.getInternal());
    return *this;
}

const ProblemSPtr IPSet::problem() const
{
    return d->m_problem;
}

void IPSet::defineProblem(ProblemSPtr prob)
{
    d->m_problem = prob->clone();
    updateProblem();
}

void IPSet::appendFunction(IFunctionSPtr func)
{
    d->m_problem->appendFunction(func);
    updateProblem();
}

void IPSet::updateProblem()
{
    if(d->m_problem->isProblemDefined() != FullyDefined) {
        Tigon::ProblemDefinitionStatus status = d->m_problem->processProblemDefinition();
        if(status != FullyDefined) {
            // ERROR: Something went wrong
        }
        updatePSetMappings();
    }

    if(d->m_direction.size() != objectiveVecSize()) {
        TVector<double> dirvec(objectiveVecSize(), 1.0);
        defineDirVec(dirvec);
    }
}

/// Utility functions
void IPSet::updatePSetMappings()
{
    // TODO - This is a placeholder. The function is called whenever the problem
    // is changed. At the moment, all solutions are deleted. It needs to be
    // changed once we start solving dynamic problems
    // (i.e., changing problem formulation).
    clearSets();
}

} // namespace Representation
} // namespace Tigon
