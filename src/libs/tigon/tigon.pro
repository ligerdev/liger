include(python.pri)
include(../boost.pri)
include(../../ligerlibrary.pri)
include(tigonresource.pri)

dll {
    DEFINES += LIGER_TIGON_LIB
} else {
    DEFINES += LIGER_TIGON_STATIC_LIB
}
INCLUDEPATH += $$PWD

CONFIG(debug, debug|release) {
    # Enable the following for debuging purpose
    DEFINES += TIGON_DEBUG
}

CONFIG += c++11
#CONFIG += coverage

win32-msvc* {
    # QMAKE_CXXFLAGS   += /WX
    QMAKE_CXXFLAGS_DEBUG   += /Od
    QMAKE_CXXFLAGS_RELEASE += /O2
    QMAKE_CXXFLAGS += -wd4251 -wd4267 -wd4275
}

macx-clang {
    QMAKE_CXXFLAGS_DEBUG   += -O0
    QMAKE_CXXFLAGS_RELEASE += -O3
    # Disable certain warnings for now
    QMAKE_CXXFLAGS += -Wno-sign-compare -Wno-ignored-attributes
}

linux-g++ {
    QMAKE_CXXFLAGS_DEBUG   += -O0
    QMAKE_CXXFLAGS_RELEASE += -O3
    # Disable certain warnings for now
    QMAKE_CXXFLAGS += -Wno-sign-compare -Wno-int-in-bool-context -Wno-ignored-attributes
}

PRECOMPILED_HEADER = $$PWD/tigon_pch.h

# Coverage
contains(CONFIG, coverage) {
    message("Add coverage analysis")
    include(coverage.pri)
}

SOURCES += \
    Core/TObject.cpp \
    Core/PluginManager.cpp \
    Algorithms/IAlgorithm.cpp \
    Algorithms/NSGAIIPSA.cpp \
    Algorithms/ACROMUSE.cpp \
    Algorithms/NSGAII.cpp \
    Algorithms/MOEAD.cpp \
    Algorithms/ParEGO.cpp \
    Algorithms/SMSEMOA.cpp \
    Algorithms/sParEGO.cpp \
    Algorithms/MOGA.cpp \
    Algorithms/NSGAIII.cpp \
    Engine/TigonEngine.cpp \
    Engine/OptimizationLinearFlow.cpp \
    Engine/TigonFunctionFactory.cpp \
    Engine/TigonOperatorFactory.cpp \
    ExceptionHandling/TException.cpp \
    Factories/RobustnessIndicatorFactory.cpp \
    Factories/DistributionFactory.cpp \
    Factories/DominanceRelationFactory.cpp \
    Log/LogEntry.cpp \
    Log/EvaluationEntry.cpp \
    Log/LogManager.cpp \
    Log/PopulationEntry.cpp \
    Operators/EmptyOperator.cpp \
    Operators/IOperator.cpp \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseDiversity.cpp \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForDirection.cpp \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForPerturbation.cpp \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseMutation.cpp \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseEliteFiltration.cpp \
    Operators/AlgorithmSpecific/ParEGO/DirectionFitnessFiltration.cpp \
    Operators/AlgorithmSpecific/ParEGO/ConstraintDirectionFitnessFiltration.cpp \
    Operators/AlgorithmSpecific/MOEADNeighbourhoodUpdate.cpp \
    Operators/AlgorithmSpecific/sParEGO/sParEGOInit.cpp \
    Operators/AlgorithmSpecific/sParEGO/SparegoValidation.cpp \
    Operators/Archives/IArchive.cpp \
    Operators/Composites/IComposite.cpp \
    Operators/Composites/SurrogateBasedOptimizer.cpp \
    Operators/Composites/ConstrainedSurrogateBasedOptimizer.cpp \
    Operators/Composites/SurrogateBasedOptimizerWithPerturbation.cpp \
    Operators/Convergence/IConvergence.cpp \
    Operators/Convergence/RandomHypervolume.cpp \
    Operators/Convergence/Hypervolume.cpp \
    Operators/Convergence/ScalarisingSpace.cpp \
    Operators/Convergence/RobustScalarisingSpace.cpp \
    Operators/Directions/DiscreteCrossover.cpp \
    Operators/Directions/SBXCrossOver.cpp \
    Operators/Directions/SinglePointCrossOver.cpp \
    Operators/Directions/SimplexLatticeDirectionIterator.cpp \
    Operators/Directions/IDirection.cpp \
    Operators/Directions/OrderedCrossOver.cpp \
    Operators/Evaluators/BatchEvaluator.cpp \
    Operators/Evaluators/ValidationWithScalarisation.cpp \
    Operators/Evaluators/IEvaluator.cpp \
    Operators/Evaluators/Evaluator.cpp \
    Operators/Evaluators/Validation.cpp \
    Operators/Filtrations/PSAClustering.cpp \
    Operators/Filtrations/RandFiltrationForDirection.cpp \
    Operators/Filtrations/RandFiltrationForModification.cpp \
    Operators/Filtrations/PSAEliteSelection.cpp \
    Operators/Filtrations/TournamentFiltrationForModification.cpp \
    Operators/Filtrations/RandFiltrationForPerturbation.cpp \
    Operators/Filtrations/FreezeOperatorOutput.cpp \
    Operators/Filtrations/MergeForNextIteration.cpp \
    Operators/Filtrations/NeighbourhoodFiltration.cpp \
    Operators/Filtrations/FitnessEliteSelection.cpp \
    Operators/Filtrations/MultiMemberTournamentForDirection.cpp \
    Operators/Filtrations/MultiMemberTournamentForPerturbation.cpp \
    Operators/Filtrations/NSGAIIEliteSelection.cpp \
    Operators/Filtrations/FiltrationForDirection.cpp \
    Operators/Filtrations/RandSetReplacement.cpp \
    Operators/Filtrations/TruncateSets.cpp \
    Operators/Filtrations/TournamentFiltrationForDirection.cpp \
    Operators/Filtrations/SMSEMOAReduce.cpp \
    Operators/Filtrations/StochasticUniversalSampling.cpp \
    Operators/Filtrations/RouletteWheelSelection.cpp \
    Operators/Filtrations/IFiltration.cpp \
    Operators/Fitness/IFitness.cpp \
    Operators/Fitness/NonDominanceRanking.cpp \
    Operators/Fitness/PSACrowding.cpp \
    Operators/Fitness/GeneralizedDecomposition.cpp \
    Operators/Fitness/RobustnessAssignment.cpp \
    Operators/Fitness/UncertainConfidenceRobustness.cpp \
    Operators/Fitness/UncertainMonteCarloRobustness.cpp \
    Operators/Fitness/Scalarization.cpp \
    Operators/Fitness/NonDominanceRankingParametric.cpp \
    Operators/Fitness/AverageFitness.cpp \
    Operators/Fitness/MOGANonDominanceRanking.cpp \
    Operators/Fitness/SharedFitness.cpp \
    Operators/Fitness/NSGAIICrowding.cpp \
    Operators/Fitness/ConstraintPenalty.cpp \
    Operators/Fitness/NSGAIIINiching.cpp \
    Operators/Formulations/IFormulation.cpp \
    Operators/Formulations/ProblemGenerator/ProblemGenerator.cpp \
    Operators/Initialisation/UserDefinedInit.cpp \
    Operators/Initialisation/WeightVectorInit.cpp \
    Operators/Initialisation/TSPOrderedInit.cpp \
    Operators/Initialisation/FitnessBasedInit.cpp \
    Operators/Initialisation/IInitialisation.cpp \
    Operators/Initialisation/LHSInit.cpp \
    Operators/Initialisation/RandomInit.cpp \
    Operators/Perturbations/IPerturbation.cpp \
    Operators/Perturbations/IntegerMutation.cpp \
    Operators/Perturbations/PolynomialMutation.cpp \
    Operators/Perturbations/BoundedPerturbation.cpp \
    Operators/Perturbations/CategoricalPerturpation.cpp \
    Operators/Perturbations/SwapMutation.cpp \
    Operators/Transformations/ITransformation.cpp \
    Operators/Transformations/Normalisation.cpp \
    Operators/Transformations/Denormalisation.cpp \
    Operators/Terminations/ITermination.cpp \
    Random/RandomGenerator.cpp \
    Representation/Constraints/BoxConstraintsData.cpp \
    Representation/Elements/IElement.cpp \
    Representation/Mappings/IMapping.cpp \
    Representation/PSets/IPSet.cpp \
    Representation/PSets/IPSetDecorator.cpp \
    Representation/PSets/IPSetPrivate.cpp \
    Representation/Constraints/LinearConstraintsData.cpp \
    Representation/Constraints/NonLinearConstraintsData.cpp \
    Representation/PSets/PSetBase.cpp \
    Representation/Mappings/IMappingPrivate.cpp \
    Representation/Properties/ElementProperties.cpp \
    Representation/Properties/ElementPropertiesFactory.cpp \
    Representation/Properties/ProblemProperties.cpp \
    Representation/Properties/ProblemPropertiesFactory.cpp \
    Representation/Sets/ISet.cpp \
    Representation/Properties/ElementPropertiesData.cpp \
    Representation/Properties/ProblemPropertiesData.cpp \
    Representation/Distributions/IDistribution.cpp \
    Representation/Distributions/UniformDistribution.cpp \
    Representation/Distributions/LinearDistribution.cpp \
    Representation/Distributions/MergedDistribution.cpp \
    Representation/Distributions/NonParaDistribution.cpp \
    Representation/Distributions/PeakDistribution.cpp \
    Representation/Functions/ExampleFunction.cpp  \
    Representation/Functions/WFG/wfg_toolkit/ExampleProblems.cpp \
    Representation/Functions/WFG/wfg_toolkit/ExampleShapes.cpp \
    Representation/Functions/WFG/wfg_toolkit/ExampleTransitions.cpp \
    Representation/Functions/WFG/wfg_toolkit/FrameworkFunctions.cpp \
    Representation/Functions/WFG/wfg_toolkit/Misc.cpp \
    Representation/Functions/WFG/wfg_toolkit/ShapeFunctions.cpp \
    Representation/Functions/WFG/wfg_toolkit/TransFunctions.cpp \
    Representation/Functions/IFunction.cpp \
    Representation/Functions/CTP/CTPProblems.cpp \
    Representation/Functions/CTP/CTP1.cpp \
    Representation/Functions/CTP/CTP2.cpp \
    Representation/Functions/CTP/CTP3.cpp \
    Representation/Functions/CTP/CTP4.cpp \
    Representation/Functions/CTP/CTP5.cpp \
    Representation/Functions/CTP/CTP6.cpp \
    Representation/Functions/CTP/CTP7.cpp \
    Representation/Functions/ZDT/ZDTProblems.cpp \
    Representation/Functions/ZDT/ZDT1.cpp \
    Representation/Functions/ZDT/ZDT2.cpp \
    Representation/Functions/DTLZ/DTLZProblems.cpp \
    Representation/Functions/DTLZ/DTLZ1.cpp \
    Representation/Functions/DTLZ/DTLZ2.cpp \
    Representation/Functions/DTLZ/DTLZ5_2.cpp \
    Representation/Functions/DTLZ/DTLZ5_3.cpp \
    Representation/Functions/SurrogateModelling/KrigingSurrogate.cpp \
    Representation/Functions/SurrogateModelling/ExpectedImprovement.cpp \
    Representation/Functions/SurrogateModelling/RandExpectedImprovement.cpp \
    Representation/Functions/SurrogateModelling/FeedforwardSurrogate.cpp \
    Representation/Functions/SurrogateModelling/FannNetwork.cpp \
    Representation/Functions/SurrogateModelling/ConstrainedExpectedImprovement.cpp \
    Representation/Functions/SurrogateModelling/FeasibleExpectedImprovement.cpp \
    Representation/Functions/SurrogateModelling/FeasibleRandExpectedImprovement.cpp \
    Representation/Functions/SurrogateModelling/ProbabilityFeasibility.cpp \
    Representation/Functions/SurrogateModelling/Likelihood.cpp \
    Representation/Functions/SurrogateModelling/ProbabilityFeasibilityBagheri.cpp \
    Representation/Functions/SingleObjective/Alpine2.cpp \
    Representation/Functions/SingleObjective/ConstrainedProblems.cpp \
    Representation/Functions/SingleObjective/BraninGomez.cpp \
    Representation/Functions/SingleObjective/Camel.cpp \
    Representation/Functions/SingleObjective/Sasena.cpp \
    Representation/Functions/SingleObjective/CBranin.cpp \
    Representation/Functions/SingleObjective/CBraninM1.cpp \
    Representation/Functions/SingleObjective/Poly1.cpp \
    Representation/Distributions/SampledDistribution.cpp \
    Representation/Distributions/NormalDistribution.cpp \
    Representation/Problems/Problem.cpp \
    Representation/Properties/FunctionPropertiesData.cpp \
    Representation/Properties/FunctionProperties.cpp \
    Representation/Properties/FunctionPropertiesFactory.cpp \
    Representation/Indicators/IRobustnessIndicator.cpp \
    Representation/Indicators/ConfidenceIndicator.cpp \
    Representation/Indicators/ThresholdIndicator.cpp \
    Representation/Distributions/ChiSquaredDistribution.cpp \
    Representation/Elements/IElementOperations.cpp \
    Representation/Mappings/IMappingOperations.cpp \
    Representation/Mappings/UncertaintyMapping.cpp \
    Representation/Container/SampleVectors.cpp \
    Representation/Container/TimeSeriesContainer.cpp \
    Representation/Functions/TF1.cpp \
    Representation/Functions/CODeM/CODeMDistribution.cpp \
    Representation/Functions/CODeM/CODeMOperators.cpp \
    Representation/Functions/CODeM/CODeMProblems.cpp \
    Representation/Functions/CODeM/EMO2019P1.cpp \
    Representation/Functions/CODeM/EMO2019P2.cpp \
    Representation/Functions/CODeM/GECCO2016.cpp \
    Representation/Functions/CODeM/UncertaintyKernel.cpp \
    Representation/Functions/MinEx.cpp \
    Utils/TigonUtils.cpp \
    Utils/AbstractInterpolator.cpp \
    Utils/LinearInterpolator.cpp \
    Utils/SplineInterpolator.cpp \
    Utils/ZeroOrderInterpolator.cpp \
    Utils/RBFInterpolator.cpp \
    Utils/RBFMultiQuadric.cpp \
    Utils/RBFThinPlate.cpp \
    Utils/RBFGauss.cpp \
    Utils/RBFInverseMultiQuadric.cpp \
    Utils/RBFBasis.cpp \
    Utils/RBFInterpolatorCascade.cpp \
    Utils/Kriging/DACE.cpp \
    Utils/Kriging/IKriging.cpp \
    Utils/Kriging/KrigingCascade.cpp \
    Utils/Kriging/KrigingVariogram.cpp \
    Utils/Kriging/OrdinaryKriging.cpp \
    Utils/Kriging/PowerVariogram.cpp \
    Utils/Kriging/SphericalVariogram.cpp \
    Utils/ScalarisingFunctions.cpp \
    Utils/ProjectionUtils.cpp \
    Utils/NormalisationUtils.cpp \
    Utils/SimplexLattice.cpp \
    Utils/BoxConstraintViolationCorrections.cpp \
    Utils/Hypervolume/hv.c \
    Utils/KernelDensityEstimation.cpp \
    Utils/DominanceUtils.cpp \
    Utils/IElementUtils.cpp \
    Utils/JsonUtils.cpp \
    Utils/HypervolumeUtils.cpp \
    Utils/BatchSolveRegister.cpp \
    Utils/ScalarisingSpaceUtils.cpp \
    Utils/TigonIOUtils.cpp \
    Utils/PolynomialRegression.cpp \
    Utils/MultiPolynomialRegression.cpp \
    Utils/HypercubeGrid.cpp \
    Utils/CorrelationMatrix.cpp \
    Utils/ObjectiveReduction.cpp \
    Utils/Dominance/ConstrDomRelation.cpp \
    Utils/Dominance/DominanceRelation.cpp \
    Utils/Dominance/Preferability.cpp \
    Utils/Dominance/PreferabilityConstraintHandling.cpp

HEADERS += \
    Tigon.h \
    tigon_global.h \
    tigonconstants.h \
    tigonengineregistry.h \
    Algorithms/IAlgorithm.h \
    Algorithms/ACROMUSE.h \
    Algorithms/NSGAIIPSA.h \
    Algorithms/NSGAII.h \
    Algorithms/MOEAD.h \
    Algorithms/ParEGO.h \
    Algorithms/SMSEMOA.h \
    Algorithms/sParEGO.h \
    Algorithms/MOGA.h \
    Algorithms/NSGAIII.h \
    Core/TSharedPointer.h \
    Core/TVector.h \
    Core/TString.h \
    Core/TMap.h \
    Core/TObject.h \
    Core/TSharedPointer.h \
    Core/TVector.h \
    Core/TString.h \
    Core/TMap.h \
    Core/IPlugin.h \
    Core/PluginManager.h \
    Engine/TigonEngine.h \
    Engine/OptimizationLinearFlow.h \
    Engine/TigonOperatorFactory.h \
    Engine/IPSetFactory.h \
    Engine/IFunctionFactory.h \
    Engine/TigonFunctionFactory.h \
    Engine/TigonEngineConstants.h \
    ExceptionHandling/TException.h \
    Factories/DominanceRelationFactory.h \
    Factories/RobustnessIndicatorFactory.h  \
    Factories/DistributionFactory.h \
    Log/LogEntry.h \
    Log/EvaluationEntry.h \
    Log/LogManager.h \
    Log/PopulationEntry.h \
    Operators/Archives/IArchive.h \
    Operators/Directions/IDirection.h \
    Operators/Evaluators/IEvaluator.h \
    Operators/Filtrations/IFiltration.h \
    Operators/Fitness/IFitness.h \
    Operators/Initialisation/IInitialisation.h \
    Operators/Initialisation/LHSInit.h \
    Operators/Initialisation/RandomInit.h \
    Operators/Perturbations/IPerturbation.h \
    Operators/IOperator.h \
    Operators/Transformations/ITransformation.h \
    Operators/Filtrations/PSAClustering.h \
    Operators/Directions/SBXCrossOver.h \
    Operators/Directions/DiscreteCrossover.h \
    Operators/Filtrations/RandFiltrationForDirection.h \
    Operators/Filtrations/RandFiltrationForModification.h \
    Operators/Formulations/ProblemGenerator/ProblemGenerator.h \
    Operators/Fitness/NonDominanceRanking.h \
    Operators/Fitness/PSACrowding.h \
    Operators/Filtrations/PSAEliteSelection.h \
    Operators/Filtrations/TournamentFiltrationForModification.h \
    Operators/Perturbations/PolynomialMutation.h \
    Operators/Filtrations/RandFiltrationForPerturbation.h \
    Operators/Filtrations/FreezeOperatorOutput.h \
    Operators/Filtrations/MergeForNextIteration.h \
    Operators/Directions/SinglePointCrossOver.h \
    Operators/Directions/SimplexLatticeDirectionIterator.h \
    Operators/Fitness/GeneralizedDecomposition.h \
    Operators/Filtrations/NeighbourhoodFiltration.h \
    Operators/Fitness/RobustnessAssignment.h \
    Operators/Filtrations/FitnessEliteSelection.h \
    Operators/Evaluators/Evaluator.h \
    Operators/Formulations/IFormulation.h \
    Operators/Composites/IComposite.h \
    Operators/Composites/SurrogateBasedOptimizer.h \
    Operators/Transformations/Normalisation.h \
    Operators/Initialisation/FitnessBasedInit.h \
    Operators/Filtrations/MultiMemberTournamentForDirection.h \
    Operators/Filtrations/MultiMemberTournamentForPerturbation.h \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseDiversity.h \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForDirection.h \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForPerturbation.h \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseMutation.h \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseEliteFiltration.h \
    Operators/AlgorithmSpecific/ParEGO/DirectionFitnessFiltration.h \
    Operators/AlgorithmSpecific/ParEGO/ConstraintDirectionFitnessFiltration.h \
    Operators/AlgorithmSpecific/MOEADNeighbourhoodUpdate.h \
    Operators/AlgorithmSpecific/sParEGO/sParEGOInit.h \
    Operators/AlgorithmSpecific/sParEGO/SparegoValidation.h \
    Operators/EmptyOperator.h \
    Operators/Convergence/IConvergence.h \
    Operators/Convergence/RandomHypervolume.h \
    Operators/Convergence/Hypervolume.h \
    Operators/Convergence/ScalarisingSpace.h \
    Operators/Convergence/RobustScalarisingSpace.h \
    Operators/Composites/SurrogateBasedOptimizerWithPerturbation.h \
    Operators/Directions/OrderedCrossOver.h \
    Operators/Evaluators/BatchEvaluator.h \
    Operators/Evaluators/Validation.h \
    Operators/Evaluators/ValidationWithScalarisation.h \
    Operators/Filtrations/NSGAIIEliteSelection.h \
    Operators/Filtrations/FiltrationForDirection.h \
    Operators/Filtrations/RandSetReplacement.h \
    Operators/Filtrations/TruncateSets.h \
    Operators/Filtrations/TournamentFiltrationForDirection.h \
    Operators/Filtrations/StochasticUniversalSampling.h \
    Operators/Filtrations/RouletteWheelSelection.h \
    Operators/Filtrations/SMSEMOAReduce.h \
    Operators/Fitness/NSGAIICrowding.h \
    Operators/Fitness/UncertainConfidenceRobustness.h \
    Operators/Fitness/UncertainMonteCarloRobustness.h \
    Operators/Fitness/Scalarization.h \
    Operators/Fitness/ConstraintPenalty.h \
    Operators/Fitness/NonDominanceRankingParametric.h \
    Operators/Fitness/AverageFitness.h \
    Operators/Fitness/MOGANonDominanceRanking.h \
    Operators/Fitness/SharedFitness.h \
    Operators/Fitness/NSGAIIINiching.h \
    Operators/Initialisation/UserDefinedInit.h \
    Operators/Initialisation/TSPOrderedInit.h \
    Operators/Initialisation/WeightVectorInit.h \
    Operators/Perturbations/BoundedPerturbation.h \
    Operators/Perturbations/CategoricalPerturpation.h \
    Operators/Perturbations/IntegerMutation.h \
    Operators/Perturbations/SwapMutation.h \
    Operators/Terminations/ITermination.h \
    Operators/Transformations/Denormalisation.h \
    Random/RandomGenerator.h  \
    Representation/Constraints/BoxConstraintsData.h \
    Representation/Elements/IElement.h \
    Representation/Mappings/IMapping.h \
    Representation/PSets/IPSet.h \
    Representation/PSets/IPSetDecorator.h \
    Representation/PSets/IPSetPrivate.h \
    Representation/Constraints/LinearConstraintsData.h \
    Representation/Constraints/NonLinearConstraintsData.h \
    Representation/PSets/PSetBase.h \
    Representation/Sets/ISet.h \
    Representation/Mappings/IMappingPrivate.h \
    Representation/Properties/ElementProperties.h \
    Representation/Properties/ProblemProperties.h \
    Representation/Properties/ProblemPropertiesFactory.h \
    Representation/Properties/ElementPropertiesFactory.h \
    Representation/Properties/ElementPropertiesData.h \
    Representation/Properties/ProblemPropertiesData.h \
    Representation/Distributions/IDistribution.h \
    Representation/Distributions/UniformDistribution.h \
    Representation/Distributions/LinearDistribution.h \
    Representation/Distributions/MergedDistribution.h \
    Representation/Distributions/NonParaDistribution.h \
    Representation/Distributions/PeakDistribution.h \
    Representation/Distributions/SampledDistribution.h \
    Representation/Distributions/NormalDistribution.h \
    Representation/Functions/IFunction.h \
    Representation/Functions/WFG/wfg_toolkit/ExampleProblems.h \
    Representation/Functions/WFG/wfg_toolkit/ExampleShapes.h \
    Representation/Functions/WFG/wfg_toolkit/ExampleTransitions.h \
    Representation/Functions/WFG/wfg_toolkit/FrameworkFunctions.h \
    Representation/Functions/WFG/wfg_toolkit/Misc.h \
    Representation/Functions/WFG/wfg_toolkit/ShapeFunctions.h \
    Representation/Functions/WFG/wfg_toolkit/TransFunctions.h \
    Representation/Functions/CTP/CTPProblems.h \
    Representation/Functions/CTP/CTP1.h \
    Representation/Functions/CTP/CTP2.h \
    Representation/Functions/CTP/CTP3.h \
    Representation/Functions/CTP/CTP4.h \
    Representation/Functions/CTP/CTP5.h \
    Representation/Functions/CTP/CTP6.h \
    Representation/Functions/CTP/CTP7.h \
    Representation/Functions/ZDT/ZDTProblems.h\
    Representation/Functions/ZDT/ZDT1.h \
    Representation/Functions/ZDT/ZDT2.h \
    Representation/Functions/DTLZ/DTLZProblems.h \
    Representation/Functions/DTLZ/DTLZ1.h \
    Representation/Functions/DTLZ/DTLZ2.h \
    Representation/Functions/DTLZ/DTLZ5_2.h \
    Representation/Functions/DTLZ/DTLZ5_3.h \
    Representation/Functions/ExampleFunction.h  \
    Representation/Functions/SurrogateModelling/KrigingSurrogate.h \
    Representation/Functions/SurrogateModelling/ExpectedImprovement.h \
    Representation/Functions/SurrogateModelling/RandExpectedImprovement.h \
    Representation/Functions/SurrogateModelling/FeedforwardSurrogate.h \
    Representation/Functions/SurrogateModelling/FannNetwork.h \
    Representation/Functions/SurrogateModelling/ConstrainedExpectedImprovement.h \
    Representation/Functions/SurrogateModelling/FeasibleExpectedImprovement.h \
    Representation/Functions/SurrogateModelling/FeasibleRandExpectedImprovement.h \
    Representation/Functions/SurrogateModelling/ProbabilityFeasibility.h \
    Representation/Functions/SurrogateModelling/Likelihood.h \
    Representation/Functions/SurrogateModelling/ProbabilityFeasibilityBagheri.h \
    Representation/Functions/SingleObjective/Alpine2.h \
    Representation/Functions/SingleObjective/ConstrainedProblems.h \
    Representation/Functions/SingleObjective/BraninGomez.h \
    Representation/Functions/SingleObjective/Camel.h \
    Representation/Functions/SingleObjective/Sasena.h \
    Representation/Functions/SingleObjective/CBranin.h \
    Representation/Functions/SingleObjective/CBraninM1.h \
    Representation/Functions/SingleObjective/Poly1.h \
    Representation/Problems/Problem.h \
    Representation/Properties/FunctionPropertiesData.h \
    Representation/Properties/FunctionProperties.h \
    Representation/Properties/FunctionPropertiesFactory.h \
    Representation/Indicators/IRobustnessIndicator.h \
    Representation/Indicators/ConfidenceIndicator.h \
    Representation/Indicators/ThresholdIndicator.h \
    Representation/Distributions/ChiSquaredDistribution.h \
    Representation/Elements/IElementOperations.h \
    Representation/Mappings/IMappingOperations.h \
    Representation/Mappings/UncertaintyMapping.h \
    Representation/Container/SampleVectors.h \
    Representation/Container/TimeSeriesContainer.h \
    Representation/Functions/TF1.h \
    Representation/Functions/CODeM/CODeMDistribution.h \
    Representation/Functions/CODeM/CODeMOperators.h \
    Representation/Functions/CODeM/CODeMProblems.h \
    Representation/Functions/CODeM/EMO2019P1.h \
    Representation/Functions/CODeM/EMO2019P2.h \
    Representation/Functions/CODeM/GECCO2016.h \
    Representation/Functions/CODeM/UncertaintyKernel.h \
    Representation/Functions/MinEx.h \
    Utils/TigonUtils.h \
    Utils/AbstractInterpolator.h \
    Utils/LinearInterpolator.h \
    Utils/SplineInterpolator.h \
    Utils/ZeroOrderInterpolator.h \
    Utils/RBFInterpolator.h \
    Utils/RBFMultiQuadric.h \
    Utils/RBFThinPlate.h \
    Utils/RBFGauss.h \
    Utils/RBFInverseMultiQuadric.h \
    Utils/RBFBasis.h \
    Utils/RBFInterpolatorCascade.h \
    Utils/Kriging/DACE.h \
    Utils/Kriging/IKriging.h \
    Utils/Kriging/KrigingCascade.h \
    Utils/Kriging/KrigingVariogram.h \
    Utils/Kriging/OrdinaryKriging.h \
    Utils/Kriging/PowerVariogram.h \
    Utils/Kriging/SphericalVariogram.h \
    Utils/ScalarisingFunctions.h \
    Utils/ProjectionUtils.h \
    Utils/NormalisationUtils.h \
    Utils/SimplexLattice.h \
    Utils/BoxConstraintViolationCorrections.h \
    Utils/Hypervolume/hv.h \
    Utils/KernelDensityEstimation.h \
    Utils/DominanceUtils.h \
    Utils/IElementUtils.h \
    Utils/JsonUtils.h \
    Utils/HypervolumeUtils.h \
    Utils/BatchSolveRegister.h \
    Utils/TigonIOUtils.h \
    Utils/ScalarisingSpaceUtils.h \
    Utils/TigonIOUtils.h \
    Utils/PolynomialRegression.h \
    Utils/MultiPolynomialRegression.h \
    Utils/HypercubeGrid.h \
    Utils/CorrelationMatrix.h \
    Utils/ObjectiveReduction.h \
    Utils/Dominance/ConstrDomRelation.h \
    Utils/Dominance/DominanceRelation.h \
    Utils/Dominance/Preferability.h \
    Utils/Dominance/PreferabilityConstraintHandling.h

# Python support
contains(HAVE_PYTHON, yes) {
  HEADERS += Representation/Functions/Python/PythonFunction.h
  SOURCES += Representation/Functions/Python/PythonFunction.cpp

  win32 {
      LIBS += -L$$(BOOST_PYTHON_LIB)
  }
  macx {
      LIBS += $$(BOOST_PYTHON_LIB)/libboost_python-mt.dylib
  }
  linux-* {
      exists($$(BOOST_PYTHON_LIB)/libboost_python.so){
          LIBS += $$(BOOST_PYTHON_LIB)/libboost_python.so
      } else : exists($$(BOOST_PYTHON_LIB)/libboost_python27.so){
          LIBS += $$(BOOST_PYTHON_LIB)/libboost_python27.so
      } else {
      message("The Boost python library has not been found.$$escape_expand(\\n) \
          $$escape_expand(\\t)Check that BOOST_PYTHON_LIB is correctly set in the Project environemt variables.$$escape_expand(\\n) \
          $$escape_expand(\\t)Check that the files libboost_python.so or libboost_python27.so exist in $$BOOST_PYTHON_LIB folder")
      }
  }
}

# Boost
win32: LIBS += -L"$$(BOOST_LIB)"
macx {
    LIBS += $$(BOOST_LIB)/libboost_filesystem-mt.dylib \
            $$(BOOST_LIB)/libboost_system-mt.dylib
}
linux-* {
    LIBS += $$(BOOST_LIB)/libboost_filesystem.so \
            $$(BOOST_LIB)/libboost_system.so
}

# Fix dll conflicts between python plugin and matlab
win32 {
    # Change if you need to compile for 32bit target
    BOOST_LIB_ARCH=x64
    CONFIG(debug, debug|release) {
        BOOST_DEBUG_FLAG = gd-
    } else {
        BOOST_DEBUG_FLAG = ""
    }

    exists($$(BOOST_LIB)/boost_filesystem-vc???-mt-$$BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_*){
        BOOST_DLLs = $$(BOOST_LIB)/boost_filesystem-vc???-mt-$$BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_* \
                 $$(BOOST_LIB)/boost_system-vc???-mt-$$BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_*
    } else : exists($$(BOOST_LIB)/boost_filesystem-vc???-mt-1_*) {
        BOOST_DLLs = $$(BOOST_LIB)/boost_filesystem-vc???-mt-$$BOOST_DEBUG_FLAG1_* \
                 $$(BOOST_LIB)/boost_system-vc???-mt-$$BOOST_DEBUG_FLAG1_*
    } else {
        message("The Boost libraries were not found.$$escape_expand(\\n) \
            $$escape_expand(\\t)Check that BOOST_INCLUDE, BOOST_LIB and BOOST_PYTHON_LIB are correctly set in the Project environemt variables.$$escape_expand(\\n) \
            $$escape_expand(\\t)Check that the files boost*vc???-mt-BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_* exist in $$BOOST_LIB folder")
    }

    # Add boost python library
    exists($$(BOOST_LIB)/boost_python27-vc???-mt-$$BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_*){
        BOOST_DLLs += $$(BOOST_LIB)/boost_python27-vc???-mt-$$BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_*
    } else : exists($$(BOOST_LIB)/boost_python-vc???-mt-$$BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_*){
        BOOST_DLLs += $$(BOOST_LIB)/boost_python-vc???-mt-$$BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_*
    } else : exists($$(BOOST_LIB)/boost_python27-vc???-mt-$$BOOST_DEBUG_FLAG1_*){
        BOOST_DLLs += $$(BOOST_LIB)/boost_python27-vc???-mt-$$BOOST_DEBUG_FLAG1_*
    } else : exists($$(BOOST_LIB)/boost_python-vc???-mt-$$BOOST_DEBUG_FLAG1_*){
        BOOST_DLLs += $$(BOOST_LIB)/boost_python-vc???-mt-$$BOOST_DEBUG_FLAG1_*
    } else {
        message("The Boost libraries were not found.$$escape_expand(\\n) \
            $$escape_expand(\\t)Check that BOOST_INCLUDE, BOOST_LIB and BOOST_PYTHON_LIB are correctly set in the Project environemt variables.$$escape_expand(\\n) \
            $$escape_expand(\\t)Check that the files boost*vc???-mt-BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_* exist in $$BOOST_LIB folder")
    }

    copyToDir($$BOOST_DLLs, $$LIGER_BIN_PATH/)
}
