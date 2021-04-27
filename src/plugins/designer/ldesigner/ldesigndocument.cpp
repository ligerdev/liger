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
#include <designer/ldesigner/ldesigner.h>
#include <designer/ldesigner/ldesigndocument.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/iprocessnodefactory.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>

#include <extensionsystem/pluginmanager.h>
#include <coreplugin/progressmanager/progressmanager.h>
#include <utils/fileutils.h>
#include <utils/reloadpromptutils.h>

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFutureInterface>
#include <QTextStream>
#include <QTextCodec>
#include <QApplication>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QString>
#include <QStringList>
#include <QMetaProperty>
#include <QMessageBox>

#include <QDebug>

namespace Designer {

class LDesignDocumentPrivate
{
public:
    explicit LDesignDocumentPrivate(LDesignDocument *q);
    ~LDesignDocumentPrivate();

    QString m_fileName;
    bool m_isModified;
    bool m_shouldAutoSave;

    QPointer<LDesigner> m_designWindow;

    QList<QString> m_nodeClassNameList;
    QList<QString> m_nodeIdList;
    QList<qreal>   m_nodeXposList;
    QList<qreal>   m_nodeYposList;
    QList<QString> m_fromNodeList;
    QList<qint32>  m_fromPortList;
    QList<QString> m_toNodeList;
    QList<qint32>  m_toPortList;

    QList<IPortLink*>    m_portLinks;
    QList<IProcessNode*> m_processNodes;
};

LDesignDocumentPrivate::LDesignDocumentPrivate(LDesignDocument *q) :
    m_isModified(false),
    m_shouldAutoSave(false),
    m_designWindow(0)
{
    Q_UNUSED(q)
}

LDesignDocumentPrivate::~LDesignDocumentPrivate()
{

}

LDesignDocument::LDesignDocument(QObject *parent)
    : d(new LDesignDocumentPrivate(this))
{
    setParent(parent);
    setId(Core::Id(Designer::Constants::K_DESIGNER_XML_EDITOR_ID));
    setCodec(QTextCodec::codecForName("UTF-8"));
    setMimeType(Designer::Constants::DESIGNER_MIMETYPE);

    //connect(d->m_designWindow, SIGNAL(changed()),SLOT(updateIsModified()));
}

LDesignDocument::LDesignDocument(LDesigner *designer, QObject *parent) :
    d(new LDesignDocumentPrivate(this))
{
    setParent(parent);
    setId(Core::Id(Designer::Constants::K_DESIGNER_XML_EDITOR_ID));
    setCodec(QTextCodec::codecForName("UTF-8"));
    setMimeType(Designer::Constants::DESIGNER_MIMETYPE);
    d->m_designWindow = designer;

    connect(d->m_designWindow, SIGNAL(contentsChanged()),
            SLOT(updateIsModified()));
}

LDesignDocument::~LDesignDocument()
{
    delete d;
}

bool LDesignDocument::save(QString *errorString, const QString &fileName, bool autoSave)
{
    QString fName = filePath();
    if (!fileName.isEmpty()) {
        fName = fileName;
    }

    if(fName.isEmpty()) {
        QString *errmsg = new QString("FileName is empty.");
        errorString = errmsg;
        return false;
    }

    // Must have an instance of a designeditor by this point.
    if(!d->m_designWindow) {
        QString *errmsg = new QString("DesignEditor not found.");
        errorString = errmsg;
        return false;
    }

    const QFileInfo fi(fName);
    const QString oldFilePath = d->m_designWindow->document()->filePath();

    d->m_shouldAutoSave = false;
    if(!autoSave) {
        d->m_designWindow->document()->setFilePath(fi.absoluteFilePath());
    }

    // Parse the XML file and load it to this documents contents.
    QFile xmlfile(fName);
    if(!xmlfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        d->m_designWindow->document()->setFilePath(oldFilePath);
        QString errmsg = QString("Could not open file: %1 for writing.").arg(fName);
        errorString = &errmsg;
        return false;
    }
    QTextStream xmlStream(&xmlfile);
    QString contents = parseDesignWindowContentsToXML();
    xmlStream << contents;
    xmlfile.flush();
    xmlfile.close();
    // Set the document contents.
    plainText().clear();
    QByteArray contentsByteArray = contents.toUtf8();
    setContents(contentsByteArray);

    // inform about the new filename
    document()->setModified(false);
    setFilePath(QDir::cleanPath(fi.absoluteFilePath()));
    d->m_fileName = QDir::cleanPath(fi.absoluteFilePath());
    emit filePathChanged(oldFilePath,d->m_fileName);
    emit titleChanged(fi.fileName());
    emit contentsChanged();
    emit changed();
    return true;
}

bool LDesignDocument::setContents(const QByteArray &contents)
{    
    bool success = false;
    if (Designer::Constants::Internal::debug)
        qDebug() << Q_FUNC_INFO << contents.size();

    document()->clear();
    document()->setPlainText(QString::fromUtf8(contents));

    if(!d->m_designWindow) {
        success = false;
        return success;
    }

    if (contents.isEmpty()) {
        success = false;
        return success;
    }

    success = loadXMLDocumentContents();

    syncXmlFromDesignWindow();
    setShouldAutoSave(false);
    return success;
}

bool LDesignDocument::shouldAutoSave() const
{
    return d->m_shouldAutoSave;
}

bool LDesignDocument::isModified() const
{
    d->m_isModified = document()->isModified();
    return d->m_isModified;
}

bool LDesignDocument::isSaveAsAllowed() const
{
    return true;
}

bool LDesignDocument::reload(QString *errorString, ReloadFlag flag, ChangeType type)
{
    if (flag == FlagIgnore)
        return true;
    if (type == TypePermissions) {
        emit changed();
    } else {
        bool success = reload(errorString);
        return success;
    }
    return true;
}

bool LDesignDocument::reload(QString *errorString)
{
    emit aboutToReload();
    bool success = open(errorString, d->m_fileName, d->m_fileName);

    if (success)
        emit reloadFinished(true);
    return success;
}

bool LDesignDocument::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
    bool readResult = false;
    // LDesignDocument derives from PlainTextDocument. So, we can use the super class
    // open function to read in the XML file into memory (it is a text file after all)
    // and then parse the tree from memory. Don't have to do it this way, but it saves
    // time and reduces the chance for error.
    if(!PlainTextDocument::open(errorString, fileName, realFileName)) {
        return readResult;
    } else {
        readResult = true;
    }

    readResult = loadXMLDocumentContents();
    if(!readResult) {
        return readResult;
    }

    document()->setModified(false);

    return readResult;
}

QString LDesignDocument::fileName() const
{
    return d->m_fileName;
}

void LDesignDocument::setShouldAutoSave(bool sad)
{
    d->m_shouldAutoSave = sad;
}

void LDesignDocument::updateIsModified()
{
    //    bool value = m_designWindow && m_designWindow->isDirty();
    //    if (value == m_isModified)
    //        return;
    //    m_isModified = value;
    //    emit changed();
}

void LDesignDocument::slotDesignWindowRemoved(LDesigner *w)
{
    // Release formwindow as soon as the FormWindowManager removes
    // as calls to isDirty() are triggered at arbitrary times
    // while building.
    if (w == d->m_designWindow)
        d->m_designWindow = 0;
}

LDesigner *LDesignDocument::designWindow() const
{
    return d->m_designWindow;
}

void LDesignDocument::syncXmlFromDesignWindow()
{
    document()->setPlainText(designWindowContents());
}

QString LDesignDocument::designWindowContents() const
{
    QString contents = QString();
    if(d->m_designWindow) {
        LDesignDocument *doc = (LDesignDocument *)d->m_designWindow->document();
        contents = doc->parseDesignWindowContentsToXML();
    }
    return contents;
}

QString LDesignDocument::parseDesignWindowContentsToXML()
{
    QString xmlString;
    LDesignerWidget*     designerWidget = 0;
    QList<IProcessNode*> pnodes;
    QList<IPortLink*>    plinks;
    int numProcessNodes = 0;

    if(d->m_designWindow) {
        designerWidget = d->m_designWindow->widget();
        QList<QGraphicsItem*> items = designerWidget->scene()->items();

        /// Find master start and end nodes
        IProcessNode* masterEndNode = 0;
        IProcessNode* masterStartNode = 0;
        foreach(QGraphicsItem* p, items) {
            IProcessNode* cnode = dynamic_cast<IProcessNode*>(p);
            if(cnode) {
                numProcessNodes++;
                if(cnode->processType() == ProcessType::MASTER_END_NODE) {
                    masterEndNode = cnode;
                }
                if(cnode->processType() == ProcessType::MASTER_START_NODE) {
                    masterStartNode = cnode;
                }
            }
        }

        /// Find all links
        foreach(QGraphicsItem* p, items) {
            IPortLink* clink = dynamic_cast<IPortLink*>(p);
            if(clink) {
                plinks.append(clink);
            }
        }
        if((plinks.size() == numProcessNodes-1) && masterEndNode && masterStartNode) {
            // If the workflow is finished
            // Append all nodes in reverse order
            IProcessNode* cnode = masterEndNode;
            pnodes.prepend(masterEndNode);
            while(cnode != masterStartNode) {
                foreach(IPortLink* clink, plinks) {
                    OutputPort* fromPort = static_cast<OutputPort*>(clink->fromPort());
                    InputPort*  toPort   = static_cast<InputPort*>(clink->toPort());
                    if(toPort->iProcessNodeParent() == cnode) {
                        cnode = fromPort->iProcessNodeParent();
                        pnodes.prepend(cnode);
                        break;
                    }
                }
            }
        } else {
            // Workflow not finished
            // Just get all process nodes. Do not care about the order
            foreach(QGraphicsItem* p, items) {
                IProcessNode* cnode = dynamic_cast<IProcessNode*>(p);
                if(cnode) {
                    pnodes.append(cnode);
                }
            }
        }

        /// XML parsing
        // Start
        QXmlStreamWriter xmlWriter(&xmlString);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.setAutoFormattingIndent(2);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("LigerData");

        // Write all process nodes properties
        QMutableListIterator<IProcessNode *>  node_it(pnodes);
        while(node_it.hasNext()) {
            IProcessNode*  node = static_cast<IProcessNode *>(node_it.next());
            node->generateLigerDocumentContent(xmlWriter);
        }

        /// Write all links
        QMutableListIterator<IPortLink *> link_it(plinks);
        while (link_it.hasNext()) {
            QGraphicsItem* ni   = link_it.next();
            IPortLink*     link = 0;
            link = dynamic_cast<IPortLink *>(ni);

            if(link) {
                xmlWriter.writeStartElement("Link");
                OutputPort* fromPort = static_cast<OutputPort*>(link->fromPort());
                InputPort*  toPort   = static_cast<InputPort*>(link->toPort());
                if(fromPort) {
                    QString fnid = fromPort->parentIDStr();
                    qint32  fpid = fromPort->portId();
                    xmlWriter.writeTextElement("FromNode",fnid);
                    xmlWriter.writeTextElement("FromPort",QString::number(fpid));
                }
                if(toPort) {
                    QString tnid = toPort->parentIDStr();
                    qint32  tpid = toPort->portId();
                    xmlWriter.writeTextElement("ToNode",tnid);
                    xmlWriter.writeTextElement("ToPort",QString::number(tpid));
                }
                xmlWriter.writeEndElement();
            }
        }

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        // XML end
    }
    return xmlString;
}

/*!
 * \brief LDesignDocument::loadXMLDocumentContents
 * \todo validate the xml
 * \todo consider to give the xml reading and writing ownership to each node
 * \return
 */
bool LDesignDocument::loadXMLDocumentContents()
{
    // Clean up the current design
    removeAllProcessNodesAndLinks();

    /// \todo Before you set the contents, validate the XML!
    bool success = true;

    // Get the available process nodes from the object pool.
    QList<IProcessNodeFactory *> process_factories =
            ExtensionSystem::PluginManager::instance()->getObjects<IProcessNodeFactory>();
    QMap<QString, IProcessNodeFactory *> factoryMap;
    foreach(IProcessNodeFactory* pf, process_factories) {
        factoryMap[pf->className()] = pf;
    }

    // The loaded text is available from document->toPlainText() which returns a QString.
    QString contentsString = document()->toPlainText();
    QDomDocument dom(contentsString);
    QDomNodeList processNodes = dom.elementsByTagName("ProcessNode");
    QDomNodeList nodeLinks = dom.elementsByTagName("Links");
    int chunks = processNodes.count() + nodeLinks.count();
    QFutureInterface<void> pminterface;
    pminterface.setProgressRange(0, chunks);
    Core::ProgressManager::addTask(pminterface.future(), tr("Opening File"),
                                   Constants::TASK_OPEN_FILE);

    int chunkCount = 0;
    pminterface.reportStarted();

    QXmlStreamReader xmlReader(contentsString);
    while(!xmlReader.hasError() && !xmlReader.atEnd()) {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        switch(toLigerDataLabel(xmlReader.name())) {
        case (LigerDataLabel::ProcessNode): {
            chunkCount++;
            pminterface.setProgressValue(chunkCount);
            IProcessNode* node = 0;
            QString classNameString;

            ///[] Read class name
            QXmlStreamAttributes attr = xmlReader.attributes();
            if(attr.hasAttribute("ClassName")) {
                classNameString = attr.value("ClassName").toString();
            } else {
                QMessageBox::warning(0, tr("Loading Liger File"),
                                     tr("Class Name Undefined."),
                                     QMessageBox::Ok);
                pminterface.cancel();
                return false;
            }

            ///[] Create node
            IProcessNodeFactory* factory = factoryMap[classNameString];
            if(factory) {
                node = factory->createProcessNode();
                if(!node) {
                    QMessageBox::warning(0, tr("Loading Liger File")
                                         , tr("Cannot initialise Operator ")
                                         + classNameString
                                         , QMessageBox::Ok);
                }
            } else {
                QMessageBox::warning(0, tr("Loading Liger File")
                                     , tr("Unrecognised Operator ")
                                     + classNameString
                                     , QMessageBox::Ok);
                pminterface.cancel();
                return false;
            }

            xmlReader.readNext();

            /// [] Load properties
            while(toLigerDataLabel(xmlReader.name()) != LigerDataLabel::ProcessNode) {
                switch (toLigerDataLabel(xmlReader.name())) {
                case (LigerDataLabel::GUIProperties): {
                    xmlReader.readNext();
                    try {
                        node->loadLigerDocumentContent(xmlReader);
                        d->m_nodeIdList.append(node->nodeIDStr());
                    } catch (...) {
                        QMessageBox::warning(0, tr("Loading Liger File")
                                             , tr("Unrecognised Property ")
                                             , QMessageBox::Ok);
                    }
                }
                    break;
                default:
                    break;
                }
                xmlReader.readNext();
            }
            addProcessNode(node);
        }
            break;
        case (LigerDataLabel::Link): {
            chunkCount++;
            pminterface.setProgressValue(chunkCount);
            xmlReader.readNext();
            while(toLigerDataLabel(xmlReader.name()) != LigerDataLabel::Link) {
                switch(toLigerDataLabel(xmlReader.name())) {
                case (LigerDataLabel::FromNode): {
                    QString fromNodeString = xmlReader.readElementText();
                    d->m_fromNodeList.append(fromNodeString);
                }
                    break;
                case (LigerDataLabel::FromPort): {
                    QString fromPortString = xmlReader.readElementText();
                    qint32 fromPort = fromPortString.toInt();
                    d->m_fromPortList.append(fromPort);
                }
                    break;
                case (LigerDataLabel::ToNode): {
                    QString toNodeString = xmlReader.readElementText();
                    d->m_toNodeList.append(toNodeString);
                }
                    break;
                case (LigerDataLabel::ToPort): {
                    QString toPortString = xmlReader.readElementText();
                    qint32 toPort = toPortString.toInt();
                    d->m_toPortList.append(toPort);
                }
                    break;
                default:
                    break;
                }
                xmlReader.readNext();
            }
        }
            break;
        default:
            break;
        }

        xmlReader.readNext();
    }
    if(xmlReader.hasError()) {
        qDebug() << xmlReader.errorString();
        success = false;
    }

    // Loads the read XML contents into the designeditor scene.
    d->m_designWindow->loadDesign();
    // Unselect the last item
    if(d->m_designWindow->widget()->scene()->selectedItems().size() > 0) {
        d->m_designWindow->widget()->scene()->selectedItems()[0]->setSelected(false);
    }

    // Notify the progress manager that the contents have been loaded.
    pminterface.reportFinished();

    return success;
}

void LDesignDocument::setDesigner(LDesigner* designer)
{
    d->m_designWindow = designer;
    connect(d->m_designWindow, SIGNAL(changed()), SLOT(updateIsModified()));
}

QList<QString> LDesignDocument::nodeIds()
{
    return d->m_nodeIdList;
}

QList<QString> LDesignDocument::fromNode()
{
    return d->m_fromNodeList;
}

QList<qint32> LDesignDocument::fromPort()
{
    return d->m_fromPortList;
}

QList<QString> LDesignDocument::toNode()
{
    return d->m_toNodeList;
}

QList<qint32> LDesignDocument::toPort()
{
    return d->m_toPortList;
}

QList<qreal> LDesignDocument::nodesXPos()
{
    return d->m_nodeXposList;
}

QList<qreal> LDesignDocument::nodesYPos()
{
    return d->m_nodeYposList;
}

void LDesignDocument::addProcessNode(IProcessNode* node)
{
    d->m_processNodes.append(node);
    emit this->document()->setModified(true);
}

void LDesignDocument::removeProcessNode(IProcessNode* node)
{
    int idx = d->m_processNodes.indexOf(node);
    if(idx != -1) {
        d->m_processNodes.removeAt(idx);
        emit this->document()->setModified(true);
    }
}

void LDesignDocument::removeProcessNodeAt(qint32 idx)
{
    if(idx>=0 && idx<d->m_processNodes.count()) {
        d->m_processNodes.removeAt(idx);
        emit this->document()->setModified(true);
    }
}

void LDesignDocument::removeAllProcessNodes()
{
    d->m_processNodes.clear();
    //d->m_nodeClassNameList.clear();
    d->m_nodeIdList.clear();
    d->m_nodeXposList.clear();
    d->m_nodeYposList.clear();
    emit this->document()->setModified(true);
}

void LDesignDocument::addPortLink(IPortLink* link)
{
    d->m_portLinks.append(link);
    emit this->document()->setModified(true);
}

void LDesignDocument::removePortLink(IPortLink* link)
{
    int idx = d->m_portLinks.indexOf(link);
    if(idx != -1) {
        d->m_portLinks.removeAt(idx);
        emit this->document()->setModified(true);
    }
}

void LDesignDocument::removeAllPortLinks()
{
    d->m_portLinks.clear();
    d->m_fromNodeList.clear();
    d->m_fromPortList.clear();
    d->m_toNodeList.clear();
    d->m_toPortList.clear();
    emit this->document()->setModified(true);
}

void LDesignDocument::removeAllProcessNodesAndLinks()
{
    // clear scene
    if(designWindow()->widget()->scene()->children().count()>0) {
        // This function call deletes all process nodes and ports
        designWindow()->widget()->scene()->clear();
    }
    // remove stored links and nodes
    removeAllPortLinks();
    removeAllProcessNodes();
}

QList<IPortLink*> LDesignDocument::portLinks()
{
    return d->m_portLinks;
}

QList<IProcessNode*> LDesignDocument::processNodes()
{
    return d->m_processNodes;
}

// Private -------------------------------------------

LDesignDocument::LigerDataLabel LDesignDocument::toLigerDataLabel(QStringRef str) const
{
    LigerDataLabel label = LigerDataLabel::Unknown;
    if(str == "ProcessNode") {
        label = LigerDataLabel::ProcessNode;
    }
    if(str == "Link") {
        label = LigerDataLabel::Link;
    }
    if(str == "NodeId") {
        label = LigerDataLabel::NodeId;
    }
    if(str == "GUIProperties") {
        label = LigerDataLabel::GUIProperties;
    }
    if(str == "Properties") {
        label = LigerDataLabel::Properties;
    }
    if(str == "xpos") {
        label = LigerDataLabel::XPos;
    }
    if(str == "ypos") {
        label = LigerDataLabel::YPos;
    }
    if(str == "FromNode") {
        label = LigerDataLabel::FromNode;
    }
    if(str == "FromPort") {
        label = LigerDataLabel::FromPort;
    }
    if(str == "ToNode") {
        label = LigerDataLabel::ToNode;
    }
    if(str == "ToPort") {
        label = LigerDataLabel::ToPort;
    }

    return label;
}

QString LDesignDocument::processTypeStr(ProcessType type) const
{
    QString typeStr;
    switch(type) {
    case ProcessType::MASTER_START_NODE:
        typeStr = QString("MASTER_START_NODE");
        break;
    case ProcessType::MASTER_END_NODE:
        typeStr = QString("MASTER_END_NODE");
        break;
    case ProcessType::ALGORITHM_NODE:
        typeStr = QString("ALGORITHM_NODE");
        break;
    case ProcessType::OPERATOR_NODE:
        typeStr = QString("OPERATOR_NODE");
        break;
    case ProcessType::PROBLEM_NODE:
        typeStr = QString("PROBLEM_NODE");
        break;
    default:
        break;
    }
    return typeStr;
}
} // namespace Designer
