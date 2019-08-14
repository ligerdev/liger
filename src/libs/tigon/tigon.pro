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
    Algorithms/IAlgorithm.cpp \
    Operators/Archives/IArchive.cpp \
    Operators/Directions/IDirection.cpp \
    Operators/Evaluators/IEvaluator.cpp \
    Operators/Filtrations/IFiltration.cpp \
    Operators/Fitness/IFitness.cpp \
    Operators/Initialisation/IInitialisation.cpp \
    Operators/Initialisation/LHSInit.cpp \
    Operators/Initialisation/RandomInit.cpp \
    Operators/Perturbations/IPerturbation.cpp \
    Operators/IOperator.cpp \
    Representation/Constraints/BoxConstraintsData.cpp \
    Representation/Elements/IElement.cpp \
    Representation/Mappings/IMapping.cpp \
    Representation/PSets/IPSet.cpp \
    Representation/PSets/IPSetDecorator.cpp \
    Representation/PSets/IPSetPrivate.cpp \
    Representation/Constraints/LinearConstraintsData.cpp \
    Representation/Constraints/NonLinearConstraintsData.cpp \
    Representation/PSets/PSetBase.cpp \
    Operators/Transformations/ITransformation.cpp \
    Operators/Filtrations/PSAClustering.cpp \
    Operators/Directions/SBXCrossOver.cpp \
    Operators/Filtrations/RandFiltrationForDirection.cpp \
    Operators/Filtrations/RandFiltrationForModification.cpp \
    Representation/Mappings/IMappingPrivate.cpp \
    Representation/Properties/ElementProperties.cpp \
    Representation/Properties/ElementPropertiesFactory.cpp \
    Representation/Properties/ProblemProperties.cpp \
    Representation/Properties/ProblemPropertiesFactory.cpp \
    Operators/Formulations/ProblemGenerator/ProblemGenerator.cpp \
    Operators/Fitness/NonDominanceRanking.cpp \
    Operators/Fitness/PSACrowding.cpp \
    Representation/Sets/ISet.cpp \
    Representation/Properties/ElementPropertiesData.cpp \
    Representation/Properties/ProblemPropertiesData.cpp \
    Utils/TigonUtils.cpp \
    Engine/TigonEngine.cpp \
    Engine/OptimizationLinearFlow.cpp \
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
    Utils/KrigingVariogram.cpp \
    Utils/Kriging.cpp \
    Utils/PowerVariogram.cpp \
    Utils/KrigingCascade.cpp \
    Utils/SphericalVariogram.cpp \
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
    Representation/Functions/KrigingSurrogate.cpp \
    Representation/Functions/SingleObjective/Alpine2.cpp \
    Representation/Functions/ExpectedImprovement.cpp \
    Representation/Functions/RandExpectedImprovement.cpp \
    Representation/Functions/FeedforwardSurrogate.cpp \
    Representation/Functions/FannNetwork.cpp \
    Utils/ScalarisingFunctions.cpp \
    Utils/ProjectionUtils.cpp \
    Utils/NormalisationUtils.cpp \
    Utils/SimplexLattice.cpp \
    Representation/Distributions/SampledDistribution.cpp \
    Representation/Distributions/NormalDistribution.cpp \
    Operators/Filtrations/PSAEliteSelection.cpp \
    Operators/Filtrations/TournamentFiltrationForModification.cpp \
    Utils/BoxConstraintViolationCorrections.cpp \
    Operators/Perturbations/PolynomialMutation.cpp \
    Operators/Filtrations/RandFiltrationForPerturbation.cpp \
    Operators/Filtrations/FreezeOperatorOutput.cpp \
    Operators/Filtrations/MergeForNextIteration.cpp \
    Algorithms/NSGAIIPSA.cpp \
    Operators/Directions/SinglePointCrossOver.cpp \
    Operators/Directions/SimplexLatticeDirectionIterator.cpp \
    Operators/Fitness/GeneralizedDecomposition.cpp \
    Operators/Filtrations/NeighbourhoodFiltration.cpp \
    Operators/Fitness/RobustnessAssignment.cpp \
    Operators/Filtrations/FitnessEliteSelection.cpp \
    Operators/Evaluators/Evaluator.cpp \
    Operators/Formulations/IFormulation.cpp \
    Representation/Problems/Problem.cpp \
    Representation/Properties/FunctionPropertiesData.cpp \
    Representation/Properties/FunctionProperties.cpp \
    Representation/Properties/FunctionPropertiesFactory.cpp \
    Representation/Indicators/IRobustnessIndicator.cpp \
    Representation/Indicators/ConfidenceIndicator.cpp \
    Representation/Indicators/ThresholdIndicator.cpp \
    Operators/Composites/IComposite.cpp \
    Operators/Composites/SurrogateBasedOptimizer.cpp \
    Operators/Transformations/Normalisation.cpp \
    Operators/Initialisation/FitnessBasedInit.cpp \
    Operators/Filtrations/MultiMemberTournamentForDirection.cpp \
    Operators/Filtrations/MultiMemberTournamentForPerturbation.cpp \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseDiversity.cpp \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForDirection.cpp \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForPerturbation.cpp \
    Operators/Transformations/Denormalisation.cpp \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseMutation.cpp \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseEliteFiltration.cpp \
    Operators/AlgorithmSpecific/parego/DirectionFitnessFiltration.cpp \
    Algorithms/ACROMUSE.cpp \
    Operators/Initialisation/UserDefinedInit.cpp \
    Representation/Distributions/ChiSquaredDistribution.cpp \
    Operators/Evaluators/Validation.cpp \
    Operators/Convergence/IConvergence.cpp \
    Utils/Hypervolume/hv.c \
    Operators/EmptyOperator.cpp \
    Operators/Convergence/RandomHypervolume.cpp \
    Utils/KernelDensityEstimation.cpp \
    Algorithms/NSGAII.cpp \
    Operators/Fitness/NSGAIICrowding.cpp \
    Operators/Filtrations/NSGAIIEliteSelection.cpp \
    Operators/Filtrations/FiltrationForDirection.cpp \
    Algorithms/MOEAD.cpp \
    Operators/Initialisation/WeightVectorInit.cpp \
    Operators/Filtrations/RandSetReplacement.cpp \
    Operators/Fitness/UncertainConfidenceRobustness.cpp \
    Operators/Fitness/UncertainMonteCarloRobustness.cpp \
    Random/RandomGenerator.cpp \
    Engine/TigonOperatorFactory.cpp \
    Utils/DominanceUtils.cpp \
    Operators/Perturbations/BoundedPerturbation.cpp \
    Operators/Fitness/Scalarization.cpp \
    Representation/Elements/IElementOperations.cpp \
    Representation/Mappings/IMappingOperations.cpp \
    Utils/IElementUtils.cpp \
    Operators/AlgorithmSpecific/MOEADNeighbourhoodUpdate.cpp \
    Engine/TigonFunctionFactory.cpp \
    Operators/Filtrations/TruncateSets.cpp \
    Operators/Filtrations/TournamentFiltrationForDirection.cpp \
    Representation/Mappings/UncertaintyMapping.cpp \
    Factories/RobustnessIndicatorFactory.cpp \
    Factories/DistributionFactory.cpp \
    Operators/Fitness/ConstraintPenalty.cpp \
    ExceptionHandling/TException.cpp \
    Operators/Perturbations/CategoricalPerturpation.cpp \
    Log/LogEntry.cpp \
    Log/EvaluationEntry.cpp \
    Log/LogManager.cpp \
    Log/PopulationEntry.cpp \
    Utils/JsonUtils.cpp \
    Utils/HypervolumeUtils.cpp \
    Operators/Convergence/Hypervolume.cpp \
    Algorithms/ParEGO.cpp \
    Operators/Composites/SurrogateBasedOptimizerWithPerturbation.cpp \
    Operators/Evaluators/BatchEvaluator.cpp \
    Utils/BatchSolveRegister.cpp \
    Operators/Perturbations/SwapMutation.cpp \
    Operators/Directions/OrderedCrossOver.cpp \
    Utils/ScalarisingSpaceUtils.cpp \
    Operators/Convergence/ScalarisingSpace.cpp \
    Operators/Convergence/RobustScalarisingSpace.cpp \
    Utils/TigonIOUtils.cpp \
    Utils/PolynomialRegression.cpp \
    Utils/MultiPolynomialRegression.cpp \
    Utils/HypercubeGrid.cpp \
    Representation/Container/SampleVectors.cpp \
    Representation/Container/TimeSeriesContainer.cpp \
    Operators/Initialisation/TSPOrderedInit.cpp \
    Core/PluginManager.cpp \
    Operators/Terminations/ITermination.cpp \
    Algorithms/SMSEMOA.cpp \
    Operators/Filtrations/SMSEMOAReduce.cpp \
    Operators/Fitness/NonDominanceRankingParametric.cpp \
    Representation/Functions/TF1.cpp \
    Utils/CorrelationMatrix.cpp \
    Utils/ObjectiveReduction.cpp \
    Algorithms/sParEGO.cpp \
    Operators/AlgorithmSpecific/Sparego/SparegoInit.cpp \
    Operators/AlgorithmSpecific/Sparego/SparegoValidation.cpp \
    Operators/Evaluators/ValidationWithScalarisation.cpp \
    Representation/Functions/CODeM/CODeMDistribution.cpp \
    Representation/Functions/CODeM/CODeMOperators.cpp \
    Representation/Functions/CODeM/CODeMProblems.cpp \
    Representation/Functions/CODeM/EMO2019P1.cpp \
    Representation/Functions/CODeM/EMO2019P2.cpp \
    Representation/Functions/CODeM/GECCO2016.cpp \
    Representation/Functions/CODeM/UncertaintyKernel.cpp \
    Utils/Dominance/ConstrDomRelation.cpp \
    Utils/Dominance/DominanceRelation.cpp \
    Utils/Dominance/IOrderedRelation.cpp \
    Utils/Dominance/Preferability.cpp \
    Utils/Dominance/PreferabilityConstraintHandling.cpp \
    Algorithms/MOGA.cpp \
    Operators/Fitness/AverageFitness.cpp \
    Operators/Fitness/MOGANonDominanceRanking.cpp \
    Operators/Fitness/SharedFitness.cpp \
    Operators/Filtrations/StochasticUniversalSampling.cpp \
    Operators/Filtrations/RouletteWheelSelection.cpp \
    Factories/DominanceRelationFactory.cpp \
    Representation/Functions/MinEx.cpp

HEADERS += \
    Core/TSharedPointer.h \
    Core/TVector.h \
    Core/TString.h \
    Core/TMap.h \
    Core/TObject.h \
    Algorithms/IAlgorithm.h \
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
    Representation/Constraints/BoxConstraintsData.h \
    Representation/Elements/IElement.h \
    Representation/Mappings/IMapping.h \
    Representation/PSets/IPSet.h \
    Representation/PSets/IPSetDecorator.h \
    Representation/PSets/IPSetPrivate.h \
    Representation/Constraints/LinearConstraintsData.h \
    Representation/Constraints/NonLinearConstraintsData.h \
    Representation/PSets/PSetBase.h \
    Tigon.h \
    tigon_global.h \
    tigonconstants.h \
    Operators/Transformations/ITransformation.h \
    Operators/Filtrations/PSAClustering.h \
    Operators/Directions/SBXCrossOver.h \
    Operators/Filtrations/RandFiltrationForDirection.h \
    Operators/Filtrations/RandFiltrationForModification.h \
    Representation/Mappings/IMappingPrivate.h \
    Representation/Properties/ElementProperties.h \
    Representation/Properties/ProblemProperties.h \
    Representation/Properties/ProblemPropertiesFactory.h \
    Representation/Properties/ElementPropertiesFactory.h \
    Operators/Formulations/ProblemGenerator/ProblemGenerator.h \
    Operators/Fitness/NonDominanceRanking.h \
    Operators/Fitness/PSACrowding.h \
    Representation/Sets/ISet.h \
    Representation/Properties/ElementPropertiesData.h \
    Representation/Properties/ProblemPropertiesData.h \
    Utils/TigonUtils.h \
    Engine/TigonEngine.h \
    Engine/OptimizationLinearFlow.h \
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
    Utils/KrigingVariogram.h \
    Utils/Kriging.h \
    Utils/PowerVariogram.h \
    Utils/KrigingCascade.h \
    Utils/SphericalVariogram.h \
    Representation/Distributions/IDistribution.h \
    Representation/Distributions/UniformDistribution.h \
    Representation/Distributions/LinearDistribution.h \
    Representation/Distributions/MergedDistribution.h \
    Representation/Distributions/NonParaDistribution.h \
    Representation/Distributions/PeakDistribution.h \
    Utils/ScalarisingFunctions.h \
    Utils/ProjectionUtils.h \
    Utils/NormalisationUtils.h \
    Utils/SimplexLattice.h \
    Representation/Distributions/SampledDistribution.h \
    Representation/Distributions/NormalDistribution.h \
    Operators/Filtrations/PSAEliteSelection.h \
    Operators/Filtrations/TournamentFiltrationForModification.h \
    Utils/BoxConstraintViolationCorrections.h \
    Operators/Perturbations/PolynomialMutation.h \
    Operators/Filtrations/RandFiltrationForPerturbation.h \
    Operators/Filtrations/FreezeOperatorOutput.h \
    Operators/Filtrations/MergeForNextIteration.h \
    Algorithms/NSGAIIPSA.h \
    Operators/Directions/SinglePointCrossOver.h \
    Operators/Directions/SimplexLatticeDirectionIterator.h \
    Operators/Fitness/GeneralizedDecomposition.h \
    Operators/Filtrations/NeighbourhoodFiltration.h \
    Operators/Fitness/RobustnessAssignment.h \
    Operators/Filtrations/FitnessEliteSelection.h \
    Operators/Evaluators/Evaluator.h \
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
    Representation/Functions/KrigingSurrogate.h \
    Representation/Functions/SingleObjective/Alpine2.h \
    Representation/Functions/ExpectedImprovement.h \
    Representation/Functions/FeedforwardSurrogate.h \
    Representation/Functions/FannNetwork.h \
    Representation/Functions/RandExpectedImprovement.h \
    Operators/Formulations/IFormulation.h \
    Representation/Problems/Problem.h \
    Representation/Properties/FunctionPropertiesData.h \
    Representation/Properties/FunctionProperties.h \
    Representation/Properties/FunctionPropertiesFactory.h \
    Representation/Indicators/IRobustnessIndicator.h \
    Representation/Indicators/ConfidenceIndicator.h \
    Representation/Indicators/ThresholdIndicator.h \
    Operators/Composites/IComposite.h \
    Operators/Composites/SurrogateBasedOptimizer.h \
    Operators/Transformations/Normalisation.h \
    Operators/Initialisation/FitnessBasedInit.h \
    Operators/Filtrations/MultiMemberTournamentForDirection.h \
    Operators/Filtrations/MultiMemberTournamentForPerturbation.h \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseDiversity.h \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForDirection.h \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForPerturbation.h \
    Operators/Transformations/Denormalisation.h \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseMutation.h \
    Operators/AlgorithmSpecific/ACROMUSE/AcromuseEliteFiltration.h \
    Operators/AlgorithmSpecific/parego/DirectionFitnessFiltration.h \
    Algorithms/ACROMUSE.h \
    Operators/Initialisation/UserDefinedInit.h \
    Representation/Distributions/ChiSquaredDistribution.h \
    Operators/Evaluators/Validation.h \
    Operators/Convergence/IConvergence.h \
    Utils/Hypervolume/hv.h \
    Algorithms/NSGAII.h \
    Operators/Fitness/NSGAIICrowding.h \
    Operators/Filtrations/NSGAIIEliteSelection.h \
    Operators/Filtrations/FiltrationForDirection.h \
    Operators/EmptyOperator.h \
    Operators/Convergence/RandomHypervolume.h \
    Utils/KernelDensityEstimation.h \
    Operators/Fitness/UncertainConfidenceRobustness.h \
    Operators/Fitness/UncertainMonteCarloRobustness.h \
    Random/RandomGenerator.h  \
    Engine/TigonOperatorFactory.h \
    Engine/IPSetFactory.h \
    tigonengineregistry.h \
    Algorithms/MOEAD.h \
    Operators/Initialisation/WeightVectorInit.h \
    Operators/Filtrations/RandSetReplacement.h \
    Utils/DominanceUtils.h \
    Operators/Perturbations/BoundedPerturbation.h \
    Operators/Fitness/Scalarization.h \
    Representation/Elements/IElementOperations.h \
    Representation/Mappings/IMappingOperations.h \
    Representation/Mappings/UncertaintyMapping.h \
    Utils/IElementUtils.h \
    Operators/AlgorithmSpecific/MOEADNeighbourhoodUpdate.h \
    Operators/Filtrations/TruncateSets.h \
    Operators/Filtrations/TournamentFiltrationForDirection.h \
    Factories/RobustnessIndicatorFactory.h  \
    Engine/IFunctionFactory.h \
    Engine/TigonFunctionFactory.h \
    Engine/TigonEngineConstants.h \
    Factories/DistributionFactory.h \
    Operators/Fitness/ConstraintPenalty.h \
    ExceptionHandling/TException.h \
    Operators/Perturbations/CategoricalPerturpation.h \
    Log/LogEntry.h \
    Log/EvaluationEntry.h \
    Log/LogManager.h \
    Log/PopulationEntry.h \
    Utils/JsonUtils.h \
    Utils/HypervolumeUtils.h \
    Operators/Convergence/Hypervolume.h \
    Algorithms/ParEGO.h \
    Operators/Composites/SurrogateBasedOptimizerWithPerturbation.h \
    Operators/Evaluators/BatchEvaluator.h \
    Utils/BatchSolveRegister.h \
    Operators/Perturbations/SwapMutation.h \
    Operators/Directions/OrderedCrossOver.h \
    Core/TSharedPointer.h \
    Core/TVector.h \
    Core/TString.h \
    Core/TMap.h \
    Utils/TigonIOUtils.h \
    Utils/ScalarisingSpaceUtils.h \
    Operators/Convergence/ScalarisingSpace.h \
    Operators/Convergence/RobustScalarisingSpace.h \
    Utils/TigonIOUtils.h \
    Utils/PolynomialRegression.h \
    Utils/MultiPolynomialRegression.h \
    Utils/HypercubeGrid.h \
    Representation/Container/SampleVectors.h \
    Representation/Container/TimeSeriesContainer.h \
    Operators/Initialisation/TSPOrderedInit.h \
    Core/IPlugin.h \
    Core/PluginManager.h \
    Operators/Terminations/ITermination.h \
    Algorithms/SMSEMOA.h \
    Operators/Filtrations/SMSEMOAReduce.h \
    Operators/Fitness/NonDominanceRankingParametric.h \
    Representation/Functions/TF1.h \
    Utils/CorrelationMatrix.h \
    Utils/ObjectiveReduction.h \
    Algorithms/sParEGO.h \
    Operators/AlgorithmSpecific/Sparego/SparegoInit.h \
    Operators/AlgorithmSpecific/Sparego/SparegoValidation.h \
    Operators/Evaluators/ValidationWithScalarisation.h \
    Representation/Functions/CODeM/CODeMDistribution.h \
    Representation/Functions/CODeM/CODeMOperators.h \
    Representation/Functions/CODeM/CODeMProblems.h \
    Representation/Functions/CODeM/EMO2019P1.h \
    Representation/Functions/CODeM/EMO2019P2.h \
    Representation/Functions/CODeM/GECCO2016.h \
    Representation/Functions/CODeM/UncertaintyKernel.h \
    Utils/Dominance/ConstrDomRelation.h \
    Utils/Dominance/DominanceRelation.h \
    Utils/Dominance/IOrderedRelation.h \
    Utils/Dominance/Preferability.h \
    Utils/Dominance/PreferabilityConstraintHandling.h \
    Algorithms/MOGA.h \
    Operators/Fitness/AverageFitness.h \
    Operators/Fitness/MOGANonDominanceRanking.h \
    Operators/Fitness/SharedFitness.h \
    Operators/Filtrations/StochasticUniversalSampling.h \
    Operators/Filtrations/RouletteWheelSelection.h \
    Factories/DominanceRelationFactory.h \
    Representation/Functions/MinEx.h

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
      LIBS += $$(BOOST_PYTHON_LIB)/libboost_python.so
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
                     $$(BOOST_LIB)/boost_system-vc???-mt-$$BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_* \
                     $$(BOOST_LIB)/boost_python-vc???-mt-$$BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_*
    } else : exists($$(BOOST_LIB)/boost_filesystem-vc???-mt-1_*) {
        BOOST_DLLs = $$(BOOST_LIB)/boost_filesystem-vc???-mt-$$BOOST_DEBUG_FLAG1_* \
                     $$(BOOST_LIB)/boost_system-vc???-mt-$$BOOST_DEBUG_FLAG1_* \
                     $$(BOOST_LIB)/boost_python-vc???-mt-$$BOOST_DEBUG_FLAG1_*
    } else {
        message("The Boost libraries were not found.$$escape_expand(\\n) \
                $$escape_expand(\\t)Check that BOOST_INCLUDE, BOOST_LIB and BOOST_PYTHON_LIB are correctly set in the Project environemt variables.$$escape_expand(\\n) \
                $$escape_expand(\\t)Check that the files boost*vc???-mt-BOOST_DEBUG_FLAG$$BOOST_LIB_ARCH-1_* exist in $$BOOST_LIB folder")
    }
    copyToDir($$BOOST_DLLs, $$LIGER_BIN_PATH/)
}
