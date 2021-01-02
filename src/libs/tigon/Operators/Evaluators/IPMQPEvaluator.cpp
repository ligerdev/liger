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
#include <tigon/Operators/Evaluators/IPMQPEvaluator.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Constraints/LinearConstraintsData.h>
#include <tigon/Representation/Mappings/QuadraticObjectiveData.h>

namespace Tigon {
namespace Operators {

IPMQPEvaluator::IPMQPEvaluator()
{
    TP_defineOutputTags(QStringList(Tigon::TIPMQP));
}

IPMQPEvaluator::IPMQPEvaluator(IPSet* ipset)
    : IEvaluator(ipset)
{
    TP_defineOutputTags(QStringList(Tigon::TIPMQP));
}

IPMQPEvaluator::~IPMQPEvaluator()
{

}

/*
 * Function evaluation and residuals caluation
 * fval = 0.5 x'Qx+c'x,
 * stored as output of the OptimizationMapping.
 * Rd, Ra, Rc and Rsz are residuals,
 * stored as the output of the generic mapping.
 * Res is the relative residual, scaled by bd.
 * Res is the main indicator of how far it is from
 * the current solution to the optimal solution.
 */
void IPMQPEvaluator::evaluateNode()
{
    updateInputs();
    // Primal info
    QVector<IMappingSPtr> optMappings = inputSet(0)->all();
    // Dual and residual info
    QVector<IMappingSPtr> genMappings = inputSet(1)->all();

    for(int k = 0; k < optMappings.size(); k++) {
        qreal residual = genMappings[k]
                ->outputVec().last()
                ->value().toReal();
        if(residual < optMappings[k]->accuracy()) {
            continue;
        }
        TCVectorReal x(decisionVecSize(),1);

        TMatrixReal  Q = optMappings[k]
                ->quadraticObjectiveData()
                ->quadraticCoeffQMatrix();
        TCVectorReal f = optMappings[k]
                ->quadraticObjectiveData()
                ->linearCoeffCVector();

        TMatrixReal  A = optMappings[k]->linearEqConstraints()
                         ->linearConstraintsAMatrix();
        TCVectorReal b = optMappings[k]->linearEqConstraints()
                         ->linearConstraintsBVector();
        if(A.rows() == 0) {
            // If the local model has no global lineq, use the
            // the ipset one
            // TODO - Remove constraints in ipset
            A = linearEqConstraints()->linearConstraintsAMatrix();
            b = linearEqConstraints()->linearConstraintsBVector();
        }

        TMatrixReal  H = optMappings[k]->linearIneqConstraints()
                         ->linearConstraintsAMatrix();
        TCVectorReal d = optMappings[k]->linearIneqConstraints()
                         ->linearConstraintsBVector();
        if(H.rows() == 0) {
            // If the local model has no linIneq, use the
            // the ipset one
            // TODO - Remove constraints in ipset
            H = linearIneqConstraints()
                ->linearConstraintsAMatrix();
            d = linearIneqConstraints()
                ->linearConstraintsBVector();
        }

        TCVectorReal y(A.rows());
        TCVectorReal z(H.rows());
        TCVectorReal s(H.rows());

        TCVectorReal Rd(decisionVecSize());
        TCVectorReal Ra(A.rows(),1);
        TCVectorReal Rc(H.rows(),1);
        TCVectorReal Rsz(H.rows(),1);
        TCVectorReal Res(decisionVecSize()
                         +A.rows()
                         +2*H.rows());

        for(int i = 0; i < decisionVecSize(); i++) {
            x(i) = optMappings[k]
                    ->inputVec()[i]
                    ->value().toReal();
        }
        for(int i = 0; i < A.rows(); i++) {
            y(i) = genMappings[k]
                    ->inputVec()[i]
                    ->value().toReal();
        }
        for(int i = 0; i < H.rows(); i++) {
            z(i) = genMappings[k]
                    ->inputVec()[A.rows()+i]
                    ->value().toReal();
            s(i) = genMappings[k]
                    ->inputVec()[A.rows()+H.rows()+i]
                    ->value().toReal();
        }

        // Func value
        TCVectorReal fval = 0.5*x.transpose()*Q*x + f.transpose()*x;
        optMappings[k]->setOutputVar(0,QVariant::fromValue(fval[0]));

        // Residuals
        double bd = 1 + qMax(b.norm(),d.norm());
        Rd  = Q*x - A.transpose()*y + H.transpose()*z + f;
        Ra  = A*x-b;
        Rc  = H*x-d+s;
        Rsz = (s.array()*z.array()).matrix();
        Res <<  Rd,
                Ra,
                Rc,
                Rsz;

        for(int i = 0; i < decisionVecSize(); i++) {
            genMappings[k]
                    ->setOutputVar(i,QVariant::fromValue(Rd(i)));
        }
        for(int i = 0; i < Ra.rows(); i++) {
            genMappings[k]
                    ->setOutputVar(i+decisionVecSize(),
                                   QVariant::fromValue(Ra(i)));
        }
        for(int i = 0; i < Rc.rows(); i++) {
            genMappings[k]
                    ->setOutputVar(i+decisionVecSize()+Ra.rows(),
                                   QVariant::fromValue(Rc(i)));
            genMappings[k]
                    ->setOutputVar(i+decisionVecSize()+Ra.rows()+Rc.rows(),
                                   QVariant::fromValue(Rsz(i)));
        }
        genMappings[k]
                ->setOutputVar(decisionVecSize()
                               +Ra.rows()+2*Rc.rows(),
                               QVariant::fromValue(
                                   Res.norm()/bd));
    }
}

QString IPMQPEvaluator::name()
{
    return QString("IPMQPEvaluator");
}

QString IPMQPEvaluator::description()
{
    return QString("Interior Point Method "
                   "for Qudratic Programming - "
                   "Evaluation Operator");
}

QStringList IPMQPEvaluator::propertyNames()
{
    return QStringList();
}


QStringList IPMQPEvaluator::propertyDescriptions()
{
    return QStringList();
}

} // namespace Operators
} // namespace Tigon
