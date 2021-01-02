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
#ifndef MASTERENDNODE_H
#define MASTERENDNODE_H

#include <qtigon/qtigon_global.h>
#include <designer/iengine/iprocessnode.h>

#include <QObject>
class String;
class QStringList;

class EndNodeDialog;

namespace QTigon {

class QTIGON_EXPORT MasterEndNode : public Designer::IProcessNode
{
    Q_OBJECT

    Q_PROPERTY(bool isUseBudget READ isUseBudget WRITE setUseBudget)
    Q_PROPERTY(bool isUseIteration READ isUseIteration WRITE setUseIteration)

    Q_PROPERTY(QString iterPausePoints READ iterPausePoints WRITE setIterPausePoints)
    Q_PROPERTY(QString budgetPausePoints READ budgetPausePoints WRITE setBudgetPausePoints)

    Q_PROPERTY(QString iterPausePointType READ iterPausePointType WRITE setIterPausePointType)
    Q_PROPERTY(QString budgetPausePointType READ budgetPausePointType WRITE setBudgetPausePointType)
    Q_PROPERTY(bool isPauseByIteration READ isPauseByIteration WRITE setPauseByIteration)
    Q_PROPERTY(bool isPauseByBudget READ isPauseByBudget WRITE setPauseByBudget)

    Q_PROPERTY(bool logEvaluations READ logEvaluations WRITE setLogEvaluations)
    Q_PROPERTY(bool logPopulations READ logPopulations WRITE setLogPopulations)
    Q_PROPERTY(bool keepPopulationsHistory READ keepPopulationsHistory WRITE setkeepPopulationsHistory)
    Q_PROPERTY(QString populationsLogPath READ populationsLogPath WRITE setPopulationsLogPath)
    Q_PROPERTY(QString evaluationsLogPath READ evaluationsLogPath WRITE setEvaluationsLogPath)

public:
    MasterEndNode();
    ~MasterEndNode();
    void updateProcessState(Designer::ProcessState state);

    /// Termination
    bool isUseIteration() const;
    void setUseIteration(bool isUseIteration);

    bool isUseBudget() const;
    void setUseBudget(bool isUseBudget);

    /// Auto Pause \note only available through Liger GUI
    bool isPauseByIteration() const;
    QString iterPausePoints() const;
    QVector<qint64> iterPausePointVec() const;
    void setIterPausePoints(const QString& points);

    bool isPauseByBudget() const;
    QString budgetPausePoints() const;
    QVector<qint64> budgetPausePointVec() const;
    void setBudgetPausePoints(const QString& points);

    QString iterPausePointType() const;
    void setIterPausePointType(const QString& points);
    void setPauseByIteration(bool pauseByIter);

    QString budgetPausePointType() const;
    void setBudgetPausePointType(const QString& points);
    void setPauseByBudget(bool pauseByBudget);

    bool logEvaluations() const;
    void setLogEvaluations(bool logEvaluations);

    bool logPopulations() const;
    void setLogPopulations(bool logPopulations);

    bool keepPopulationsHistory() const;
    void setkeepPopulationsHistory(bool keepHistory);

    QString populationsLogPath() const;
    void setPopulationsLogPath(const QString &populationsLogPath);
    QString evaluationsLogPath() const;
    void setEvaluationsLogPath(const QString &evaluationsLogPath);

public slots:
    void acceptChangesSlot();

protected:
    void readDataProperties(QXmlStreamReader &xmlReader);
    void writeDataProperties(QXmlStreamWriter &xmlWriter);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    EndNodeDialog* m_dialog;
    bool           m_useIter;
    bool           m_useBudget;
    bool           m_pauseByIter;
    bool           m_pauseByBudget;
    bool           m_logPopulation;
    QString        m_logPath;
};

}  // namespace QTigon

#endif // MASTERENDNODE_H
