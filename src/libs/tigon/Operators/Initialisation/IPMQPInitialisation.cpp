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
#include <tigon/Operators/Initialisation/IPMQPInitialisation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Properties/ElementProperties.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <tigon/Representation/Mappings/GenericMapping.h>

namespace Tigon {
namespace Operators {

IPMQPInitialisation::IPMQPInitialisation()
{
    m_fct = 0;
    addOutputTag(Tigon::TIPMQP);
    addOutputTag(Tigon::TForDirection);
}

IPMQPInitialisation::IPMQPInitialisation(Tigon::Representation::IPSet* ipset)
    : IInitialisation(ipset)
{
    m_fct = 0;
    addOutputTag(Tigon::TIPMQP);
    addOutputTag(Tigon::TForDirection);
}

IPMQPInitialisation::~IPMQPInitialisation()
{

}

void IPMQPInitialisation::evaluateNode()
{
    // TODO - simplify the following code after get the new update
    // TODO - mainGenericSet and mainOptimizationSet should be already there
    if(currentIteration() == 0) {
        m_fct = ElementPropertiesFactory::instance();

        // TODO - update to accept tagged sets
        appendOutputSet(set(Tigon::TMainOptimizationSet));

        QVector<IMappingSPtr> optMappings = outputSet(0)->all();
        QVector<IMappingSPtr> genMappings;

        // TODO - Update to the number of all imappings in all subset
        int n = decisionVecSize();

        for(int k = 0; k < optMappings.size(); k++) {

            // Scaling constant
            TCVectorReal b = optMappings[k]
                    ->linearEqConstraints()
                    ->linearConstraintsBVector();
            TCVectorReal d = optMappings[k]
                    ->linearIneqConstraints()
                    ->linearConstraintsBVector();
            // TODO - remove constraints from ipset
            if(b.rows() == 0) {
                b = linearEqConstraints()
                        ->linearConstraintsBVector();
            }
            if(d.rows() == 0) {
                d = linearIneqConstraints()
                        ->linearConstraintsBVector();
            }
            double bd = 1 + qMax(b.norm(),d.norm());

            int m_a = b.rows();
            int m_c = d.rows();

            // Generate dual variables and slack variables
            QVector<ElementProperties> ivp;
            for(int i = 0; i < m_a; i++) {
                ivp.append(m_fct->createElementProperties(i,
                                                          "y_"+QString::number(i),
                                                          "Dual variables y",
                                                          RealType));
            }
            for(int i = m_a; i < m_a+m_c; i++) {
                ivp.append(m_fct->createElementProperties(i,
                                                          "z_"+QString::number(i),
                                                          "Dual variables z",
                                                          RealType));
            }
            for(int i = m_a+m_c; i < m_a+m_c*2; i++) {
                ivp.append(m_fct->createElementProperties(i,
                                                          "s_"+QString::number(i),
                                                          "slcak variables s",
                                                          RealType));
            }
            // Residuals as generic output
            QVector<ElementProperties> ovp;
            for(int i = 0; i < n; i++) {
                ovp.append(m_fct->createElementProperties(i,
                                                          "Rd_"+QString::number(i),
                                                          "dual residuals Rd",
                                                          RealType));
            }
            for(int i = n; i < n+m_a; i++) {
                ovp.append(m_fct->createElementProperties(i,
                                                          "Ra_"+QString::number(i),
                                                          "Primal residuals Ra",
                                                          RealType));
            }
            for(int i = n+m_a; i < n+m_a+m_c; i++) {
                ovp.append(m_fct->createElementProperties(i,
                                                          "Rc_"+QString::number(i),
                                                          "Primal residuals Rc",
                                                          RealType));
            }
            for(int i = n+m_a+m_c; i < n+m_a+m_c*2; i++) {
                ovp.append(m_fct->createElementProperties(i,
                                                          "Rsz_"+QString::number(i),
                                                          "Duality gap Rsz",
                                                          RealType));
            }
            ovp.append(m_fct->createElementProperties(n+m_a+m_c*2,
                                                      "Res",
                                                      "Relative residual",
                                                      RealType));
            // Define generic set input and output
            genMappings.append(IMappingSPtr(new IMapping(ivp,ovp)));

            // Set initial values for x
            for(int i = 0; i < decisionVecSize(); i++) {
                optMappings[k]
                        ->setInputVar(i,QVariant::fromValue(bd));
            }

            // Initial values for dual (y,z) and slack (s)
            for(int i = 0; i < genMappings[k]->inputVec().size(); i++) {
                if(i < m_a) {
                    genMappings[k]
                            ->setInputVar(i,QVariant::fromValue(0));
                } else {
                    genMappings[k]
                            ->setInputVar(i,QVariant::fromValue(bd));
                }
            }
            // Initial value (some large number) for residual
            genMappings[k]
                    ->setOutputVar(n+m_a+m_c*2,
                                   QVariant::fromValue(bd));
            appendToGenericSet(genMappings[k]);
        }

        // TODO - create its own subsets, not main generic set
        appendOutputSet(set(Tigon::TMainGenericSet));
    }
}

QString IPMQPInitialisation::name()
{
    return QString("IPMQPInitialisation");
}

QString IPMQPInitialisation::description()
{
    return QString("Interior Point Method "
                   "for Qudratic Programming - "
                   "Initialisation Operator");
}

QStringList IPMQPInitialisation::propertyNames()
{
    return QStringList();
}

QStringList IPMQPInitialisation::propertyDescriptions()
{
    return QStringList();
}

} // namespace Operators
} // namespace Tigon
