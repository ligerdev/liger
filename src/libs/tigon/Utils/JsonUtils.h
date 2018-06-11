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
#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

#include <tigon/Representation/Properties/ElementProperties.h>
#include <json/json.h>

using namespace Json;

namespace Tigon {
namespace Representation {
class ISet;
class IDistribution;
class UncertaintyMapping;
}
}
using namespace Tigon::Representation;

namespace Tigon {

/// IElement
/*!
 * \brief Convert a vector of IElement shared pointer to a json array which
 * contains real numbers only.
 */
LIGER_TIGON_EXPORT JsonArray toJsonArrayDoubleValue(const TVector<IElementSPtr>& vec);
LIGER_TIGON_EXPORT JsonArray toJsonArrayDoubleValue(const TVector<IElement>& vec);
LIGER_TIGON_EXPORT void fromJsonArrayDoubleValue(JsonArray array,
                                            TVector<IElementSPtr>& elems);
/*!
 * \brief Conver a vector of IElement Shared Pointer to a json array with full
 * information, includng types, distributions, etc;
 */
LIGER_TIGON_EXPORT JsonArray toJsonArrayFullInfo(const TVector<IElementSPtr>& vec);
LIGER_TIGON_EXPORT JsonArray toJsonArrayFullInfo(const TVector<IElement>& vec);
LIGER_TIGON_EXPORT TVector<ElementProperties> fromJsonArray(JsonArray array);
LIGER_TIGON_EXPORT void fromJsonArrayFullInfo(JsonArray array,
                                      TVector<IElementSPtr>& elems);

/// TVector
LIGER_TIGON_EXPORT JsonArray toJsonArray(const TVector<double>& vec);
LIGER_TIGON_EXPORT JsonArray toJsonArray(const TVector<bool>& vec);
LIGER_TIGON_EXPORT JsonArray toJsonArray(const TVector<TVector<int> > &vec);
LIGER_TIGON_EXPORT
void fromJsonArray(const JsonArray& jarray, TVector<double> &vec);
LIGER_TIGON_EXPORT
void fromJsonArray(const JsonArray& jarray, TVector<bool> &vec);
LIGER_TIGON_EXPORT
void fromJsonArray(const JsonArray& jarray, TVector<TVector<int> >& vec);

/// ISet
LIGER_TIGON_EXPORT JsonArray toJsonArray(ISet* solSet);

/// IFunction
LIGER_TIGON_EXPORT JsonArray functionsToJsonArray(const TVector<IFunctionSPtr> &funcs);
LIGER_TIGON_EXPORT void fromJsonArray(const JsonArray& jarray, TStringList& slist);
void assignFunctionProperties(IFunctionSPtr func, const JsonObject& prptsObj);
//LIGER_TIGON_EXPORT void fromJsonArray(const JsonArray& jarray,
//                                      TVector<QVariantMap>& map);

/// ElementProperties
LIGER_TIGON_EXPORT
JsonArray toIDJsonArray(const TVector<ElementProperties>& prpts);
LIGER_TIGON_EXPORT
JsonArray  toJsonArray(const TVector<ElementProperties> &prts);
LIGER_TIGON_EXPORT
void fromJsonArray(const JsonArray &jarray, TVector<TVector<ElementProperties> >& prts);

/// Distribution
LIGER_TIGON_EXPORT JsonObject distToJsonObject(const IDistribution& dist);
LIGER_TIGON_EXPORT
IDistribution* JsonObjectToDistribution(const JsonObject& json);
LIGER_TIGON_EXPORT TString distributionTypeToString(DistributionType type);
LIGER_TIGON_EXPORT
DistributionType stringToDistributionType(const TString& typeString);

LIGER_TIGON_EXPORT JsonArray toJsonArray(const TVector<UncertaintyMapping*>& mapping);
LIGER_TIGON_EXPORT
void fromJsonArray(const JsonArray& jarray, TVector<UncertaintyMapping*>& vec);

LIGER_TIGON_EXPORT
JsonArray toJsonArray(const TVector<TVector<UncertaintyMapping*> >& mapping);
LIGER_TIGON_EXPORT
void fromJsonArray(const JsonArray& jarray,
                   TVector<TVector<UncertaintyMapping*> >& vec);

/// Problem
/*!
 * \brief This function unrolls all information in a tigon problem.
 * \param Tigon Problem Shared Pointer
 * \return A Json object
 */
LIGER_TIGON_EXPORT JsonObject problemToJsonObject(const ProblemSPtr prob);
//LIGER_TIGON_EXPORT ProblemDefinitionStatus fromJsonObject(const JsonObject& jobj,
//                                                          ProblemSPtr prob);

LIGER_TIGON_EXPORT
JsonObject toJsonObjWithNames(const TVector<ElementProperties>& prpts,
                               const TVector<IElementSPtr>& vec);

LIGER_TIGON_EXPORT JsonObject toJsonObj(IMappingSPtr imap);
LIGER_TIGON_EXPORT bool fromJSonObj(IMappingSPtr imap, const JsonObject &jObj);

LIGER_TIGON_EXPORT TString elemTypeToString(const ElementType& type);
LIGER_TIGON_EXPORT ElementType stringToElemType(const TString &typeString);

/// Misc
LIGER_TIGON_EXPORT TStringList toStringList(const JsonArray& array);

} // namespace Tigon
#endif // JSONUTILS_H
