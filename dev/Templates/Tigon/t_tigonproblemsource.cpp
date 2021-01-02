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
#include <tigon/Problems/DevProblems/DevProblem.h>
#include <tigon/Representation/IElement.h>
#include <tigon/Representation/IMapping.h>
#include <tigon/tigonexception.h>


namespace Tigon {
namespace Problems {

DevProblem::DevProblem()
{
    setProblemName(QString("DevProblem"));
    QVector<Tigon::ElementType> dvecType;
    QVector<Tigon::ElementType> ovecType;
    for(int i=0; i<7; i++) {
        dvecType.append(Tigon::RealType);
    }
    for(int i=0; i<3; i++) {
        ovecType.append(Tigon::RealType);
    }
    setDVecType(dvecType);
    setOVecType(ovecType);
}

DevProblem::DevProblem(Tigon::Representation::IPSet *ipset)
    : IProblem(ipset)
{
    setProblemName(QString("DevProblem"));
    QVector<Tigon::ElementType> dvecType;
    QVector<Tigon::ElementType> ovecType;
    for(int i=0; i<7; i++) {
        dvecType.append(Tigon::RealType);
    }
    for(int i=0; i<3; i++) {
        ovecType.append(Tigon::RealType);
    }
    setDVecType(dvecType);
    setOVecType(ovecType);
}

DevProblem::~DevProblem() {

}

void DevProblem::evaluate() {
    syncEvaluation();

    // Enter code logic here...
}

void DevProblem::configure()
{
    syncConfiguration();
    setProblemName(name());
    QVector<Tigon::ElementType> dvecType;
    QVector<Tigon::ElementType> ovecType;
    dvecType.append(Tigon::RealType);
    ovecType.append(Tigon::RealType);
    setDVecType(m_nvar, Tigon::RealType);
    setOVecType(m_nobj, Tigon::RealType);
}

QString DevProblem::name()
{
    return m_name;
}

QString DevProblem::description()
{
    return m_description;
}

QStringList DevProblem::propertyNames()
{
    return m_propertyNames;
}

QStringList DevProblem::propertyDescriptions()
{
    return m_propertyDescriptions;
}

} // namespace Problems
} // namespace Tigon
