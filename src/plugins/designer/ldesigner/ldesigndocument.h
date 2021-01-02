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
#ifndef LDESIGNDOCUMENT_H
#define LDESIGNDOCUMENT_H
#include <designer/designerconstants.h>
#include <designer/designer_global.h>
#include <texteditor/plaintexteditor.h>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QXmlStreamWriter;
class QXmlStreamReader;
QT_END_NAMESPACE

namespace Designer {
class IPortLink;
class IProcessNode;
class LDesigner;
class LDesignDocumentPrivate;

class DESIGNER_EXPORT LDesignDocument : public TextEditor::PlainTextDocument
{
    Q_OBJECT

public:
    enum LigerDataLabel {
        ProcessNode,
        Link,
        NodeId,
        GUIProperties,
        XPos,
        YPos,
        Properties,
        FromNode,
        FromPort,
        ToNode,
        ToPort,
        Unknown
    };

    LDesignDocument(QObject *parent = 0);
    explicit LDesignDocument(LDesigner *designer, QObject *parent = 0);
    virtual ~LDesignDocument();

    // IDocument implementation.
    bool save(QString *errorString, const QString &fileName, bool autoSave);
    bool setContents(const QByteArray &contents);
    bool shouldAutoSave() const;
    bool isModified() const;
    bool isSaveAsAllowed() const;
    bool reload(QString *errorString, ReloadFlag flag, ChangeType type);
    // reload helper function
    bool reload(QString *errorString);
    bool open(QString *errorString, const QString &fileName,
              const QString &realFileName);

    LDesigner *designWindow() const;
    void syncXmlFromDesignWindow();
    QString designWindowContents() const;

    void setDesigner(LDesigner* designer);

    QString fileName() const;

    // Node handling ----------------------------
    QList<QString> nodeIds();
    QList<QString> fromNode();
    QList<qint32>  fromPort();
    QList<QString> toNode();
    QList<qint32>  toPort();
    QList<qreal>   nodesXPos();
    QList<qreal>   nodesYPos();

    void addProcessNode(IProcessNode* node);
    void removeProcessNode(IProcessNode* node);
    void removeProcessNodeAt(qint32 idx);
    void removeAllProcessNodes();
    void addPortLink(IPortLink* link);
    void removePortLink(IPortLink* link);
    void removeAllPortLinks();
    void removeAllProcessNodesAndLinks();
    QList<IPortLink*>    portLinks();
    QList<IProcessNode*> processNodes();    
    //-------------------------------------------

protected:
    QString parseDesignWindowContentsToXML();
    bool loadXMLDocumentContents();

signals:
    void titleChanged(QString title);
    // Internal
    void reloadRequested(QString *errorString, const QString &);

public slots:
    void setShouldAutoSave(bool sad = true);
    void updateIsModified();

private slots:
    void slotDesignWindowRemoved(LDesigner *w);

private:
    LigerDataLabel toLigerDataLabel(QStringRef str) const;
    QString processTypeStr(ProcessType type) const;
    LDesignDocumentPrivate *d;
};

} // namespace Designer

#endif // LDESIGNDOCUMENT_H
