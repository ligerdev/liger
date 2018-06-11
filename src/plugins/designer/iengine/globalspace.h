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
//#ifndef GLOBALSPACE_H
//#define GLOBALSPACE_H

//#include <QObject>

//#include <metal/metalconstants.h>
//#include <designer/designer_export.h>
//#include <designer/designerconstants.h>
//#include <arma/include/armadillo>

//class IAlgorithm;
//class IOperator;
//class IProblem;
//class Solution;
//class SolutionSet;
//class SolutionType;
//class IVariable;

//QT_BEGIN_NAMESPACE
//class QStringList;
//class QObject;
//class QVariant;
//class QString;
//QT_END_NAMESPACE

////namespace arma {
////template<class T> class Mat;
////typedef Mat<double> mat;
////}


//namespace Designer {
//class GlobalSpacePrivate;

//class DESIGNER_EXPORT GlobalSpace : public QObject
//{
//    Q_OBJECT

//public:
//    GlobalSpace();
//    GlobalSpace(const GlobalSpace& data);
//    virtual ~GlobalSpace();

//    Solution* get(int i) const;

//    virtual void setProblem(IProblem* prob);
//    virtual void setAlgorithm(IAlgorithm* algo);
//    virtual void setPopulation(SolutionSet* pop);
//    virtual void addOperator(qint32 nodeId, IOperator *op);
//    virtual void removeOperator(qint32 nodeId);
//    virtual bool operatorExists(qint32 nodeId);

//    virtual IProblem*    problem() const;
//    virtual IAlgorithm*  algorithm() const;
//    virtual IOperator*   getOperator(qint32 i) const;
//    virtual SolutionSet* population() const;
//    virtual arma::mat    objectiveVectors() const;
//    virtual arma::mat    decisionVectors() const;
//    virtual void setObjectiveVectors(arma::mat data);
//    virtual void setDecisionVectors(arma::mat data);
//    virtual QMap<qint32,IOperator*> operators() const;

//    void setNumberOfVariables(int nvar);
//    void setPopulationSize(int nind);
//    void setNumberOfObjectives(int nobj);
//    void setVariableNames(QStringList names);
//    void setObjectiveNames(QStringList names);
//    void setLowerBounds(QList<qreal> lb);
//    void setUpperBounds(QList<qreal> ub);
//    void setVariableTypes(QList<MetalVariableType> vtype);
//    int numberOfVariables() const;
//    int populationSize() const;
//    int numberOfObjectives() const;
//    QStringList variableNames() const;
//    QStringList objectiveNames() const;
//    QList<qreal> lowerBounds() const;
//    QList<qreal> upperBounds() const;
//    QList<MetalVariableType> variableTypes() const;

//    SolutionType* solutionType() const;
//    void setSolutionType(SolutionType* solType);

//    void clearGlobal();

//    // Uni-bus functions
//    void     setMultiPackData(QString dname, QVariant data);
//    QVariant getMultiPackData(QString dname);
//    //------------------

//    virtual GlobalSpace& operator=(const GlobalSpace& data);

//private:
//    void copy_elements(const GlobalSpace& a);
//    void base_assign(const GlobalSpace& a);

//    GlobalSpacePrivate *d;
//};

//} // namespace Designer

//Q_DECLARE_METATYPE(Designer::GlobalSpace)

//#endif // GLOBALSPACE_H
