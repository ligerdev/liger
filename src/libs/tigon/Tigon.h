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
#pragma once

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

// Tigon Object
#include <tigon/Core/TObject.h>

// Random
#include <tigon/Random/RandomGenerator.h>

// Log
#include <tigon/Log/LogManager.h>
#include <tigon/Log/LogEntry.h>
#include <tigon/Log/EvaluationEntry.h>
#include <tigon/Log/PopulationEntry.h>

// Factories - TODO: migrate all factories in here
#include <tigon/Factories/RobustnessIndicatorFactory.h>

// Representation - Basic Classes
#include <tigon/Representation/PSets/IPSet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Problems/Problem.h>

// Representation - Container
#include <tigon/Representation/Container/SampleVectors.h>
#include <tigon/Representation/Container/TimeSeriesContainer.h>

// Representation - IPSets
#include <tigon/Representation/PSets/IPSetPrivate.h>
#include <tigon/Representation/PSets/PSetBase.h>
#include <tigon/Representation/PSets/IPSetDecorator.h>

// Representation - Mappings
#include <tigon/Representation/Mappings/IMappingPrivate.h>
//#include <tigon/Representation/Mappings/QuadraticObjectiveData.h>
#include <tigon/Representation/Mappings/IMappingOperations.h>
#include <tigon/Representation/Mappings/UncertaintyMapping.h>

// Representation - IElements
#include <tigon/Representation/Elements/IElementOperations.h>

// Representation - Constraints
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Representation/Constraints/LinearConstraintsData.h>
#include <tigon/Representation/Constraints/NonLinearConstraintsData.h>

// Representation - Properties
#include <tigon/Representation/Properties/ElementProperties.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <tigon/Representation/Properties/ProblemProperties.h>
#include <tigon/Representation/Properties/ProblemPropertiesFactory.h>

// Representation - Functions
#include <tigon/Representation/Functions/ExampleFunction.h>
#include <tigon/Representation/Functions/DTLZ/DTLZProblems.h>
#include <tigon/Representation/Functions/DTLZ/DTLZ1.h>
#include <tigon/Representation/Functions/DTLZ/DTLZ2.h>
#include <tigon/Representation/Functions/DTLZ/DTLZ5_2.h>
#include <tigon/Representation/Functions/DTLZ/DTLZ5_3.h>
#include <tigon/Representation/Functions/SingleObjective/Alpine2.h>
#include <tigon/Representation/Functions/CTP/CTPProblems.h>
#include <tigon/Representation/Functions/CTP/CTP1.h>
#include <tigon/Representation/Functions/CTP/CTP2.h>
#include <tigon/Representation/Functions/CTP/CTP3.h>
#include <tigon/Representation/Functions/CTP/CTP4.h>
#include <tigon/Representation/Functions/CTP/CTP5.h>
#include <tigon/Representation/Functions/CTP/CTP6.h>
#include <tigon/Representation/Functions/CTP/CTP7.h>
#include <tigon/Representation/Functions/TF1.h>
#include <tigon/Representation/Functions/SurrogateModelling/KrigingSurrogate.h>
#include <tigon/Representation/Functions/SurrogateModelling/ExpectedImprovement.h>
#include <tigon/Representation/Functions/SurrogateModelling/FeasibleExpectedImprovement.h>
#include <tigon/Representation/Functions/SurrogateModelling/RandExpectedImprovement.h>
#include <tigon/Representation/Functions/SurrogateModelling/FeasibleRandExpectedImprovement.h>
#include <tigon/Representation/Functions/SurrogateModelling/ProbabilityFeasibility.h>
#include <tigon/Representation/Functions/SurrogateModelling/ConstrainedExpectedImprovement.h>
#include <tigon/Representation/Functions/SurrogateModelling/FannNetwork.h>
#include <tigon/Representation/Functions/SurrogateModelling/FeedforwardSurrogate.h>
#include <tigon/Representation/Functions/SurrogateModelling/ProbabilityFeasibilityBagheri.h>
#include <tigon/Representation/Functions/CODeM/CODeMDistribution.h>
#include <tigon/Representation/Functions/CODeM/UncertaintyKernel.h>
#include <tigon/Representation/Functions/CODeM/CODeMOperators.h>
#include <tigon/Representation/Functions/CODeM/CODeMProblems.h>
#include <tigon/Representation/Functions/CODeM/GECCO2016.h>
#include <tigon/Representation/Functions/CODeM/EMO2019P1.h>
#include <tigon/Representation/Functions/CODeM/EMO2019P2.h>

// Representation - Distributions
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Representation/Distributions/UniformDistribution.h>
#include <tigon/Representation/Distributions/LinearDistribution.h>
#include <tigon/Representation/Distributions/MergedDistribution.h>
#include <tigon/Representation/Distributions/NonParaDistribution.h>
#include <tigon/Representation/Distributions/PeakDistribution.h>
#include <tigon/Representation/Distributions/SampledDistribution.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>
#include <tigon/Representation/Distributions/ChiSquaredDistribution.h>
#include <tigon/Factories/DistributionFactory.h>

// Representation - Indicators
#include <tigon/Representation/Indicators/IRobustnessIndicator.h>
#include <tigon/Representation/Indicators/ConfidenceIndicator.h>
#include <tigon/Representation/Indicators/ThresholdIndicator.h>

// Representation - Problem definition

// Operators
#include <tigon/Operators/IOperator.h>
#include <tigon/Operators/EmptyOperator.h>

// Operators - Archives
#include <tigon/Operators/Archives/IArchive.h>

// Operators - Composites
#include <tigon/Operators/Composites/IComposite.h>
#include <tigon/Operators/Composites/SurrogateBasedOptimizer.h>
#include <tigon/Operators/Composites/SurrogateBasedOptimizerWithPerturbation.h>

// Operators - Convergence
#include <tigon/Operators/Convergence/IConvergence.h>
#include <tigon/Operators/Convergence/RandomHypervolume.h>
#include <tigon/Operators/Convergence/Hypervolume.h>
#include <tigon/Operators/Convergence/ScalarisingSpace.h>
#include <tigon/Operators/Convergence/RobustScalarisingSpace.h>

// Operators - Directions
#include <tigon/Operators/Directions/IDirection.h>
#include <tigon/Operators/Directions/SBXCrossOver.h>
//#include <tigon/Operators/Directions/IPMQPDirection.h>
#include <tigon/Operators/Directions/SinglePointCrossOver.h>
#include <tigon/Operators/Directions/SimplexLatticeDirectionIterator.h>
#include <tigon/Operators/Directions/OrderedCrossOver.h>

// Operators - Evaluators
#include <tigon/Operators/Evaluators/IEvaluator.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Evaluators/Validation.h>
#include <tigon/Operators/Evaluators/ValidationWithScalarisation.h>
#include <tigon/Operators/Evaluators/BatchEvaluator.h>

// Operators - Filtrations
#include <tigon/Operators/Filtrations/IFiltration.h>
#include <tigon/Operators/Filtrations/PSAClustering.h>
#include <tigon/Operators/Filtrations/RandFiltrationForDirection.h>
#include <tigon/Operators/Filtrations/RandFiltrationForModification.h>
#include <tigon/Operators/Filtrations/PSAEliteSelection.h>
#include <tigon/Operators/Filtrations/TournamentFiltrationForModification.h>
#include <tigon/Operators/Filtrations/RandFiltrationForPerturbation.h>
#include <tigon/Operators/Filtrations/FreezeOperatorOutput.h>
#include <tigon/Operators/Filtrations/MergeForNextIteration.h>
#include <tigon/Operators/Filtrations/NeighbourhoodFiltration.h>
#include <tigon/Operators/Filtrations/FitnessEliteSelection.h>
#include <tigon/Operators/Filtrations/MultiMemberTournamentForDirection.h>
#include <tigon/Operators/Filtrations/MultiMemberTournamentForPerturbation.h>
#include <tigon/Operators/Filtrations/NSGAIIEliteSelection.h>
#include <tigon/Operators/Filtrations/FiltrationForDirection.h>
#include <tigon/Operators/Filtrations/RandSetReplacement.h>
#include <tigon/Operators/Filtrations/TruncateSets.h>
#include <tigon/Operators/Filtrations/TournamentFiltrationForDirection.h>
#include <tigon/Operators/Filtrations/SMSEMOAReduce.h>
#include <tigon/Operators/Filtrations/RouletteWheelSelection.h>
#include <tigon/Operators/Filtrations/StochasticUniversalSampling.h>

// Operators - Inititalisation
#include <tigon/Operators/Initialisation/IInitialisation.h>
//#include <tigon/Operators/Initialisation/IPMQPInitialisation.h>
#include <tigon/Operators/Initialisation/RandomInit.h>
#include <tigon/Operators/Initialisation/LHSInit.h>
#include <tigon/Operators/Initialisation/FitnessBasedInit.h>
#include <tigon/Operators/Initialisation/UserDefinedInit.h>
#include <tigon/Operators/Initialisation/WeightVectorInit.h>
#include <tigon/Operators/Initialisation/TSPOrderedInit.h>

// Operators - Perturbations
#include <tigon/Operators/Perturbations/IPerturbation.h>
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
#include <tigon/Operators/Perturbations/BoundedPerturbation.h>
#include <tigon/Operators/Perturbations/CategoricalPerturpation.h>
#include <tigon/Operators/Perturbations/SwapMutation.h>

// Operators - Fitness
#include <tigon/Operators/Fitness/IFitness.h>
#include <tigon/Operators/Fitness/NonDominanceRanking.h>
#include <tigon/Operators/Fitness/NonDominanceRankingParametric.h>
#include <tigon/Operators/Fitness/PSACrowding.h>
#include <tigon/Operators/Fitness/Scalarization.h>
#include <tigon/Operators/Fitness/GeneralizedDecomposition.h>
#include <tigon/Operators/Fitness/RobustnessAssignment.h>
#include <tigon/Operators/Fitness/UncertainMonteCarloRobustness.h>
#include <tigon/Operators/Fitness/UncertainConfidenceRobustness.h>
#include <tigon/Operators/Fitness/NSGAIICrowding.h>
#include <tigon/Operators/Fitness/ConstraintPenalty.h>
#include <tigon/Operators/Fitness/AverageFitness.h>
#include <tigon/Operators/Fitness/MOGANonDominanceRanking.h>
#include <tigon/Operators/Fitness/SharedFitness.h>

// Operators - Formulations
#include <tigon/Operators/Formulations/IFormulation.h>
#include <tigon/Operators/Formulations/ProblemGenerator/ProblemGenerator.h>

// Operators - Transformations
#include <tigon/Operators/Transformations/ITransformation.h>
#include <tigon/Operators/Transformations/Normalisation.h>
#include <tigon/Operators/Transformations/Denormalisation.h>

// Operators - Algorithm specific
#include <tigon/Operators/AlgorithmSpecific/sParEGO/sParEGOInit.h>
#include <tigon/Operators/AlgorithmSpecific/sParEGO/SparegoValidation.h>
#include <tigon/Operators/AlgorithmSpecific/ParEGO/DirectionFitnessFiltration.h>
#include <tigon/Operators/AlgorithmSpecific/MOEADNeighbourhoodUpdate.h>
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseDiversity.h>
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForDirection.h>
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForPerturbation.h>
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseMutation.h>
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseEliteFiltration.h>

// Operarors - Terminations
#include <tigon/Operators/Terminations/ITermination.h>

// Algorithms
#include <tigon/Algorithms/IAlgorithm.h>
//#include <tigon/Algorithms/IPMQPAlgorithm.h>
#include <tigon/Algorithms/NSGAIIPSA.h>
#include <tigon/Algorithms/NSGAII.h>
#include <tigon/Algorithms/ACROMUSE.h>
#include <tigon/Algorithms/sParEGO.h>
#include <tigon/Algorithms/ParEGO.h>
#include <tigon/Algorithms/MOEAD.h>
#include <tigon/Algorithms/SMSEMOA.h>
#include <tigon/Algorithms/MOGA.h>

// Engine
#include <tigon/Engine/OptimizationLinearFlow.h>
#include <tigon/Engine/TigonEngine.h>
#include <tigon/Engine/TigonOperatorFactory.h>
#include <tigon/Engine/TigonFunctionFactory.h>

// Utils
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/TigonIOUtils.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Utils/AbstractInterpolator.h>
#include <tigon/Utils/ZeroOrderInterpolator.h>
#include <tigon/Utils/LinearInterpolator.h>
#include <tigon/Utils/SplineInterpolator.h>
#include <tigon/Utils/RBFBasis.h>
#include <tigon/Utils/RBFGauss.h>
#include <tigon/Utils/RBFMultiQuadric.h>
#include <tigon/Utils/RBFInverseMultiQuadric.h>
#include <tigon/Utils/RBFThinPlate.h>
#include <tigon/Utils/RBFInterpolator.h>
#include <tigon/Utils/RBFInterpolatorCascade.h>
#include <tigon/Utils/PolynomialRegression.h>
#include <tigon/Utils/MultiPolynomialRegression.h>
#include <tigon/Utils/Kriging/IKriging.h>
#include <tigon/Utils/Kriging/DACE.h>
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/Kriging/KrigingVariogram.h>
#include <tigon/Utils/Kriging/PowerVariogram.h>
#include <tigon/Utils/Kriging/KrigingCascade.h>
#include <tigon/Utils/Kriging/SphericalVariogram.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/ScalarisingFunctions.h>
#include <tigon/Utils/ProjectionUtils.h>
#include <tigon/Utils/SimplexLattice.h>
#include <tigon/Utils/HypercubeGrid.h>
#include <tigon/Utils/BoxConstraintViolationCorrections.h>
#include <tigon/Utils/HypervolumeUtils.h>
#include <tigon/Utils/ScalarisingSpaceUtils.h>
#include <tigon/Utils/KernelDensityEstimation.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/JsonUtils.h>
#include <tigon/Utils/CorrelationMatrix.h>
#include <tigon/Utils/ObjectiveReduction.h>


// Exception
#include <tigon/ExceptionHandling/TException.h>

// Plugin
#include <tigon/Core/IPlugin.h>
#include <tigon/Core/PluginManager.h>

// MATLAB Functionality
#include <tigon/Operators/Evaluators/BatchEvaluator.h>

// Python Functionality
#ifdef PYTHON_API
#include <tigon/Representation/Functions/Python/PythonFunction.h>
#endif
