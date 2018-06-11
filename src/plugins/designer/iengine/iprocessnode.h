/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef IPROCESSNODE_H
#define IPROCESSNODE_H

// Designer
#include <designer/designer_global.h>
#include <designer/designerconstants.h>

// Qt Includes
#include <QString>
#include <QVector>
#include <QGraphicsItem>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QSvgRenderer>
#include <QPointer>
#include <QObject>

namespace Designer {
class IPortLink;
class IPort;
class OutputPort;
class InputPort;
class NodeID;

/*!
 * \brief IProcessNode
 * \todo  Redesign this classs
 * \todo  Create IStartNode and IEndNode, which are derived classes of IProcessNode.
 * \note  Think about the following:
 * 1. how to allow copying of IProcessNode. No copying is allowed for now
 * 2. How to implement the copy and paste functions in designer for IProcessNode
 * 3. Better way to handle file I/O - maybe consider QSettings with customised XML parsing
 */
class DESIGNER_EXPORT IProcessNode : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    Q_PROPERTY(QString NodeId READ nodeIDStr WRITE setNodeID)
    Q_PROPERTY(qreal xpos READ x WRITE setX)
    Q_PROPERTY(qreal ypos READ y WRITE setY)

public:
    IProcessNode();
    virtual ~IProcessNode();
    virtual void initializeNode();
    /*!
     * \brief Update the process state. When the process state has changes,
     * process node can enable/disable certain functionalities.
     * By default, it emits the signal nodeStateChangedSignal
     */
    virtual void updateProcessState(ProcessState state);
    void* data() const;

    /*!
     * \brief Enable interactions between Process Nodes.
     */
    virtual void setupInteractions(IProcessNode*) {return;}

    virtual void show() {return;}

    /// I/O to data file
    void generateLigerDocumentContent(QXmlStreamWriter& xmlWriter);
    void loadLigerDocumentContent(QXmlStreamReader& xmlReader);

    /// Node information
    virtual int numberOfInputs()  { return m_numberOfInputs;  }
    virtual int numberOfOutputs() { return m_numberOfOutputs; }
    void setNumberOfOutputs(const qint32 &numberOfOutputs);
    void setNumberOfInputs(const qint32 &numberOfInputs);
    virtual int numberOfConnectedInputs();
    virtual int numberOfConnectedOutputs();

    QList<IPortLink*> portLinks();
    InputPort*  inputPortAt(qint32 idx);
    OutputPort* outputPortAt(qint32 idx);

    void     setNodeID(QString id);
    QString  nodeIDStr() const;

    enum { Type = UserType + 15 };
    int type() const { return Type; }

    /// Termination
    /// \todo move to IEndNode
    virtual bool isUseIteration() const {return false;}
    qint64 maxIterations() const { return m_maxIterations; }
    void setMaxIterations(qint64 maxIterations) { m_maxIterations = maxIterations; }

    virtual bool isUseBudget() const {return false;}
    qint64 budget() const { return m_budget; }
    void setBudget(qint64 budget) { m_budget = budget; }

    /// Auto Pause
    /// \todo move to IEndNode
    virtual bool isPauseByIteration() const {return false;}
    virtual bool isPauseByBudget() const {return false;}
    virtual QVector<qint64> iterPausePointVec() const {return QVector<qint64>();}
    virtual QVector<qint64> budgetPausePointVec() const {return QVector<qint64>();}

    /// Log
    /// \todo move to IEndNode
    virtual bool logEvaluations() const {return false;}
    virtual bool logPopulations() const {return false;}
    virtual bool keepPopulationsHistory() const {return false;}
    virtual QString populationsLogPath() const {return QString();}
    virtual QString evaluationsLogPath() const {return QString();}

    /// Process information
    ProcessType  processType()       const { return m_procType;  }
    QString      processTypeString() const;

    ProcessState state() const;
    void setState(ProcessState state);

signals:
    void nodeStateChangedSignal(ProcessState state);
    void nodeErrorSignal(QString error_message);
    void nodePropertiesChangedSignal();

public slots:
    /*!
     * \brief Set data when received from engine
     * \param Data data from engine
     * \note  This is the function that should be used for update data pointer
     * held by the process node.
     */
    virtual void dataReceivedFromEngine(void* data);
    virtual void dataUpdatedByEngine() {}

protected:
    /*!
     * \brief Initialised data for the process data
     */
    void setData(void* data);

    virtual void readDataProperties(QXmlStreamReader& xmlReader);
    virtual void writeDataProperties(QXmlStreamWriter& xmlWriter);

    void addInputPort(IProcessNode* parent);
    void addOutputPort(IProcessNode *parent);
    void addInputPort(IProcessNode* parent, qint32 id);
    void addOutputPort(IProcessNode *parent, qint32 id);
    void removeInputPort(InputPort* port);
    void removeOutputPort(OutputPort* port);

    virtual void setText(const QString &text);
    virtual QString text() const;
    virtual void setTextColor(const QColor &color);
    virtual QColor textColor() const;
    virtual void setOutlineColor(const QColor &color);
    virtual QColor outlineColor() const;
    virtual void setBackgroundColor(const QColor &color);
    virtual QColor backgroundColor() const;
    virtual QPolygonF outlinePolygon();
    virtual QRectF boundingRect() const;
    QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget);
    void setSvg(QString svgFile);
    void setPixmap(QString pixmapFile);
    void qt_graphicsItem_highlightSelected(QGraphicsItem *item, QPainter *painter,
                                           const QStyleOptionGraphicsItem *option);

    virtual bool isConnected();

private slots:
    void repaintNode();

protected:
    virtual QRectF outlineRect() const;
    virtual int roundness(double size) const;
    virtual QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);

    QString      m_text;
    QColor       m_textColor;
    QColor       m_backgroundColor;
    QColor       m_outlineColor;

    ProcessType        m_procType;
    QList<InputPort*>  m_inputPorts;
    QList<OutputPort*> m_outputPorts;

private:
    // IProcessNode must be used as pointers not values
    Q_DISABLE_COPY(IProcessNode)

    qint32        m_numberOfOutputs;
    qint32        m_numberOfInputs;
    qint32        m_numberOfConnectedOutputs;
    qint32        m_numberOfConnectedInputs;
    /*!
     * \brief m_data pointer to any data/class type stored in iprocessnode.
     * setData method will cast any class pointer type to void* and store it
     * in iprocess node.
     * To use the data, access the pointer via void* data() method and cast it
     * to a proper pointer type.
     * \note It requires the developer of the plugins to konw the data type they
     * will use in their implementation.
     * \todo Find a more generic representation of any data type.
     */
    void*              m_data;
    bool               m_ownData;
    NodeID*            m_nodeID;
    qint64             m_maxIterations;
    qint64             m_budget;
    QPixmap*           m_pixmap;
    QSvgRenderer*      m_renderer;
    ProcessState       m_state;
};

} // namespace Designer

//Q_DECLARE_METATYPE(Designer::IProcessNode)

#endif // IPROCESSNODE_H
