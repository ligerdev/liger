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
#include <tigon/Operators/Filtrations/RandSetReplacement.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Random/RandomGenerator.h>



namespace Tigon {
namespace Operators {

RandSetReplacement::RandSetReplacement()
{
    RandSetReplacement::initialise();
}

RandSetReplacement::RandSetReplacement(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    RandSetReplacement::initialise();
}

RandSetReplacement::~RandSetReplacement()
{

}

void RandSetReplacement::initialise()
{
    m_name = TString("Random Set Replacement");
    m_description = TString("Iterates throw the input set and for a given probability,"
                            " replaces each element from the set with an element"
                            " that is set by a property.");

    addProperty("ReplacementProbability"
                , TString("Specifies the probability associated with"
                          "each replacement operation. Default is 10%")
                , getTType(double));

    addProperty("ReplacementSetTags"
                , TString("Specifies the Sets with a given tag that are to replace some input sets\n"
                          "The default is TMainOptimizationSet.")
                , getTType(TString));

    defineReplacementProbability(Tigon::DefaultReplacementProbability);
    defineReplacementSetTags(TStringList({Tigon::TMainOptimizationSet}));
    defineShuffleInputSet(true);

    addInputTag(Tigon::TForSetReplacement);
    addOutputTag(Tigon::TSetReplaced);
}

void RandSetReplacement::evaluateNode()
{
    clearOutputSets();

    TVector <ISet*> temp;
    temp = setsWithTags(m_replacementSetTags);
    m_replacementSet = temp[0];

    while(hasNextInputSet()) {
        ISet* iSet = nextInputSet();
        if(doSetReplacement()) {
            appendOutputSet(m_replacementSet);
        }
        else {
            appendOutputSet(iSet);
        }
    }
}

void RandSetReplacement::defineReplacementProbability(double u)
{
    if(u >=0) {
        m_replacementProbability = u;
    }
}

TString RandSetReplacement::TP_replacementSetTags() const
{
    return join(m_replacementSetTags, TagDelimiter);
}

void RandSetReplacement::TP_defineReplacementSetTags(const TString &tags)
{
    defineReplacementSetTags(split(tags, TagDelimiter));
}

double RandSetReplacement::TP_replacementProbability() const
{
    return m_replacementProbability;
}

void RandSetReplacement::defineReplacementSetTags(TStringList tags)
{
    m_replacementSetTags = tags;
}

TStringList RandSetReplacement::replacementSetTags() const
{
    return m_replacementSetTags;
}

TString RandSetReplacement::name()
{
    return m_name;
}

TString RandSetReplacement::description()
{
    return m_description;
}

bool RandSetReplacement::doSetReplacement()
{
    if(m_replacementProbability >= 1.0) {
        return true;
    } else if (m_replacementProbability <= 0.0) {
        return false;
    } else {
        return (m_replacementProbability > TRAND.randUni());
    }
}

} // namespace Operators
} // namespace Tigon
