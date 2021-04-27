/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef {% filter upper %}{{ ClassName }}{% endfilter %}PROBLEM_H
#define {% filter upper %}{{ ClassName }}{% endfilter %}PROBLEM_H
#include <tigon/tigon_global.h>
#include <tigon/Problems/IProblem.h>
#include <tigon/Representation/BoxConstraintsData.h>
#include <tigon/Representation/LinearConstraintsData.h>
#include <tigon/Representation/NonLinearConstraintsData.h>

// Qt Includes
#include <QString>
#include <QStringList>


namespace Tigon {
namespace Problems {

using namespace Tigon::Representation;
class LIGER_TIGON_EXPORT {{ ClassName }} : public IProblem
{
    Q_OBJECT
    Q_PROPERTY(QString     ProblemName          READ name)
    Q_PROPERTY(QString     ProblemDescription   READ description)
    Q_PROPERTY(QStringList PropertyNames        READ propertyNames)
    Q_PROPERTY(QStringList PropertyDescriptions READ propertyDescriptions)
    Q_PROPERTY(int         NumberOfVariables    READ TP_nVar               WRITE TP_setNVar)
    Q_PROPERTY(int         NumberOfObjectives   READ TP_nObj               WRITE TP_setNObj)

public:
    {{ ClassName }}();
    {{ ClassName }}(Tigon::Representation::IPSet *ipset);
    ~{{ ClassName }}();

    void evaluate();
    void configure();

    // Information about the node.
    QString     name();
    QString     description();
    QStringList propertyNames();
    QStringList propertyDescriptions();

    // Parameters
    int  TP_nVar();
    void TP_setNVar(int num);
    int  TP_nObj();
    void TP_setNObj(int num);

private:
    QString m_name;
    QString m_description;
    QStringList m_propertyNames;
    QStringList m_propertyDescriptions;
};

} // namespace Problems
} // namespace Tigon

#endif // {% filter upper %}{{ ClassName }}{% endfilter %}PROBLEM_H
