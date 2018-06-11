///****************************************************************************
//**
//** Copyright (C) 2012-2014 The University of Sheffield (www.sheffield.ac.uk)
//** Copyright (C) 2012-2014 Ioannis Giagkiozis
//**
//**
//** This file is part of Liger.
//**
//** GNU Lesser General Public License Usage
//** This file may be used under the terms of the GNU Lesser General
//** Public License version 2.1 as published by the Free Software
//** Foundation and appearing in the file LICENSE.LGPL included in the
//** packaging of this file.  Please review the following information to
//** ensure the GNU Lesser General Public License version 2.1 requirements
//** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
//**
//****************************************************************************/
//#include <designer/iengine/globalspace.h>
//#include <designer/iengine/iproblemreal.h>

////#include <arma/include/armadillo>

//#include <metal/core/IAlgorithm.h>
//#include <metal/core/IOperator.h>
//#include <metal/core/IProblem.h>
//#include <metal/core/Solution.h>
//#include <metal/core/SolutionSet.h>
//#include <metal/core/SolutionType.h>
//#include <metal/core/IVariable.h>

//#include <QList>
//#include <QMap>
//#include <QStringList>

//#include <QVariant>
//#include <QString>
//#include <QVariantMap>

//namespace Designer {

//class GlobalSpacePrivate
//{
//public:
//    GlobalSpacePrivate();
//    ~GlobalSpacePrivate();

//    IProblem*     m_problem;
//    IAlgorithm*   m_algorithm;
//    SolutionSet*  m_population;
//    SolutionType* m_solutionType;
//    arma::mat     m_objectiveVectors;
//    arma::mat     m_decisionVectors;
//    QMap<qint32, IOperator*> m_operators;

//    int          m_nvar;
//    int          m_nind;
//    int          m_nobj;
//    QStringList  m_dvnames;
//    QStringList  m_obnames;
//    QList<qreal> m_lowerBounds;
//    QList<qreal> m_upperBounds;
//    QList<MetalVariableType> m_dvarTypes;

//    // The Uni-bus
//    QVariantMap m_multipack;
//};

//GlobalSpacePrivate::GlobalSpacePrivate()
//    : m_problem(0),
//      m_algorithm(0),
//      m_population(0),
//      m_nvar(0),
//      m_nind(0),
//      m_nobj(0),
//      m_solutionType(0)
//{
//    m_objectiveVectors      = arma::mat(1,1);
//    m_objectiveVectors(0,0) = -1;
//    m_decisionVectors       = arma::mat(1,1);
//    m_decisionVectors(0,0)  = -1;
//}

//GlobalSpacePrivate::~GlobalSpacePrivate()
//{

//}

//GlobalSpace::GlobalSpace()
//    : d(new GlobalSpacePrivate)
//{

//}

//GlobalSpace::GlobalSpace(const GlobalSpace& data)
//    : d(new GlobalSpacePrivate)
//{
//    copy_elements(data);
//}

//GlobalSpace::~GlobalSpace()
//{
//    delete d;
//}

///**
// * Returns the ith solution in the set.
// * @param i Position of the solution to obtain.
// * @return The <code>Solution</code> at the position i.
// * @throws IndexOutOfBoundsException.
// */
//Solution* GlobalSpace::get(int i) const {
//  return d->m_population->get(i);
//} // get

//void GlobalSpace::setProblem(IProblem* prob)
//{
//    if(d->m_population != 0)
//        d->m_population->setProblem(prob);
//    if(d->m_solutionType != 0)
//        d->m_solutionType->setProblem(prob);

//    d->m_problem = prob;
//    if(!(d->m_lowerBounds.isEmpty()))
//        d->m_problem->setLowerLimit(d->m_lowerBounds);
//    if(!(d->m_upperBounds.isEmpty()))
//        d->m_problem->setUpperLimit(d->m_upperBounds);
//}

//void GlobalSpace::setAlgorithm(IAlgorithm* algo)
//{
//    d->m_algorithm = algo;
//}

//void GlobalSpace::setPopulation(SolutionSet* pop)
//{
//    d->m_population       = pop;
//    d->m_objectiveVectors = objectiveVectors();
//    d->m_decisionVectors  = decisionVectors();
//}

//void GlobalSpace::addOperator(qint32 nodeId, IOperator* op)
//{
//    // Make sure we create no duplicates.
//    if(d->m_operators.contains(nodeId))
//        return;

//   d-> m_operators.insert(nodeId, op);
//}

//void GlobalSpace::removeOperator(qint32 nodeId)
//{
//    if(d->m_operators.contains(nodeId))
//        d->m_operators.remove(nodeId);
//}

//bool GlobalSpace::operatorExists(qint32 nodeId)
//{
//    if(d->m_operators.contains(nodeId))
//        return true;
//    else
//        return false;

//    // Unecessary but keeps the compiler happy.
//    return false;
//}

//IProblem* GlobalSpace::problem() const
//{
//    return d->m_problem;
//}

//IAlgorithm* GlobalSpace::algorithm() const
//{
//    return d->m_algorithm;
//}

//IOperator* GlobalSpace::getOperator(qint32 i) const
//{
//    if(i>=0 && i<d->m_operators.size()) {
//      return d->m_operators[i];
//    }
//    return 0;
//}

//QMap<qint32, IOperator *> GlobalSpace::operators() const
//{
//    return d->m_operators;
//}

//SolutionSet* GlobalSpace::population() const
//{
//    return d->m_population;
//}

//arma::mat GlobalSpace::objectiveVectors() const
//{
//    if(d->m_population) {
//        int nind = d->m_population->size();
//        int nobj = get(0)->getNumberOfObjectives();
//        d->m_objectiveVectors = arma::mat(nind,nobj);

//        for (int i = 0; i < nind; i++) {
//            for (int j = 0; j < nobj; j++) {
//                d->m_objectiveVectors(i,j) = get(i)->getObjective(j);
//            }
//        }
//    }

//    return d->m_objectiveVectors;
//}

//arma::mat GlobalSpace::decisionVectors() const
//{
//    if(d->m_population) {
//        int nind = d->m_population->size();
//        int nvar = get(0)->getNumberOfVariables();

//        d->m_decisionVectors = arma::mat(nind,nvar);
//        for(int i=0; i<nind; i++) {
//            for(int j=0; j<nvar; j++) {
//                d->m_decisionVectors(i,j) = get(i)->getDecisionVariables()[j]->getValue();
//            }
//        }
//    }

//    return d->m_decisionVectors;
//}

//void GlobalSpace::setObjectiveVectors(arma::mat data)
//{
//    d->m_objectiveVectors = data;
//}

//void GlobalSpace::setDecisionVectors(arma::mat data)
//{
//    d->m_decisionVectors = data;
//}


//void GlobalSpace::setNumberOfVariables(int nvar)
//{
//    d->m_nvar = nvar;
//}

//void GlobalSpace::setPopulationSize(int nind)
//{
//    d->m_nind = nind;
//}

//void GlobalSpace::setNumberOfObjectives(int nobj)
//{
//    d->m_nobj = nobj;
//}

//void GlobalSpace::setVariableNames(QStringList names)
//{
//    d->m_dvnames.clear();
//    d->m_dvnames = names;
//}

//void GlobalSpace::setObjectiveNames(QStringList names)
//{
//    d->m_obnames.clear();
//   d->m_obnames = names;
//}

//void GlobalSpace::setLowerBounds(QList<qreal> lb)
//{
//    d->m_lowerBounds.clear();
//    d->m_lowerBounds = lb;
//}

//void GlobalSpace::setUpperBounds(QList<qreal> ub)
//{
//    d->m_upperBounds.clear();
//    d->m_upperBounds = ub;
//}

//void GlobalSpace::setVariableTypes(QList<MetalVariableType> vtype)
//{
//    d->m_dvarTypes.clear();
//    d->m_dvarTypes = vtype;
//}

//int GlobalSpace::numberOfVariables() const
//{
//    return d->m_nvar;
//}

//int GlobalSpace::populationSize() const
//{
//    return d->m_nind;
//}

//int GlobalSpace::numberOfObjectives() const
//{
//    return d->m_nobj;
//}

//QStringList GlobalSpace::variableNames() const
//{
//    return d->m_dvnames;
//}

//QStringList GlobalSpace::objectiveNames() const
//{
//    return d->m_obnames;
//}

//QList<qreal> GlobalSpace::lowerBounds() const
//{
//    return d->m_lowerBounds;
//}

//QList<qreal> GlobalSpace::upperBounds() const
//{
//    return d->m_upperBounds;
//}

//QList<MetalVariableType> GlobalSpace::variableTypes() const
//{
//    return d->m_dvarTypes;
//}

//SolutionType* GlobalSpace::solutionType() const
//{
//    return d->m_solutionType;
//}

//void GlobalSpace::setSolutionType(SolutionType* solType)
//{
//    d->m_solutionType = solType;
//}

//void GlobalSpace::clearGlobal()
//{
//    d->m_problem    = 0;
//    d->m_algorithm  = 0;
//    d->m_population = 0;
//    d->m_nvar = 0;
//    d->m_nind = 0;
//    d->m_nobj = 0;
//    d->m_solutionType = 0;
//    d->m_objectiveVectors      = arma::mat(1,1);
//    d->m_objectiveVectors(0,0) = -1;
//    d->m_decisionVectors       = arma::mat(1,1);
//    d->m_decisionVectors(0,0)  = -1;
//    d->m_operators.clear();
//    d->m_lowerBounds.clear();
//    d->m_upperBounds.clear();
//    d->m_dvarTypes.clear();
//    d->m_multipack.clear();
//}

//// Uni-bus functions
//void GlobalSpace::setMultiPackData(QString dname, QVariant data)
//{
//    QString mkey = dname.normalized(QString::NormalizationForm_D);
//    d->m_multipack.insert(mkey,data);
//}

//QVariant GlobalSpace::getMultiPackData(QString dname)
//{
//    QString mkey = dname.normalized(QString::NormalizationForm_D);
//    return d->m_multipack.value(mkey);
//}

////------------------

//void GlobalSpace::copy_elements(const GlobalSpace& a)
//{
//    d->m_problem    = a.problem();
//    d->m_algorithm  = a.algorithm();
//    d->m_operators  = a.operators();
//    d->m_population = a.population();
//    d->m_objectiveVectors = a.objectiveVectors();
//    d->m_decisionVectors  = a.decisionVectors();

//    d->m_nvar = a.numberOfVariables();
//    d->m_nind = a.populationSize();
//    d->m_nobj = a.numberOfObjectives();
//    d->m_dvnames = a.variableNames();
//    d->m_obnames = a.objectiveNames();
//    d->m_lowerBounds = a.lowerBounds();
//    d->m_upperBounds = a.upperBounds();
//    d->m_dvarTypes   = a.variableTypes();
//    d->m_solutionType = a.solutionType();
//}

//void GlobalSpace::base_assign(const GlobalSpace& a) { copy_elements(a); }

//GlobalSpace& GlobalSpace::operator=(const GlobalSpace& data)
//{
//    this->base_assign(data);
//    return *this;
//}

//} // namespace Designer
