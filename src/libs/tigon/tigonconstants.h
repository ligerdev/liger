/****************************************************************************
**
** Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk)
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

#include <tigon/tigon_global.h>
#include <tigon/Core/TVector.h>
#include <tigon/Core/TString.h>

// boost includes
#include <boost/numeric/conversion/bounds.hpp>

namespace Tigon {

const bool  PROTOTYPE_BOOL_ERROR_CODE  = false;
const int   PROTOTYPE_INT_ERROR_CODE  = -1;
const double PROTOTYPE_REAL_ERROR_CODE = -1.0;

enum ElementType {
    RealType,
    IntegerType,
    OrdinalType,
    NominalType
};

const TString ConfidenceType("Confidence in satisfying a threshold");
const TString ThresholdType("Threshold for a given confidence");

const TString ETReal("Real Type");
const TString ETInteger("Integer Type");
const TString ETOrdinal("Ordinal Type");
const TString ETNominal("Nominal Type");

enum DistributionType {
    GenericDistType,
    NonParametricDistType,
    MergedDistType,
    SampledDistType,
    UniformDistType,
    NormalDistType,
    LinearDistType,
    PeakDistType,
    BetaDistType,
    GammaDistType,
    BinomialDistType,
    ChiSquaredDistType,
    ExponentialDistType,
    PoissonDistType,
    StudentTDistType,
    CauchyDistType,
    BernoulliDistType,
    DiscreteUniformDistType
};

const char DTGenericDistType[]         = "Generic Distribution Type";
const char DTNonParametricDistType[]   = "NonParametric Distribution Type";
const char DTMergedDistType[]          = "Merged Distribution Type";
const char DTSampledDistType[]         = "Sampled Distribution Type";
const char DTUniformDistType[]         = "Uniform Distribution Type";
const char DTNormalDistType[]          = "Normal Distribution Type";
const char DTLinearDistType[]          = "Linear Distribution Type";
const char DTPeakDistType[]            = "Peak Distribution Type";
const char DTBetaDistType[]            = "Beta Distribution Type";
const char DTGammaDistType[]           = "Gamma Distribution Type";
const char DTBinomialDistType[]        = "Binomial Distribution Type";
const char DTChiSquaredDistType[]      = "ChiSquared Distribution Type";
const char DTExponentialDistType[]     = "Exponential Distribution Type";
const char DTPoissonDistType[]         = "Poisson Distribution Type";
const char DTStudentTDistType[]        = "StudentT Distribution Type";
const char DTCauchyDistType[]          = "Cauchy Distribution Type";
const char DTBernoulliDistType[]       = "Bernoulli Distribution Type";
const char DTDiscreteUniformDistType[] = "DiscreteUniform Distribution Type";

enum OptimizationType {
    Minimization,
    Maximization,
    NonOptimization,
};

enum ProblemType {
    Convex,
    Linear,
    Quadratic,
    NonConvex
};

enum FunctionType {
    Internal,
    Matlab,
    SharedLib,
    Python,
    Other
};

const TString FuncTypeStrInternal("Internal");
const TString FuncTypeStrMatlab("Matlab");
const TString FuncTypeStrSharedLib("ShareLib");
const TString FuncTypeStrPython("Python");
const TString FuncTypeStrOther("Other");

enum ConstraintType {
    BoxType,
    LinearEqType,
    LinearIneqType,
    NonLinearEqType,
    NonLinearIneqType
};

enum ConstraintEvaluationType {
    PreEvaluationType,
    PostEvaluationType
};

enum OperatorParameterType {
    TGN_OP_INT,
    TGN_OP_REAL,
    TGN_OP_STRING,
    TGN_OP_INTVEC,
    TGN_OP_REALVEC,
    TGN_OP_STRINGVEC
};

enum ScalarisationType {
    WeightedSum,
    WeightedChebyshev,
    WeightedChebyshevAugmented,
    WeightedLp
};

enum RegularisationApproach {
    InitialOrder,
    CentroidBasedOrder
};

enum NormalisationBoundsType {
    GlobalBounds,
    UserDefinedBounds,
    CurrentDataBounds
};


enum NeighbourhoodType {
    NeighbourhoodRadius,
    NeighbourhoodSize
};

enum WeightScope {
    LocalWeight,
    GlobalWeight
};

enum ErrorHandlingMethod {
    ErrDensityBased,
    ErrConfidenceIntervalBased
};

enum ConstraintHandlingMethod {
    NoConstraintHandling,
    PenaltyBased,
    SurrogateBased
};

enum ArchiveUpdateStatus {
    UnchangedArchive,
    AddedToArchive,
    ReplacedInArchive
};

enum ProblemDefinitionStatus {
    FullyDefined,
    Undefined,
    UnprocessedChanges,
    IllDefinedDVecProperties,
    IllDefinedOProperties,
    IllDefinedFunctionVec,
    IllDefinedDVecMaps,
    IllDefinedOVecMaps,
    IllDefinedPVecMaps,
    IllDefinedCVecMaps,
    IllDefinedUVecMaps,
    IllDefinedBoxConstraints,
    IllDefinedIdealVec,
    IllDefinedAntiIdealVec,
    IllDefinedNadirVec,
    IllDefinedSetGoalVec,
    IllDefinedGoalVec,
    IllDefinedPriorityVec,
    IllDefinedThresholdVec,
    IllDefinedUncertaintyVec,
    IllDefinedFuncOutUncertaintyVec,
    IllDefinedParameterVec,
    IllDefinedParameterGroups,
};

enum TErrorState {
    NoError,
    UndefinedError
};

enum ValidationStatus {
    NotValidated,
    ParametricValidation,
    SampledValidation
};

namespace ProbNamespace {
enum VectorType {
    AllVecs,
    DVec,
    OVec,
    PVec,
    CVec,
    UVec
};
} // namespace ProbNamespace


enum DistanceMeasure {
    EuclideanDistance,
    ManhattanDistance,
    AngleDistance
};

enum SelectionType {
    RandomSelection,
    ShuffledSelection,
    OrderedSelection
};

enum DominanceRelationType {
    NormalDominanceRelation,
    ConstraintHandlingDominanceRelation,
    PreferabilityRelation,
    PreferabilityConstraintHandlingRelation
};

/// IMapping constants
const TVector<Tigon::ElementType> EmptyType;
const TVector<TVector<bool> >     EmptyIOMap;

/// Tags
const TString TMainOptimizationSet("MAIN_OPTIMIZATION_SET");
const TString TNonDominatedArchive("NON_DOMINATED_ARCHIVE");
const TString TMatingPool("MATING_POOL");
const TString TForEvaluation("FOR_EVALUATION");
const TString TForFitness("FOR_FITNESS");
const TString TForSelection("FOR_SELECTION");
const TString TForModification("FOR_MODIFICATION");
const TString TForNextIteration("FOR_NEXT_ITERATION");
const TString TForDirection("FOR_DIRECTION");
const TString TForCrowding("FOR_CROWDING");
const TString TFitness("FITNESS");
const TString TDirection("DIRECTION");
const TString TForPerturbation("FOR_PERTURBATION");
const TString TIPMQP("FOR_IPM_QP");
const TString TForNeighbourhoods("FOR_NEIGHBOURHOODS");
const TString TForFiltration("FOR_FILTRATION");
const TString TFiltration("FILTRATION");
const TString TNeighbourhoods("NEIGHBOURHOODS");
const TString TRandomNumbers("RANDOM_NUMBERS");
const TString TNormalised("NORMALISED");
const TString TDenormalised("DENORMALISED");
const TString TValidation("VALIDATION");
const TString TForConvergence("FOR_CONVERGENCE");
const TString TForClustering("FOR_CLUSTERING");
// Use temporary  tags within composite operators. A temporary tag can exist
// only within an evaluateNode() block. It must be removed from the operator, or
// the temporary operator with this tag must be deleted before the end of
// evaluateNode().
const TString TTempForClustering("TEMP_FOR_CLUSTERING");
const TString TTempNewSolutions("TEMP_NEW_SOLUTIONS");
const TString TForSetReplacement("FOR_SET_REPLACEMENT");
const TString TSetReplaced("SET_REPLACED");
const TString TTempForMutation("TEMP_FOR_MUTATION");
const TString TForResize("FOR_RESIZE");
const TString TForMOEADUpdate("FOR_MOEAD_UPDATE");
const TString TReducePopulation("FOR_REDUCE_POPULATION");

/// Distribution constants
const int   DistMinNSamples(3);
const int   DistNSamples(500);
const int   DistConvNSamples(250);
const int   DistMultNSamples(100);
const int   DistAvgSamplesInBin(10);
const double DistMinInterval(0.001);
const double DistPeakMinN(1.0);
const double DistPeakMaxN(50.0);
const double DistPeakMinNBasisFunc(30.0);
const double DistPeakMaxNBasisFunc(150.0);

/// Precision and numeric limits
const double  Epsilon(1e-6);
const double  Highest(boost::numeric::bounds<double>::highest()/2.0);
const double  Lowest(boost::numeric::bounds<double>::lowest()/2.0);
const long long HighestInteger(boost::numeric::bounds<long long>::highest()/2);
const long long LowestInteger(boost::numeric::bounds<long long>::lowest()/2);
const double  AugmentedConstant(0.05);

/// Operators default values
const double DefaultCrossoverSolutionProbability(0.9);
const double DefaultCrossoverVariableProbability(0.5);
const double DefaultCrossoverVariableSwapProbability(0.5);
const double DefaultMutationSolutionProbability(1.0);
const double DefaultMutationVariableProbability(0.1);
const Tigon::ScalarisationType DefaultScalarisationFunction(Tigon::WeightedSum);
const TString TagDelimiter(";");

/// Algorithm specific
// ACROMUSE
const TString AcSpd("SPD");
const TString AcHpd("HPD");
const TString AcMaxF("Fmax");
const TString AcMinF("Fmin");
const double   AcMaxSpd(0.4);
const double   AcMaxHpd(0.4);
const double   AcK(0.5);
const double   AcK1(0.4);
const double   AcK2(0.8);

// ACROMUSE parameters for surrogate based optimization
// Parameters for high computationally intensive optimization search
// Search the surrogate
const int      DefaultBudgetPerVariableSS_H(100);
const int      DefaultInitialPopsizePerVariableSS_H(20);
const int      DefaultStallIterationsSS_H(20);
// Train the surrogate
const int      DefaultBudgetPerVariableTS_H(100);
const int      DefaultInitialPopsizePerVariableTS_H(20);
const int      DefaultStallIterationsTS_H(20);

// Parameters for low computationally intensive optimization search
// Search the surrogate
const int      DefaultBudgetPerVariableSS_L(40);
const int      DefaultInitialPopsizePerVariableSS_L(5);
const int      DefaultStallIterationsSS_L(3);
// Train the surrogate
const int      DefaultBudgetPerVariableTS_L(40);
const int      DefaultInitialPopsizePerVariableTS_L(5);
const int      DefaultStallIterationsTS_L(3);


// Normal Mutation
const double DefaultMutationSigma(-1.0);
const double DefaultWidthPercent(0.1);
const double DefaultMutationStepSize(-1.0);

// NSGA-II
const double DefaultCrossoverDistributionIndex(15.0);
const double DefaultMutationDistributionIndex(20.0);

// NSGA-II Parametric
const int DefaultNumberBins(20);

// MOEA/D
const int   DefaultNumberPointsPerDimension(-1); // finds the closest H for the given population size
const int   DefaultNeighbourhoodSize(5);
const int   DefaultMaxNumberReplacements(2);
const double DefaultReplacementProbability(0.1);

// ParEGO
const int DefaultMaxSolutionsSurrogateModelling(100);

// sParEGO
const double   UseDefaultNeighbourhoodRadius(-1.0);
const double   KDEBandwidthRatio(0.1);
const double   PerturbationRadiusByNeighbourhoodRadius(0.5);
const double   DefaultNeighbourhoodRadius(0.1);
const double   DefaultsParEGOInitPerturbationRatio(0.5);
const TString  SampleSizes("Sample sizes");  //multipack key

// MOGA
const double   DefaultErrorSigmaEstimation(1e-6);
const double   DefaultSharingFunctionAlpha(1.0);

// InterwovenSystemHandler
const int DefaultNumberPointsForKDE(100);

enum ConfidenceInSampleType {
    UnchangedDistribution,
    ConfidenceIncrease,
    ConfidenceDecrease,
};

/// Network Constants
enum NetworkProtocolType {
    Syncthing,
    LibTorrent,
    Unknown
};

/// Robustness Indicators constants
const double DefaultConfidence(0.95);
const double DefaultThreshold(1.0);
const int   DefaultMonteCarloSample(100);
const double GenericRobustnessParameter(0.0);

/// Exception Constants
const TString DivisionByZeroException("Division by zero.");
const TString IncorrectNumberArguments("Incorrect number of arguments.");
const TString IncorrectProblemFormatException("Incorrect problem format.");
const TString RangeException("Range errors in internal computation.");
const TString DomainException("Parameter outside the valid range.");
const TString NullPointerException("Invalid access to null pointer.");
const TString InvalidTypeException("Invalid type.");
const TString UnrecognisedTypeException("Unrecognised type.");
const TString InvalidRealOpException("Invalid operation for real type");
const TString InvalidIntegerOpException("Invalid operation for integer type.");
const TString InvalidOrdinalOpException("Invalid operation for ordinal type.");
const TString InvalidNominalOpException("Invalid operation for nominal type.");
const TString TagsNotFoundException("Operator cannot find sets with ucurrent tags configuraion.");
const TString CannotOpenFileException("The file cannot be opened.");
const TString IncorrectMATLABFormulation("Incorrect Matlab problem formulation");
const TString IncorrectPythonFormulation("Incorrect Python problem formulation");
const TString InputOutputSizeMissMatch("Input/output size miss match");
const TString EvaluationFailed("MATLAB Engine not responding. Please try executing the workglow again.");
const TString FunctionNotAvailable("Function not available for this operation");
const TString UnDefinedFunction("Function not defined for this operation");
const TString UnrecognisedScalarisationFunction("Unrecognised Scalarisation Function");
const TString UnrecognisedRegularisationMethod("Unrecognised Regularisation Method");
const TString UnrecognisedTigonOperator("Unrecognised Tigon Operator");
const TString UnrecognisedTigonProperty("Unrecognised Tigon Property");
const TString UnrecognisedDistanceMeasure("Unrecognised Distance Measure");
const TString UnrecognisedOption("Unrecognised Option");

/// Log Constants
namespace Log {
const TString LEvals("Evaluations");
const TString LPops("Populations");
const TString LTime("Time");
const TString LDVec("Decision Vec");
const TString LPVec("Parameter Vec");
const TString LOVec("Objective Vec");
const TString LWVec("Weight Vec");
const TString LCVec("Constraint Vec");
const TString LUVec("Unused Vec");
const TString LIsEval("Solution Evaluated");
const TString LObjEval("Objectives Evaluated");
const TString LConstEval("Constraints Evaluated");
const TString LUnusedEval("Unused Evaluated");
const TString LScalarised("Scalarised");
const TString LCost("Cost");
const TString LFunc("Function");
const TString LInputs("Inputs");
const TString LOutputs("Outputs");
const TString LName("Name");
const TString LTags("Tags");
const TString LSolutions("Solutions");
const TString LSize("Size");
const TString LSinglePop("Single Population");
const TString LRunNumber("Run Number");

} // namespace Log

} // namespace Tigon
