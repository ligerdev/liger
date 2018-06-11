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
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Properties/ElementProperties.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>

///\todo Improve the performance of the following functions via std methods,
/// e.g. replace all loops with std methods.
using namespace Tigon::Representation;
namespace Tigon {

TVector<double> IElementVecToRealVec(const TVector<IElementSPtr> &ev)
{
    TVector<double> ret;
    std::transform(ev.begin(), ev.end(), std::back_inserter(ret),
                   [](IElementSPtr pe){return pe->value<double>();});
    return ret;
}

TVector<double> IElementVecToRealVec(const TVector<IElement>& ev)
{
    TVector<double> ret;
    std::transform(ev.begin(), ev.end(), std::back_inserter(ret),
                   [](IElement e){return e.value<double>();});
    return ret;
}

TVector<IElement> copyIElementVecFromPointers(const TVector<IElementSPtr>& ev)
{
    TVector<IElement> ret;
    for(int i=0; i<ev.size(); i++) {
        ret.push_back(IElement(*(ev[i])));
    }
    return ret;
}

void resetToMax(TVector<IElementSPtr> vec)
{
    for(int i=0; i< vec.size(); i++) {
        vec[i]->resetToMaxValue();
    }
}

void resetToMin(TVector<IElementSPtr> vec)
{
    for(int i=0; i< vec.size(); i++) {
        vec[i]->resetToMinValue();
    }
}

void resetToMax(TVector<IElement>& vec)
{
    for(int i=0; i< vec.size(); i++) {
        vec[i].resetToMaxValue();
    }
}

void resetToMin(TVector<IElement>& vec)
{
    for(int i=0; i< vec.size(); i++) {
        vec[i].resetToMinValue();
    }
}

TVector<IElementSPtr> cloneVector(TVector<IElementSPtr> vec)
{
    TVector<IElementSPtr> ret;
    for(int i=0; i<vec.size(); i++) {
        ret.push_back(vec[i]->clone());
    }
    return ret;
}

TVector<IElement> copyVector(const TVector<IElement>& vec)
{
    TVector<IElement> ret;
    for(int i=0; i<vec.size(); i++) {
        ret.push_back(IElement(vec[i]));
    }
    return ret;
}

TVector<IElementSPtr> createIElementSPtrVector(TVector<ElementProperties> prpts)
{
    TVector<IElementSPtr> vec;
    int sz = prpts.size();
    for(int i=0; i<sz; i++) {
        vec.push_back(IElementSPtr(new IElement(prpts[i].type())));
    }
    return vec;
}

TVector<IElementSPtr> createIElementSPtrVector(TVector<ElementType> types)
{
    TVector<IElementSPtr> vec;
    for(int i=0; i<types.size(); i++) {
        vec.push_back(IElementSPtr(new IElement(types[i])));
    }

    return vec;
}

TVector<IElementSPtr> createIElementSPtrVector(int size, ElementType type)
{
    TVector<IElementSPtr> vec(size);
    for(int i=0; i<size; i++) {
        vec[i] = IElementSPtr(new IElement(type));
    }

    return vec;
}

TVector<IElement> createIElementVector(const TVector<ElementProperties> &prpts)
{
    TVector<IElement> vec;
    vec.reserve(prpts.size());
    for(const ElementProperties & prpt : prpts) {
        vec.push_back(IElement(prpt.type()));
    }
    return vec;
}

TVector<IElement> createIElementVector(const TVector<ElementType> &types)
{
    TVector<IElement> vec;
    vec.reserve(types.size());
    for(const ElementType &type : types) {
        vec.push_back(IElement(type));
    }

    return vec;
}

TVector<IElement> createIElementVector(int size, ElementType type)
{
    TVector<IElement> vec;
    for(int i=0; i<size; i++) {
        vec.push_back(IElement(type));
    }

    return vec;
}

void clearVector(TVector<IElementSPtr>& vec)
{
    for(int i=0; i<vec.size(); i++) {
        vec[i].reset();
    }
    vec.clear();
}

TVector<ElementProperties> createElemPropVec(const TVector<ElementType> &types,
                                             ProbNamespace::VectorType vecType)
{
    TVector<TString> names;
    TVector<TString> desc;
    TString vecTypeName;
    switch(vecType) {
    case ProbNamespace::DVec:
        vecTypeName = TString("Variable ");
        break;
    case ProbNamespace::OVec:
        vecTypeName = TString("Objective ");
        break;
    case ProbNamespace::PVec:
        vecTypeName = TString("Parameter ");
        break;
    case ProbNamespace::CVec:
        vecTypeName = TString("Constraint ");
        break;
    default:
        vecTypeName = TString("Undefined ");
        break;
    }

    int sz = types.size();
    for(int i=0; i<sz; i++) {
        TString tmpname = vecTypeName + std::to_string(i);
        TString tmpdesc = TString("No description is available for ") + tmpname + TString(".");
        names.push_back(tmpname);
        desc.push_back(tmpdesc);
    }

    ElementPropertiesFactory* fct = ElementPropertiesFactory::instance();

    TVector<ElementProperties> elmPrpts;
    for(int i=0; i<sz; i++) {
        ElementProperties elm = fct->createElementProperties(i, names[i], desc[i], types[i]);
        elmPrpts.push_back(elm);
    }

    return elmPrpts;
}

} // namespace Tigon

