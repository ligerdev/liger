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
#pragma once

/// Preprocessors
#include <boost/config.hpp>
#if defined(LIGER_TIGON_LIB)
#  define LIGER_TIGON_EXPORT BOOST_SYMBOL_EXPORT
#elif  defined(LIGER_TIGON_STATIC_LIB)
#  define LIGER_TIGON_EXPORT
#else
#  define LIGER_TIGON_EXPORT BOOST_SYMBOL_IMPORT
#endif

#if !defined(TIGON_FUNCTION_EXPORT)
#  define TIGON_FUNCTION_EXPORT extern "C" BOOST_SYMBOL_EXPORT
#endif

/// Macro used to to disable the warnings from external library.
#if defined _MSC_VER
#define DISABLE_WARNINGS __pragma(warning(push)) \
    __pragma(warning(disable: 4503)) \
    __pragma(warning(disable: 4702)) \
    __pragma(warning(disable: 4996)) \
    __pragma(warning(disable: 4100)) \
    __pragma(warning(disable: 4127)) \
    __pragma(warning(disable: 4244)) \
    __pragma(warning(disable: 4267)) \
    __pragma(warning(disable: 4512)) \
    __pragma(warning(disable: 4510)) \
    __pragma(warning(disable: 4610))
#define ENABLE_WARNINGS __pragma(warning(pop))
#elif defined __clang__
#define DISABLE_WARNINGS                          \
    _Pragma("clang diagnostic push")              \
    _Pragma("clang diagnostic ignored \"-Wall\"") \
    _Pragma("clang diagnostic ignored \"-Wextra\"")
#define ENABLE_WARNINGS _Pragma("clang diagnostic pop")
#elif defined __GNUC__
#define DISABLE_WARNINGS                                                       \
    _Pragma("GCC diagnostic push")                                             \
    _Pragma("GCC diagnostic ignored \"-Wall\"")                                \
    _Pragma("GCC diagnostic ignored \"-Wextra\"")                              \
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")                   \
    _Pragma("GCC diagnostic ignored \"-Wignored-qualifiers\"")                 \
    _Pragma("GCC diagnostic ignored \"-Wunused-local-typedefs\"")              \
    _Pragma("GCC diagnostic ignored \"-Wignored-attributes\"")                 \
    _Pragma("GCC diagnostic ignored \"-Wint-in-bool-context\"")                \
    _Pragma("GCC diagnostic ignored \"-Wattributes\"")                         \
    _Pragma("GCC diagnostic ignored \"-Wmisleading-indentation\"")
#define ENABLE_WARNINGS _Pragma("GCC diagnostic pop")
#else
#define DISABLE_WARNINGS
#define ENABLE_WARNINGS
#endif

/// Type Defs
#include <complex>
DISABLE_WARNINGS
#include <eigen/Eigen>
ENABLE_WARNINGS
#include <tigon/Core/TVector.h>
#include <tigon/Core/TString.h>
#include <tigon/Core/TMap.h>

typedef Eigen::MatrixXd TMatrixReal;
typedef Eigen::VectorXd TCVectorReal;
typedef Eigen::RowVectorXd TRVectorReal;
typedef std::complex<double> TComplex;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned int uint;

#include <boost/logic/tribool.hpp>
typedef boost::logic::tribool tribool;
BOOST_TRIBOOL_THIRD_STATE(incomparable)

#define T_UNUSED(x) (void)x;

#include <tigon/Core/TSharedPointer.h>
/// Elegant solution but only supported by MS VS 2013 and onwards.
/// TODO: When the time comes and the 'using' synatx of c++11 is standard
/// in all compilers, use the following.
//template<typename T> using SPtr = TSharedPointer<T>;
namespace Tigon {
class IKriging;
class PolynomialRegression;
class MultiPolynomialRegression;

typedef TSharedPointer<IKriging>  KrigingSPtr;
typedef TSharedPointer<PolynomialRegression> PolyRegressionSPtr;
typedef TSharedPointer<MultiPolynomialRegression> MPolyRegressionSPtr;

namespace Log {
class LogManager;
typedef TSharedPointer<LogManager> LogManagerSPtr;
}

namespace Representation {
class IElement;
class IMapping;
class ElementPropertiesData;
class ProblemPropertiesData;
class FunctionPropertiesData;
class Problem;
class IFunction;
class BoxConstraintsData;
class TimeSeriesContainer;
class SampleVectors;

typedef TSharedPointer<IElement>                             IElementSPtr;
typedef TSharedPointer<IMapping>                             IMappingSPtr;
typedef TSharedPointer<ElementPropertiesData>       ElementPropertiesDataSPtr;
typedef TSharedPointer<ProblemPropertiesData>       ProblemPropertiesDataSPtr;
typedef TSharedPointer<FunctionPropertiesData>      FunctionPropertiesDataSPtr;
typedef TSharedPointer<Problem>                               ProblemSPtr;
typedef TSharedPointer<IFunction>                           IFunctionSPtr;
typedef TSharedPointer<BoxConstraintsData>         BoxConstraintsDataSPtr;
typedef TSharedPointer<TimeSeriesContainer>        TimeSeriesContainerSPtr;
typedef TSharedPointer<SampleVectors>              SampleVectorsSPtr;
}
}
