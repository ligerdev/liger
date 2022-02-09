/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
**
****************************************************************************/
#include <tigon/Operators/Directions/IPMQPDirection.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Constraints/LinearConstraintsData.h>
#include <tigon/Representation/Mappings/QuadraticObjectiveData.h>

//Qt includes
#include <QtMath>

namespace Tigon {
namespace Operators {

IPMQPDirection::IPMQPDirection()
{
    TP_defineOutputTags(QStringList(Tigon::TIPMQP));
}

IPMQPDirection::IPMQPDirection(IPSet* ipset)
    : IDirection(ipset)
{
    TP_defineOutputTags(QStringList(Tigon::TIPMQP));
}


IPMQPDirection::~IPMQPDirection()
{

}

void IPMQPDirection::evaluateNode()
{
    /* Newton directions with predictor-corrector */
    if(outputSets().size() == 0) {
        // if no sets there; skip
        return;
    }
    QVector<IMappingSPtr> optMappings = outputSet(0)->all();
    QVector<IMappingSPtr> genMappings = outputSet(1)->all();

    TCVectorReal x(decisionVecSize());
    for(int k = 0; k < optMappings.size(); k++) {
        qreal residual = genMappings[k]
                ->outputVec().last()
                ->value().toReal();
        if(residual < optMappings[k]->accuracy()) {
            continue;
        }
        // Extract data - Rd, Ra, Rc, Rsz, x, y, z, s
        TMatrixReal Q = optMappings[k]
                ->quadraticObjectiveData()
                ->quadraticCoeffQMatrix();

        TMatrixReal A = optMappings[k]->
                linearEqConstraints()
                ->linearConstraintsAMatrix();
        if(A.rows() == 0) {
            // TODO - remove constraints in ipset
            A = linearEqConstraints()
                    ->linearConstraintsAMatrix();
        }
        TMatrixReal H = optMappings[k]->
                linearIneqConstraints()
                ->linearConstraintsAMatrix();
        if(H.rows() == 0) {
            // TODO - remove constraints in ipset
            H = linearIneqConstraints()
                    ->linearConstraintsAMatrix();
        }
        TCVectorReal y(A.rows());
        TCVectorReal z(H.rows());
        TCVectorReal s(H.rows());

        TCVectorReal Rd(A.cols());
        TCVectorReal Ra(A.rows());
        TCVectorReal Rc(H.rows());
        TCVectorReal Rsz(H.rows());

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
        for(int i = 0; i < A.cols(); i++) {
            Rd(i) = genMappings[k]
                    ->outputVec()[i]->
                    value().toReal();
        }
        for(int i = 0; i < A.rows(); i++) {
            Ra(i) = genMappings[k]
                    ->outputVec()[A.cols()+i]
                    ->value().toReal();
        }
        for(int i = 0; i < H.rows(); i++) {
            Rc(i) = genMappings[k]
                    ->outputVec()[A.cols()+A.rows()+i]
                    ->value().toReal();
            Rsz(i) = genMappings[k]
                    ->outputVec()[A.cols()+A.rows()+H.rows()+i]
                    ->value().toReal();
        }

        qreal mu = Rsz.sum()/H.rows();

        TMatrixReal  D_2(A.rows(),A.rows());
        TCVectorReal rhs(Rd.rows()+Ra.rows());
        TMatrixReal  B(A.cols()+A.rows(),
                       A.cols()+A.rows());

        // Affine-scaling step
        D_2 = z.cwiseQuotient(s).asDiagonal();
        rhs << -Rd-H.transpose()*D_2*Rc+H.transpose()*Rsz.cwiseQuotient(s),
                -Ra;
        B   << Q + H.transpose()*D_2*H, -A.transpose(),
                A,                       TMatrixReal::Zero(A.rows(),A.rows());

        // factorise B
        Eigen::HouseholderQR<TMatrixReal> factor = B.householderQr();
        rhs = factor.solve(rhs);    // in-place copy; save memory and time
        TCVectorReal dx = rhs.head(A.cols());
        TCVectorReal dy = rhs.tail(A.rows());
        TCVectorReal dz = D_2*(H*dx+Rc)-Rsz.cwiseQuotient(s);
        TCVectorReal ds = -(Rsz+s.cwiseProduct(dz)).cwiseQuotient(z);

        double alphaz;
        double alphas;
        getStepSizes(alphaz,alphas,z,s,dz,ds);

        // mu and sigma
        qreal mu_aff = (s+alphas*ds).dot(z+alphaz*dz)/H.rows();
        qreal sigma  = qPow(mu_aff/mu,3.0); // TODO - Use tigon maths

        // Corrector + centering step
        Rsz = Rsz+ds.cwiseProduct(dz)-sigma*mu*TCVectorReal::Ones(H.rows());
        rhs <<  -Rd-H.transpose()*D_2*Rc+H.transpose()*(Rsz.cwiseQuotient(s)),
                -Ra;
        rhs = factor.solve(rhs);
        dx  = rhs.head(A.cols());
        dy  = rhs.tail(A.rows());
        dz  = D_2*(H*dx+Rc)-Rsz.cwiseQuotient(s);
        ds  = -(Rsz+s.cwiseProduct(dz)).cwiseQuotient(z);
        getStepSizes(alphaz,alphas,z,s,dz,ds);

        // Update
        qreal alpha_max = max(alphaz,alphas);
        x = x + alpha_max*dx;
        y = y + alpha_max*dy;
        z = z + alphaz*dz;
        s = s + alphas*ds;

        // Pass data to ipset
        // Set values for x
        for(int i = 0; i < decisionVecSize(); i++) {
            optMappings[k]
                    ->setInputVar(i,QVariant::fromValue(x(i)));

        }
        // Set values for y, z, s
        for(int i = 0; i < A.rows(); i++) {
            genMappings[k]
                    ->setInputVar(i,QVariant::fromValue(y(i)));
        }
        for(int i = 0; i < H.rows(); i++) {
            genMappings[k]
                    ->setInputVar(i+A.rows(),
                                  QVariant::fromValue(z(i)));
            genMappings[k]
                    ->setInputVar(i+A.rows()+H.rows(),
                                  QVariant::fromValue(s(i)));
        }
    }
}

QString IPMQPDirection::name()
{
    return QString("IPMQPDirection");
}

QString IPMQPDirection::description()
{
    return QString("Interior Point Method"
                   "for Qudratic Programming - "
                   "Newton Direction with"
                   "Predictor-Corrector");
}

QStringList IPMQPDirection::propertyNames()
{
    return QStringList();
}

QStringList IPMQPDirection::propertyDescriptions()
{
    return QStringList();
}

void IPMQPDirection::getStepSizes(qreal& alphaz, double &alphas,
                                  TCVectorReal z, TCVectorReal s,
                                  TCVectorReal dz, TCVectorReal ds)
{
    alphaz = (dz.cwiseQuotient(z)).minCoeff();
    alphas = (ds.cwiseQuotient(s)).minCoeff();
    alphaz = -1.0/min(alphaz,-1.0);
    alphas = -1.0/min(alphas,-1.0);
    alphaz = min(0.9995*alphaz, 1.0);
    alphas = min(0.9995*alphas, 1.0);
}

} // namespace Operators
} // namespace Tigon

