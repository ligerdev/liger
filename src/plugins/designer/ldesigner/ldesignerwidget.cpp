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
#include "ldesigner.h"
#include "ldesignerwidget.h"

#include <designer/designeditorplugin.h>
#include <designer/ldesigner/ldesigndocument.h>
#include <designer/designeditorplugin.h>
#include <designer/iengine/processengine.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/iprocessnodefactory.h>
#include <designer/iengine/iport.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/portlinkfactory.h>
#include <designer/iengine/portlink.h>
#include <designer/iengine/iengine.h>
#include <designer/iengine/ienginefactory.h>

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/iplugin.h>

#include <aggregation/aggregate.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/infobar.h>
#include <coreplugin/id.h>
#include <coreplugin/manhattanstyle.h>
#include <coreplugin/messagemanager.h>

#include <extensionsystem/pluginmanager.h>

#include <utils/styledbar.h>
#include <utils/linecolumnlabel.h>
#include <utils/hostosinfo.h>
#include <utils/qtcassert.h>
#include <utils/stylehelper.h>
#include <utils/changeset.h>

#include <QCoreApplication>
#include <QTextCodec>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QTimeLine>
#include <QTime>
#include <QMimeData>
#include <QAbstractTextDocumentLayout>
#include <QAbstractButton>
#include <QApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QScrollBar>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QTextLayout>
#include <QToolBar>
#include <QInputDialog>
#include <QMenu>
#include <QEvent>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QSet>
#include <QPointer>
#include <QDropEvent>

namespace Designer {

using namespace Designer;
using namespace Designer::Internal;
using namespace Utils;

//static void drawRectBox(QPainter *painter, const QRect &rect, bool start, bool end,
//                        const QPalette &pal)
//{
//    painter->save();
//    painter->setRenderHint(QPainter::Antialiasing, false);

//    QRgb b = pal.base().color().rgb();
//    QRgb h = pal.highlight().color().rgb();
//    QColor c = Utils::StyleHelper::mergedColors(b,h, 50);

//    QLinearGradient grad(rect.topLeft(), rect.topRight());
//    grad.setColorAt(0, c.lighter(110));
//    grad.setColorAt(1, c.lighter(130));
//    QColor outline = c;

//    painter->fillRect(rect, grad);
//    painter->setPen(outline);
//    if (start)
//        painter->drawLine(rect.topLeft() + QPoint(1, 0), rect.topRight() -  QPoint(1, 0));
//    if (end)
//        painter->drawLine(rect.bottomLeft() + QPoint(1, 0), rect.bottomRight() -  QPoint(1, 0));

//    painter->drawLine(rect.topRight() + QPoint(0, start ? 1 : 0), rect.bottomRight() - QPoint(0, end ? 1 : 0));
//    painter->drawLine(rect.topLeft() + QPoint(0, start ? 1 : 0), rect.bottomLeft() - QPoint(0, end ? 1 : 0));

//    painter->restore();
//}


//static QColor blendColors(const QColor &a, const QColor &b, int alpha)
//{
//    return QColor((a.red()   * (256 - alpha) + b.red()   * alpha) / 256,
//                  (a.green() * (256 - alpha) + b.green() * alpha) / 256,
//                  (a.blue()  * (256 - alpha) + b.blue()  * alpha) / 256);
//}

//static QColor calcBlendColor(const QColor &baseColor, int level, int count)
//{
//    QColor color80;
//    QColor color90;

//    if (baseColor.value() > 128) {
//        const int f90 = 15;
//        const int f80 = 30;
//        color80.setRgb(qMax(0, baseColor.red() - f80),
//                       qMax(0, baseColor.green() - f80),
//                       qMax(0, baseColor.blue() - f80));
//        color90.setRgb(qMax(0, baseColor.red() - f90),
//                       qMax(0, baseColor.green() - f90),
//                       qMax(0, baseColor.blue() - f90));
//    } else {
//        const int f90 = 20;
//        const int f80 = 40;
//        color80.setRgb(qMin(255, baseColor.red() + f80),
//                       qMin(255, baseColor.green() + f80),
//                       qMin(255, baseColor.blue() + f80));
//        color90.setRgb(qMin(255, baseColor.red() + f90),
//                       qMin(255, baseColor.green() + f90),
//                       qMin(255, baseColor.blue() + f90));
//    }

//    if (level == count)
//        return baseColor;
//    if (level == 0)
//        return color80;
//    if (level == count - 1)
//        return color90;

//    const int blendFactor = level * (256 / (count - 2));

//    return blendColors(color80, color90, blendFactor);
//}

//static int foldBoxWidth(const QFontMetrics &fm)
//{
//    const int lineSpacing = fm.lineSpacing();
//    return lineSpacing + lineSpacing%2 + 1;
//}

//static void printPage(int index, QPainter *painter, const QTextDocument *doc,
//                      const QRectF &body, const QRectF &titleBox,
//                      const QString &title)
//{
//    painter->save();

//    painter->translate(body.left(), body.top() - (index - 1) * body.height());
//    QRectF view(0, (index - 1) * body.height(), body.width(), body.height());

//    QAbstractTextDocumentLayout *layout = doc->documentLayout();
//    QAbstractTextDocumentLayout::PaintContext ctx;

//    painter->setFont(QFont(doc->defaultFont()));
//    QRectF box = titleBox.translated(0, view.top());
//    int dpix = painter->device()->logicalDpiX();
//    int dpiy = painter->device()->logicalDpiY();
//    int mx = 5 * dpix / 72.0;
//    int my = 2 * dpiy / 72.0;
//    painter->fillRect(box.adjusted(-mx, -my, mx, my), QColor(210, 210, 210));
//    if (!title.isEmpty())
//        painter->drawText(box, Qt::AlignCenter, title);
//    const QString pageString = QString::number(index);
//    painter->drawText(box, Qt::AlignRight, pageString);

//    painter->setClipRect(view);
//    ctx.clip = view;
//    // don't use the system palette text as default text color, on HP/UX
//    // for example that's white, and white text on white paper doesn't
//    // look that nice
//    ctx.palette.setColor(QPalette::Text, Qt::black);

//    layout->draw(painter, ctx);

//    painter->restore();
//}

LDesignerAnimator::LDesignerAnimator(QObject *parent)
    :QObject(parent)
{
    m_value = 0;
    m_timeline = new QTimeLine(256, this);
    m_timeline->setCurveShape(QTimeLine::SineCurve);
    connect(m_timeline, SIGNAL(valueChanged(qreal)), this, SLOT(step(qreal)));
    connect(m_timeline, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_timeline->start();
}


void LDesignerAnimator::setData(QFont f, QPalette pal, const QString &text)
{
    m_font = f;
    m_palette = pal;
    m_text = text;
    QFontMetrics fm(m_font);
    m_size = QSizeF(fm.width(m_text), fm.height());
}

void LDesignerAnimator::draw(QPainter *p, const QPointF &pos)
{
    m_lastDrawPos = pos;
    p->setPen(m_palette.text().color());
    QFont f = m_font;
    f.setPointSizeF(f.pointSizeF() * (1.0 + m_value/2));
    QFontMetrics fm(f);
    int width = fm.width(m_text);
    QRectF r((m_size.width()-width)/2, (m_size.height() - fm.height())/2, width, fm.height());
    r.translate(pos);
    p->fillRect(r, m_palette.base());
    p->setFont(f);
    p->drawText(r, m_text);
}

bool LDesignerAnimator::isRunning() const
{
    return m_timeline->state() == QTimeLine::Running;
}

QRectF LDesignerAnimator::rect() const
{
    QFont f = m_font;
    f.setPointSizeF(f.pointSizeF() * (1.0 + m_value/2));
    QFontMetrics fm(f);
    int width = fm.width(m_text);
    return QRectF((m_size.width()-width)/2, (m_size.height() - fm.height())/2, width, fm.height());
}

void LDesignerAnimator::step(qreal v)
{
    QRectF before = rect();
    m_value = v;
    QRectF after = rect();
    emit updateRequest(m_position, m_lastDrawPos, before.united(after));
}

void LDesignerAnimator::finish()
{
    m_timeline->stop();
    step(0);
    deleteLater();
}

//================LDesignerWidgetPrivate==============

class LDesignerWidgetPrivate
{

public:
    LDesignerWidgetPrivate();
    ~LDesignerWidgetPrivate();

    void setupDocumentSignals(LDesignDocument *document);
    void updateLineSelectionColor();
    void print(QPrinter *printer);

    LDesignerWidget *q;
    QPointer<LDesigner> m_designer;

    bool m_contentsChanged;

    QString  m_documentTitle;
    QPointer<LDesignDocument> m_document;

    QByteArray m_tempNavigationState;

    QString m_displayName;
    bool m_parenthesesMatchingEnabled;

    QWidget* m_extraArea;
    int extraAreaSelectionAnchorBlockNumber;
    int extraAreaToggleMarkBlockNumber;
    int extraAreaHighlightFoldedBlockNumber;

    QPointer<QGraphicsScene> m_scene;
    QPointer<QGraphicsView>  m_view;

    bool m_isDesignRunning;

    int groupId;
    int itemId;

    Core::ActionContainer* m_editMenu;
    QAction*  m_addNodeAction;
    QAction*  m_addLinkAction;
    QAction*  m_deleteAction;
    QAction*  m_cutAction;
    QAction*  m_copyAction;
    QAction*  m_pasteAction;
    QAction*  m_bringToFrontAction;
    QAction*  m_sendToBackAction;
    QAction*  m_propertiesAction;

    QGraphicsLineItem* m_line;
    QColor             m_lineColor;

    QPointer<PortLinkFactory>     m_portLinkFactory;
    QPointer<IProcessNodeFactory> m_processNodeFactory;
    QPointer<IEngine> m_engine;
    int m_minZ;
    int m_maxZ;
    int m_seqNumber;
    QMutex m_mutex;

    bool m_isDrawingLink;
};

//---------------- LDesignerWidgetPrivate ---------------
LDesignerWidgetPrivate::LDesignerWidgetPrivate()
    : q(0),
      m_designer(0),
      m_contentsChanged(false),
      m_documentTitle("Unknown Document"),
      m_document(new LDesignDocument),
      m_parenthesesMatchingEnabled(false),
      m_extraArea(0),
      extraAreaSelectionAnchorBlockNumber(-1),
      extraAreaToggleMarkBlockNumber(-1),
      extraAreaHighlightFoldedBlockNumber(-1),
      m_isDesignRunning(false),
      m_line(0),
      m_lineColor(Qt::black),
      m_engine(0),
      m_seqNumber(0),
      m_isDrawingLink(false)
{

}

LDesignerWidgetPrivate::~LDesignerWidgetPrivate()
{
    if(!m_document.isNull())
        delete m_document;

    if(!m_scene.isNull()) {
        QList<QGraphicsItem *> items = m_scene->items();
        QList<IProcessNode *> pnodes;
        QList<IPortLink *>    plinks;
        foreach(QGraphicsItem* p, items) {
            IProcessNode* cnode = dynamic_cast<IProcessNode*>(p);
            if(cnode)
                pnodes.append(cnode);
        }
        foreach(QGraphicsItem* p, items) {
            IPortLink* clink = dynamic_cast<IPortLink*>(p);
            if(clink)
                plinks.append(clink);
        }

        QMutableListIterator<IProcessNode *>  node_it(pnodes);
        QMutableListIterator<IPortLink *>     link_it(plinks);

        while (link_it.hasNext()) {
            QGraphicsItem* ni   = link_it.next();
            IPortLink*     link = 0;
            link = dynamic_cast<IPortLink *>(ni);

            if(link) {
                delete link;
                link_it.remove();
            }
        }

        while (node_it.hasNext()) {
            QGraphicsItem* ni   = node_it.next();
            IProcessNode*  node = 0;
            node = dynamic_cast<IProcessNode *>(ni);

            if(node) {
                // m_engine->removeProcessNode(node);
                delete node;
                node_it.remove();
            }
        }
        delete m_scene;
    }
    if(!m_view.isNull()) {
        delete m_view;
    }

    if(!m_portLinkFactory.isNull())
        delete m_portLinkFactory;
    if(!m_processNodeFactory.isNull())
        delete m_processNodeFactory;
    if(!m_engine.isNull())
        m_engine->removeAllNodes();
}

void LDesignerWidgetPrivate::setupDocumentSignals(LDesignDocument *document)
{
    if (m_document) {
        q->disconnect(m_document->document(), 0, q, 0);
        q->disconnect(m_document, 0, q, 0);
    }

    m_document->deleteLater();
    //    QTextDocument *doc = document->document();
    //    Q_UNUSED(doc)
    ////////////////////////////////////////////////////////////////////////////////////
    //    q->setDocument(doc);
    //    q->setDocument(document);
    m_document = document;

    //    QObject::connect(documentLayout, SIGNAL(updateBlock(QTextBlock)), q, SLOT(slotUpdateBlockNotify(QTextBlock)));
    //    QObject::connect(documentLayout, SIGNAL(updateExtraArea()), q, SLOT(slotUpdateExtraArea()));
    //    QObject::connect(q, SIGNAL(requestBlockUpdate(QTextBlock)), documentLayout, SIGNAL(updateBlock(QTextBlock)));
    //    QObject::connect(doc, SIGNAL(modificationChanged(bool)), q, SIGNAL(changed()));
    //    QObject::connect(doc, SIGNAL(contentsChange(int,int,int)), q,
    //        SLOT(designerContentsChange(int,int,int)), Qt::DirectConnection);
    QObject::connect(document, SIGNAL(changed()), q, SIGNAL(changed()));
    QObject::connect(document, SIGNAL(titleChanged(QString)), q, SLOT(setDisplayName(QString)));
}

//=================================================
LDesignerWidget::LDesignerWidget(QWidget *parent)
{
    Q_UNUSED(parent)

    d    = new LDesignerWidgetPrivate();
    d->q = this;
    //    d->m_extraArea = new QGraphicsScene(this);
    //    d->m_extraArea->setMouseTracking(true);

    //    setParent(parent);
    setFocusPolicy(Qt::ClickFocus);
    setMimeType(QLatin1String(Designer::Constants::C_DESIGNER_MIMETYPE_TEXT));
    // TODO: This needs to display the file name.
    setDisplayName(tr(Core::Constants::K_DEFAULT_TEXT_EDITOR_DISPLAY_NAME));
    connect(designerDocument(), SIGNAL(changed()), this, SLOT(configure()));

    QHBoxLayout* tl = new QHBoxLayout;

    d->m_engine = 0;

    tl->setMargin(0);
    tl->setSpacing(0);
    qreal scene_width  = 5000;
    qreal scene_height = 5000;
    qreal scene_x      = -(scene_width/2);
    qreal scene_y      = -(scene_height/2);
    d->m_scene = new QGraphicsScene(scene_x,scene_y,scene_width,scene_height,this);
    d->m_scene->installEventFilter(this);

    setMouseTracking(true);
    d->m_lineColor = Qt::black;
    d->m_view = new QGraphicsView(this);
    d->m_view->setScene(d->m_scene);
    d->m_view->installEventFilter(this);
    d->m_view->setDragMode(QGraphicsView::NoDrag);
    d->m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    //    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //    m_editToolBar = new QToolBar;
    //    m_editToolBar->setOrientation(Qt::Vertical);

    tl->addWidget(d->m_view);
    //    tl->addWidget(m_editToolBar);

    setLayout(tl);

    //    d->m_overlay = new DesignerOverlay(this);
    d->setupDocumentSignals(d->m_document);

    setLayoutDirection(Qt::LeftToRight);
    viewport()->setMouseTracking(true);
    d->extraAreaSelectionAnchorBlockNumber
            = d->extraAreaToggleMarkBlockNumber
            = d->extraAreaHighlightFoldedBlockNumber
            = -1;

    // Nodes and Links init
    d->m_portLinkFactory    = new PortLinkFactory(this);
    d->m_processNodeFactory = new IProcessNodeFactory(this);

    connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(slotModificationChanged(bool)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(slotUpdateRequest(QRect,int)));

    slotUpdateExtraAreaWidth();
    setFrameStyle(QFrame::NoFrame);

    createMenus();
    createToolBars();
    d->m_view->setAcceptDrops(true);
}

LDesignerWidget::LDesignerWidget(const QString &text, QWidget *parent)
{
    Q_UNUSED(parent)

    d    = new LDesignerWidgetPrivate();
    d->q = this;
    d->m_documentTitle = text;

    setFocusPolicy(Qt::ClickFocus);
    setMimeType(QLatin1String(Designer::Constants::C_DESIGNER_MIMETYPE_TEXT));
    setDisplayName(tr(Core::Constants::K_DEFAULT_TEXT_EDITOR_DISPLAY_NAME));
    connect(designerDocument(), SIGNAL(changed()), this, SLOT(configure()));
    setMouseTracking(true);

    QHBoxLayout* tl = new QHBoxLayout;

    tl->setMargin(0);
    tl->setSpacing(0);
    qreal scene_width  = 5000;
    qreal scene_height = 5000;
    qreal scene_x      = -(scene_width/2);
    qreal scene_y      = -(scene_height/2);
    d->m_scene = new QGraphicsScene(scene_x,scene_y,scene_width,scene_height,this);
    d->m_scene->installEventFilter(this);

    d->m_lineColor = Qt::black;
    d->m_view = new QGraphicsView(this);
    d->m_view->setScene(d->m_scene);
    d->m_view->installEventFilter(this);
    d->m_view->setDragMode(QGraphicsView::NoDrag);
    d->m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    //    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //    m_editToolBar = new QToolBar;
    //    m_editToolBar->setOrientation(Qt::Vertical);

    tl->addWidget(d->m_view);
    //    tl->addWidget(m_editToolBar);

    setLayout(tl);

    //    d->m_overlay = new DesignerOverlay(this);
    d->setupDocumentSignals(d->m_document);

    setLayoutDirection(Qt::LeftToRight);
    viewport()->setMouseTracking(true);
    d->extraAreaSelectionAnchorBlockNumber
            = d->extraAreaToggleMarkBlockNumber
            = d->extraAreaHighlightFoldedBlockNumber
            = -1;

    // Nodes and Links init
    d->m_portLinkFactory    = new PortLinkFactory(this);
    d->m_processNodeFactory = new IProcessNodeFactory(this);

    connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(slotModificationChanged(bool)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(slotUpdateRequest(QRect,int)));

    slotUpdateExtraAreaWidth();
    setFrameStyle(QFrame::NoFrame);

    createMenus();
    createToolBars();
    d->m_view->setAcceptDrops(true);
}

LDesignerWidget::~LDesignerWidget()
{
    delete d;
}

// Event helps the LDesignerWidget intercept the mouse events before they are
// passed to the main window object, which essentially will ignore drags/drops.
bool LDesignerWidget::eventFilter(QObject* target, QEvent* event)
{
    if(target == d->m_scene || target == d->m_view) {
        if(event->type() == QEvent::GraphicsSceneMousePress) {
            QGraphicsSceneMouseEvent* ev = static_cast<QGraphicsSceneMouseEvent*>(event);
            mousePressEvent(ev);
            // By returning false, the event is further processed by QGraphcisScene::mousePressEvent.
            // So, since we don't handle all possibilities this is a good idea. To stop this return true.
            return false;
        } else if(event->type() == QEvent::GraphicsSceneMouseRelease) {
            QGraphicsSceneMouseEvent* ev = static_cast<QGraphicsSceneMouseEvent*>(event);
            mouseReleaseEvent(ev);
            return false;
        } else if(event->type() == QEvent::GraphicsSceneMouseMove) {
            QGraphicsSceneMouseEvent* ev = static_cast<QGraphicsSceneMouseEvent*>(event);
            return mouseMoveEvent(ev);
        } else if(event->type() == QEvent::GraphicsSceneWheel) {
            QGraphicsSceneWheelEvent* ev = static_cast<QGraphicsSceneWheelEvent*>(event);
            wheelEvent(ev);
        } else if(event->type() == QEvent::GraphicsSceneDragEnter) {
            QGraphicsSceneDragDropEvent* ev =
                    static_cast<QGraphicsSceneDragDropEvent*>(event);
            dragEnterEvent(ev);
            return true;
        } else if(event->type() == QEvent::GraphicsSceneDragMove) {
            QGraphicsSceneDragDropEvent* ev =
                    static_cast<QGraphicsSceneDragDropEvent*>(event);
            dragMoveEvent(ev);
            return true;
        } else if(event->type() == QEvent::GraphicsSceneDrop) {
            QGraphicsSceneDragDropEvent* ev =
                    static_cast<QGraphicsSceneDragDropEvent*>(event);
            dropEvent(ev);
            return true;
        }
    }

    return false;
}

QString LDesignerWidget::mimeType() const
{
    return d->m_document->mimeType();
}

void LDesignerWidget::setMimeType(const QString &mt)
{
    d->m_document->setMimeType(mt);
}

void LDesignerWidget::print(QPrinter *printer)
{
    const bool oldFullPage =  printer->fullPage();
    printer->setFullPage(true);
    QPrintDialog *dlg = new QPrintDialog(printer, this);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        d->print(printer);
    }
    printer->setFullPage(oldFullPage);
    delete dlg;
}

void LDesignerWidgetPrivate::print(QPrinter *printer)
{
    Q_UNUSED(printer)
}


LDesigner *LDesignerWidget::designer() const
{
    return d->m_designer;
}

void LDesignerWidget::setDesigner(LDesigner* designer)
{
    d->m_designer = designer;
}

bool LDesignerWidget::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
    if (d->m_document->open(errorString, fileName, realFileName)) {
        return true;
    }
    return false;
}

Core::IDocument *LDesignerWidget::designerDocument() const
{
    return d->m_document;
}

void LDesignerWidget::designerContentsChange(int position, int charsRemoved, int charsAdded)
{
    Q_UNUSED(position)
    Q_UNUSED(charsRemoved)
    Q_UNUSED(charsAdded)
}

void LDesignerWidget::keyPressEvent(QKeyEvent *e)
{
    Q_UNUSED(e)
}

bool LDesignerWidget::event(QEvent *e)
{
    Q_UNUSED(e)
    return false;
}

void LDesignerWidget::duplicateFrom(LDesignerWidget *widget)
{
    if (this == widget)
        return;
    setDisplayName(widget->displayName());
    if (d->m_document == widget->d->m_document)
        return;
    d->setupDocumentSignals(widget->d->m_document);
    d->m_document = widget->d->m_document;
}

QString LDesignerWidget::displayName() const
{
    return d->m_displayName;
}

void LDesignerWidget::setDisplayName(const QString &title)
{
    d->m_displayName = title;
    emit changed();
}

LDesignDocument *LDesignerWidget::DesignDocument() const
{
    return d->m_document;
}


void LDesignerWidget::setDesignDocument(LDesignDocument *doc)
{
    if (doc) {
        d->setupDocumentSignals(doc);
        d->m_document = doc;
    }
}

void LDesignerWidget::setDefaultPath(const QString &defaultPath)
{
    DesignDocument()->setDefaultPath(defaultPath);
}

void LDesignerWidget::setSuggestedFileName(const QString &suggestedFileName)
{
    DesignDocument()->setSuggestedFileName(suggestedFileName);
}

void LDesignerWidget::setMouseNavigationEnabled(bool b)
{
    Q_UNUSED(b)
    //    d->m_behaviorSettings.m_mouseNavigation = b;
}

bool LDesignerWidget::mouseNavigationEnabled() const
{
    //    return d->m_behaviorSettings.m_mouseNavigation;
    return false;
}

void LDesignerWidget::setScrollWheelZoomingEnabled(bool b)
{
    Q_UNUSED(b)
    //    d->m_behaviorSettings.m_scrollWheelZooming = b;
}

bool LDesignerWidget::scrollWheelZoomingEnabled() const
{
    //    return d->m_behaviorSettings.m_scrollWheelZooming;
    return true;
}

void LDesignerWidget::setConstrainTooltips(bool b)
{
    Q_UNUSED(b)
    //    d->m_behaviorSettings.m_constrainHoverTooltips = b;
}

bool LDesignerWidget::constrainTooltips() const
{
    //    return d->m_behaviorSettings.m_constrainHoverTooltips;
    return false;
}

bool LDesignerWidget::viewportEvent(QEvent *event)
{
    Q_UNUSED(event)

    d->m_contentsChanged = false;
    return false;
}


void LDesignerWidget::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
}

void LDesignerWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
}

QWidget *LDesignerWidget::extraArea() const
{
    return d->m_extraArea;
}

int LDesignerWidget::extraAreaWidth(int *markWidthPtr) const
{
    Q_UNUSED(markWidthPtr)

    return 0;
}

void LDesignerWidget::slotUpdateExtraAreaWidth()
{
    if (isLeftToRight())
        setViewportMargins(extraAreaWidth(), 0, 0, 0);
    else
        setViewportMargins(0, 0, extraAreaWidth(), 0);
}

void LDesignerWidget::extraAreaPaintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
}

void LDesignerWidget::slotModificationChanged(bool m)
{
    Q_UNUSED(m)
}

void LDesignerWidget::slotUpdateRequest(const QRect &r, int dy)
{
    Q_UNUSED(r)
    Q_UNUSED(dy)
}

void LDesignerWidget::slotUpdateExtraArea()
{
    d->m_extraArea->update();
}

void LDesignerWidget::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e)
}

void LDesignerWidget::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    qDebug() << Q_FUNC_INFO << e->button();
}

bool LDesignerWidget::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Logic:
    // If left button is not pressed, clear current line and exit
    //
    // If an output port is selected or under cursor, start to add new link

    d->m_isDrawingLink = false;

    // Scroll Hand Drag
    if((QApplication::keyboardModifiers() & Qt::ControlModifier)
            && (mouseEvent->button() == Qt::LeftButton)) {
        d->m_view->setDragMode(QGraphicsView::ScrollHandDrag);
        return false;
    }

    // Check if left button. If not, clear exsting line
    if(mouseEvent->button() != Qt::LeftButton) {
        if(d->m_line) {
            d->m_scene->removeItem(d->m_line);
            delete d->m_line;
            d->m_line = 0;
        }
        return false;
    }

    // Check if a port is selected
    // If yes, prepare for drawing link
    QList<QGraphicsItem*> items = d->m_scene->items(mouseEvent->scenePos());
    if(items.count() > 0) {
        IPort* port = dynamic_cast<IPort*>(items.first());
        if(port) {
            if(!port->isConnected()) {
                // Change the shape
                //d->m_view->setCursor(Qt::PointingHandCursor);
                this->setCursor(Qt::PointingHandCursor);

                // Snap the mouse to the center of the port
                QPointF itemPosF = port->mapToScene(port->boundingRect().center());
                QPoint  itemPos  = port->scene()->views().first()->mapFromScene(itemPosF);
                QPoint  itemGlobalPos = port->scene()->views().first()->mapToGlobal(itemPos);
                this->cursor().setPos(itemGlobalPos);

                // Prepare for drawing links
                d->m_isDrawingLink = true;
                d->m_line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                                         mouseEvent->scenePos()));
                d->m_line->setZValue(-1);
                d->m_line->setPen(QPen(d->m_lineColor, 2));
                d->m_scene->addItem(d->m_line);
                return false;
            }
        }
    }

    // Enter selection mode
    d->m_view->setDragMode(QGraphicsView::RubberBandDrag);
    return false;
}

void LDesignerWidget::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    qDebug() << Q_FUNC_INFO;
}

bool LDesignerWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(d->m_isDrawingLink) {
        d->m_view->setDragMode(QGraphicsView::NoDrag);
        QLineF newLine(d->m_line->line().p1(), mouseEvent->scenePos());
        d->m_line->setLine(newLine);
        QList<QGraphicsItem*> items = d->m_scene->items(d->m_line->line().p2());
        if (items.count() && items.first() == d->m_line) {
            items.removeFirst();
        }
        if(items.count() > 0 && items.first()->type() == InputPort::Type) {
            // If an input port is detected, change the cursor to closed hand
            // and grad the cursor
            //d->m_view->setCursor(Qt::ClosedHandCursor);
            this->setCursor(Qt::ClosedHandCursor);

            QGraphicsItem* inputItem = items.first();
            QPointF itemPosF = inputItem->mapToScene(inputItem->boundingRect().center());
            QPoint  itemPos  = inputItem->scene()->views().first()->mapFromScene(itemPosF);
            QPoint  itemGlobalPos = inputItem->scene()->views().first()->mapToGlobal(itemPos);
            d->m_view->cursor().setPos(itemGlobalPos);
        }
        return false;
    }

    d->m_view->setDragMode(QGraphicsView::RubberBandDrag);

    return false;
}
void LDesignerWidget::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
}

bool LDesignerWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Reset cursor
    //d->m_view->setCursor(Qt::ArrowCursor);
    this->setCursor(Qt::ArrowCursor);

    if(d->m_isDrawingLink) {
        // rest to false
        d->m_isDrawingLink = false;

        if (mouseEvent->button() == Qt::LeftButton) {
            if(QApplication::keyboardModifiers() & Qt::ControlModifier) {
                return false;
            } else {
                if(d->m_line) {
                    QList<QGraphicsItem *> startItems = d->m_scene->items(d->m_line->line().p1());
                    if (startItems.count() && startItems.first() == d->m_line)
                        startItems.removeFirst();
                    QList<QGraphicsItem *> endItems = d->m_scene->items(d->m_line->line().p2());
                    if (endItems.count() && endItems.first() == d->m_line)
                        endItems.removeFirst();

                    d->m_scene->removeItem(d->m_line);
                    delete d->m_line;
                    d->m_line = 0;

                    if (startItems.count() > 0 && endItems.count() > 0 &&
                            startItems.first()->type() == OutputPort::Type &&
                            endItems.first()->type() == InputPort::Type &&
                            startItems.first() != endItems.first()) {
                        OutputPort *startItem = qgraphicsitem_cast<OutputPort *>(startItems.first());
                        InputPort *endItem    = qgraphicsitem_cast<InputPort *>(endItems.first());
                        if(!startItem || !endItem) {
                            return false;
                        }
                        if(!startItem->isConnected() && !endItem->isConnected()) {
                            IPortLink* link = d->m_portLinkFactory->createPortLink(startItem, endItem);
                            setupPortLink(link);
                            // Unselect the start item once the link is connected.
                            startItem->setSelected(false);
                            endItem->setSelected(false);
                            return false;
                        }
                    }
                }
            }
            return false;

        } else { // is drawing link but not LeftButton
            // clear
            if(d->m_line) {
                d->m_scene->removeItem(d->m_line);
                delete d->m_line;
                d->m_line = 0;
            }
            return false;
        }
    }

    return false;
}

void LDesignerWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void LDesignerWidget::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)
}

void LDesignerWidget::keyReleaseEvent(QKeyEvent *e)
{
    Q_UNUSED(e)
}

void LDesignerWidget::dragEnterEvent(QGraphicsSceneDragDropEvent* e)
{
    e->accept();
}

void LDesignerWidget::dragMoveEvent(QGraphicsSceneDragDropEvent* e)
{
    e->accept();
}

void LDesignerWidget::dropEvent(QGraphicsSceneDragDropEvent* e)
{
    e->accept();
    IProcessNodeFactory* pf = selectedProcessNodeFactory();
    if(pf) {
        IProcessNode* node = pf->createProcessNode();
        node->setPos(e->scenePos());
        setupProcessNode(node);
        pf->unSelect();
        node->setSelected(false);
    }
    return;
}

void LDesignerWidget::showDefaultContextMenu(QContextMenuEvent *e, const Core::Id menuContextId)
{
    QMenu menu;
    appendMenuActionsFromContext(&menu, menuContextId);
    appendStandardContextMenuActions(&menu);
    menu.exec(e->globalPos());
}

void LDesignerWidget::extraAreaLeaveEvent(QEvent *)
{

}

void LDesignerWidget::extraAreaContextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}

void LDesignerWidget::extraAreaMouseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

QString LDesignerWidget::documentTitle()
{
    return d->m_documentTitle;
}

void LDesignerWidget::setDocumentTitle(QString doc)
{
    d->m_documentTitle=doc;
}

QGraphicsScene*  LDesignerWidget::scene() const
{
    return d->m_scene;
}

void LDesignerWidget::setScene(QGraphicsScene* scene)
{
    d->m_scene = scene;
}

IEngine *LDesignerWidget::engine() const
{
    return d->m_engine.data();
}

QGraphicsView* LDesignerWidget::getView()
{
    return d->m_view;
}

void LDesignerWidget::wheelEvent(QGraphicsSceneWheelEvent *event)
{
#ifdef WIN32
    d->m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    qreal scaleFactor = qPow(2.0, -event->delta() / 240.0);
    zoom(scaleFactor);
#else
    Q_UNUSED(event);
#endif
}

void LDesignerWidget::zoomIn(qreal scaleFactor)
{
    zoom(scaleFactor);
}

void LDesignerWidget::zoomOut(qreal scaleFactor)
{
    zoom(scaleFactor);
}

void LDesignerWidget::zoomReset()
{
    d->m_view->resetTransform();
}

void LDesignerWidget::fitToWindow(qreal scaleFactor)
{
    d->m_view->fitInView(d->m_scene->itemsBoundingRect(),
                         Qt::KeepAspectRatio);
    zoomOut(scaleFactor);
}

void LDesignerWidget::sceneModified()
{
    d->m_document->document()->setModified(true);
}

/*! \todo Yiming - redo this function; Have some error checking
*/
bool LDesignerWidget::loadDesign()
{
    bool designLoaded = true;
    if(d->m_scene->children().count()>0) {
        d->m_scene->clear();
    }

    QList<IProcessNode*> processNodes = d->m_document->processNodes();
    QList<QString> fromNodes = d->m_document->fromNode();
    QList<qint32>  fromPorts = d->m_document->fromPort();
    QList<QString> toNodes   = d->m_document->toNode();
    QList<qint32>  toPorts   = d->m_document->toPort();

    // Get the available process nodes from the object pool.
    QList<IEngine*> engines = ExtensionSystem::PluginManager::instance()->getObjects<IEngine>();
    if(engines.size() == 0 && d->m_engine == 0) {
        QMessageBox::information(this,"Optimization Engine.","No optimization engine found.");
        return true;
    }

    // Just pick one for now
    /// \todo Support multiple engines in the future
    if(d->m_engine == 0) {
        d->m_engine = engines[0];
        // connect signals from engine with plugin slots
        connect(d->m_engine, SIGNAL(engineStarted()),
                DesignEditorPlugin::instance(), SLOT(swapRunButtonToPause()));
        connect(d->m_engine, SIGNAL(enginePaused()),
                DesignEditorPlugin::instance(), SLOT(resetRunButton()));
        connect(d->m_engine, SIGNAL(engineResumed()),
                DesignEditorPlugin::instance(), SLOT(swapRunButtonToPause()));
        connect(d->m_engine, &IEngine::engineFinished,
                DesignEditorPlugin::instance(), &DesignEditorPlugin::resetRunButton);
        connect(d->m_engine, SIGNAL(engineExit()),
                DesignEditorPlugin::instance(), SLOT(resetRunButton()));
        qDebug() << "Loaded engine: " << d->m_engine->metaObject()->className();
    }

    for(int i=0; i<processNodes.size(); i++) {
        setupProcessNode(processNodes[i]);
    }

    for(int i=0; i<fromNodes.count(); i++) {
        QString nodeId = fromNodes[i];
        foreach(IProcessNode* p, processNodes) {
            if(p->nodeIDStr() == nodeId) {
                OutputPort* oport = p->outputPortAt(fromPorts[i]);
                InputPort*  iport;
                QString onodeId = toNodes[i];
                foreach(IProcessNode* o, processNodes) {
                    if(o->nodeIDStr() == onodeId) {
                        iport = o->inputPortAt(toPorts[i]);
                        break;
                    }
                }
                IPortLink* link = d->m_portLinkFactory->createPortLink(oport, iport);
                setupPortLink(link);
                break;
            }
        }
    }

    return designLoaded;
}
//--------------------------

void LDesignerWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::ApplicationFontChange
            || e->type() == QEvent::FontChange) {
        if (d->m_extraArea) {
            QFont f = d->m_extraArea->font();
            f.setPointSizeF(font().pointSizeF());
            d->m_extraArea->setFont(f);
            slotUpdateExtraAreaWidth();
            d->m_extraArea->update();
        }
    }
}

void LDesignerWidget::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e)
}

void LDesignerWidget::focusOutEvent(QFocusEvent *e)
{
    Q_UNUSED(e)
}

void LDesignerWidget::showEvent(QShowEvent* e)
{
    Q_UNUSED(e)
}

QMimeData *LDesignerWidget::createMimeDataFromSelection() const
{
    return 0;
}

bool LDesignerWidget::canInsertFromMimeData(const QMimeData *source) const
{
    Q_UNUSED(source)
    return false;
}

void LDesignerWidget::insertFromMimeData(const QMimeData *source)
{
    Q_UNUSED(source)
}

QMimeData *LDesignerWidget::duplicateMimeData(const QMimeData *source) const
{
    Q_UNUSED(source)
    return 0;
}

void LDesignerWidget::appendMenuActionsFromContext(QMenu *menu, const Core::Id menuContextId)
{
    Core::ActionContainer *mcontext = Core::ActionManager::actionContainer(menuContextId);
    QMenu *contextMenu = mcontext->menu();

    foreach (QAction *action, contextMenu->actions())
        menu->addAction(action);
}

void LDesignerWidget::appendStandardContextMenuActions(QMenu *menu)
{
    menu->addSeparator();

    QAction *a = Core::ActionManager::command(Core::Constants::CUT)->action();
    if (a && a->isEnabled())
        menu->addAction(a);
    a = Core::ActionManager::command(Core::Constants::COPY)->action();
    if (a && a->isEnabled())
        menu->addAction(a);
    a = Core::ActionManager::command(Core::Constants::PASTE)->action();
    if (a && a->isEnabled())
        menu->addAction(a);
    //    a = Core::ActionManager::command(Constants::CIRCULAR_PASTE)->action();
    //    if (a && a->isEnabled())
    //        menu->addAction(a);

    LDesignDocument *doc = DesignDocument();
    Q_UNUSED(doc)
    //    if (doc->codec()->name() == QString(QLatin1String("UTF-8"))) {
    //        a = Core::ActionManager::command(Constants::SWITCH_UTF8BOM)->action();
    //        if (a && a->isEnabled()) {
    //            a->setText(doc->format().hasUtf8Bom ? tr("Delete UTF-8 BOM on Save")
    //                                                : tr("Add UTF-8 BOM on Save"));
    //            menu->addSeparator();
    //            menu->addAction(a);
    //        }
    //    }
}


void LDesignerWidget::del()
{    
    QList<QGraphicsItem *> items = d->m_scene->selectedItems();
    QList<IProcessNode *> pnodes;
    QList<IPortLink *>    plinks;
    foreach(QGraphicsItem* p, items) {
        IProcessNode* cnode = dynamic_cast<IProcessNode*>(p);
        if(cnode)
            pnodes.append(cnode);
    }
    foreach(QGraphicsItem* p, items) {
        IPortLink* clink = dynamic_cast<IPortLink*>(p);
        if(clink)
            plinks.append(clink);
    }

    QList<IPortLink*> extra_plinks;
    foreach(IProcessNode* p, pnodes) {
        extra_plinks.append(p->portLinks());
    }
    // Now add the selected links and set them all on fire ;)
    plinks.append(extra_plinks);
    // Remove duplicates. There may be a more efficient way but
    // at this stage this works just fine.
    QSet<IPortLink*> tmpset;
    tmpset = tmpset.fromList(plinks);
    plinks.clear();
    plinks = tmpset.toList();

    QMutableListIterator<IProcessNode *>  node_it(pnodes);
    QMutableListIterator<IPortLink *>     link_it(plinks);

    while (link_it.hasNext()) {
        QGraphicsItem* ni   = link_it.next();
        IPortLink*     link = 0;
        link = dynamic_cast<IPortLink *>(ni);

        if(link) {
            d->m_document->removePortLink(link);
            d->m_scene->removeItem(ni);
            delete link;
            link_it.remove();
            d->m_scene->update();
        }
    }

    while (node_it.hasNext()) {
        QGraphicsItem* ni   = node_it.next();
        IProcessNode*  node = 0;
        node = dynamic_cast<IProcessNode *>(ni);

        if(node) {
            d->m_scene->removeItem(ni);
            d->m_document->removeProcessNode(node);
            //            m_engine->removeProcessNode(node);
            delete node;
            node_it.remove();
            d->m_scene->update();
        }
    }

    //    qDeleteAll(items);
}

// TODO: Implement this.
void LDesignerWidget::cut()
{
    //    IProcessNode *node = selectedProcessNode();
    //    if (!node)
    //        return;

    //    copy();
    //    delete node;
}

//void LDesigner::selectAll()
//{

//}

// TODO: When you implement this, remember that nodes have (must have) unique
// ids so, need to create new ids for the duplicates.
void LDesignerWidget::copy()
{
    // TODO: Implement this for the generic interface.
    //    IProcessNode *node = selectedProcessNode();
    //    if (!node)
    //        return;

    //    QString str = QString("Node%1%2%3%4")
    //                  .arg(node->textColor().name())
    //                  .arg(node->outlineColor().name())
    //                  .arg(node->backgroundColor().name())
    //                  .arg(node->text());
    //    QApplication::clipboard()->setText(str);
}

void LDesignerWidget::paste()
{
    // TODO: Implement this for the generic interface.
    //    QString str = QApplication::clipboard()->text();
    //    QStringList parts = str.split(" ");

    //    if (parts.count() >= 5 && parts.first() == "ProcessNode") {
    //        IProcessNode *node = new ProcessNode;
    //        node->setText(QStringList(parts.mid(4)).join(" "));
    //        node->setTextColor(QColor(parts[1]));
    //        node->setOutlineColor(QColor(parts[2]));
    //        node->setBackgroundColor(QColor(parts[3]));
    //        setupProcessNode(node);
    //    }
}

// TODO: This needs to return a bool
void LDesignerWidget::runDesign()
{
    if(!(d->m_engine.isNull())) {

        if(d->m_engine->isRunning()) {
            return;
        }

        if(d->m_engine->isPaused()) {
            resumeDesign();
            return;
        }

        // Not running, not paused
        d->m_engine->removeAllNodes();

        // Force saving current file before loading the flow
        bool status_saving = true;
        QString ligerFile = d->m_document->filePath();
        if(d->m_document->document()->isModified()) {
            QString* errorString = new QString();
            d->m_document->save(errorString, ligerFile, false);
            d->m_document->reload(errorString);
        }
        if(status_saving) {
            bool status = d->m_engine->loadDesign(d->m_document->processNodes(),
                                                  ligerFile);
            //bool status = d->m_engine->loadDesign(d->m_document->processNodes(),
            //                                      d->m_document->portLinks());
            if(status) {
                d->m_engine->start();
            } else {
                QMessageBox::information(this, "Optimization Engine", "Cannot run current workflow");
                return;
            }
        } else {
            // Cannot save file
            QMessageBox::information(this, "Optimization Engine", "Cannot save current workflow");
            return;
        }
    } else {
        QMessageBox::information(this,"Optimization Engine", "No optimization engine found.");
    }
}

void LDesignerWidget::pauseDesign()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    qDebug() << "Pausing";
    d->m_engine->pause();
    qDebug() << "Paused";
    QApplication::restoreOverrideCursor();
}

void LDesignerWidget::resumeDesign()
{
    d->m_engine->resume();
}

void LDesignerWidget::stopDesign()
{
    if(!d->m_engine.isNull()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        d->m_engine->exit();
        QApplication::restoreOverrideCursor();
    }
}

void LDesignerWidget::updateActions()
{
    bool hasSelection = !d->m_scene->selectedItems().isEmpty();
    bool isNode       = (selectedProcessNode() != 0);
    bool isNodePair   = (selectedProcessNodePair() != ProcessNodePair());

    d->m_cutAction->setEnabled(isNode);
    d->m_copyAction->setEnabled(isNode);
    d->m_addLinkAction->setEnabled(isNodePair);
    d->m_deleteAction->setEnabled(hasSelection);
    d->m_bringToFrontAction->setEnabled(isNode);
    d->m_sendToBackAction->setEnabled(isNode);
    d->m_propertiesAction->setEnabled(isNode);

    foreach (QAction *action, d->m_view->actions())
        d->m_view->removeAction(action);

    foreach (QAction *action, d->m_editMenu->menu()->actions()) {
        if (action->isEnabled())
            d->m_view->addAction(action);
    }
}

void LDesignerWidget::createMenus()
{
    // This creates the menus every time a new file is opened... This is
    // definitely NOT correct. Legacy code where there existed only a signleton
    // of this class. Needs to be amended and code here be migrated appropriately.
    Core::Context designerContext("Liger.Designer");
    //    Core::EditorManager* cdm = Core::EditorManager::instance();
    Core::ActionContainer *menubar = Core::ActionManager::createMenuBar("Liger.Designer.MenuBar");
    menubar->appendGroup("Liger.Designer.Group.Edit");

    d->m_editMenu = Core::ActionManager::createMenu("Liger.Designer.Menu.Edit");

    menubar->addMenu(d->m_editMenu, "Liger.Designer.Group.Edit");
    d->m_editMenu->menu()->setTitle("Edit Nodes");

    d->m_editMenu->appendGroup("Liger.Designer.Group.Edit.AddNode");
    d->m_editMenu->appendGroup("Liger.Designer.Group.Edit.AddLink");
    d->m_editMenu->appendGroup("Liger.Designer.Group.Edit.Delete");
    d->m_editMenu->appendGroup("Liger.Designer.Group.Edit.Cut");
    d->m_editMenu->appendGroup("Liger.Designer.Group.Edit.Copy");
    d->m_editMenu->appendGroup("Liger.Designer.Group.Edit.Paste");
    d->m_editMenu->appendGroup("Liger.Designer.Group.Edit.BringToFront");
    d->m_editMenu->appendGroup("Liger.Designer.Group.Edit.SendToBack");
    d->m_editMenu->appendGroup("Liger.Designer.Group.Edit.Properties");
    d->m_editMenu->appendGroup("Liger.Designer.Menu.Edit");

    Core::Command* cmd;
    //    d->m_addNodeAction = new QAction(tr("Add &Node"), this);
    //    cmd = Core::ActionManager::registerAction(d->m_addNodeAction, "Liger.Designer.AddNode", designerContext);
    //    d->m_editMenu->addAction(cmd, "Liger.Designer.Menu.Edit");
    //    d->m_addNodeAction->setIcon(QIcon(":/designer/images/node.png"));
    //    d->m_addNodeAction->setShortcut(tr("Ctrl+N"));
    //    connect(d->m_addNodeAction, SIGNAL(triggered()), this, SLOT(addProcessNode()));

    //    d->m_addLinkAction = new QAction(tr("Add &Link"), this);
    //    cmd = Core::ActionManager::registerAction(d->m_addLinkAction, "Liger.Designer.AddLink",designerContext);
    //    d->m_editMenu->addAction(cmd, "Liger.Designer.Group.Edit.AddLink");
    //    m_editMenu->addAction(cmd, "Liger.Designer.Menu.Edit.AddLink");
    //    d->m_addLinkAction->setIcon(QIcon(":/designer/images/link.png"));
    //    d->m_addLinkAction->setShortcut(tr("Ctrl+L"));
    //    Core::ActionManager::instance()->actionContainer(Core::Constants::M_TOOLS)->addAction(cmd);

    d->m_deleteAction = new QAction(tr("&Delete"), this);
    cmd = Core::ActionManager::registerAction(d->m_deleteAction, "Liger.Designer.Delete",designerContext);
    d->m_editMenu->addAction(cmd, "Liger.Designer.Group.Edit.Delete");
    d->m_deleteAction->setIcon(QIcon(":/designer/images/delete.png"));
    d->m_deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));
    connect(d->m_deleteAction, SIGNAL(triggered()), this, SLOT(del()));

    // TODO: DESIGNER ACTIONS
    //    connect(cdm->currentEditorView(), SIGNAL(deleteDesignClicked()), this, SLOT(del()));

    // TODO: DESIGNER ACTIONS
    d->m_deleteAction = new QAction(tr("&Delete"), this);
    //    cmd = am->registerAction(m_deleteAction, Constants::ACTION_ID,
    //                             Core::Context(Core::Constants::C_GLOBAL));
#if defined(Q_OS_MAC)
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Backspace")));
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    cmd->setDefaultKeySequence(QKeySequence(Qt::Key_Delete));
#endif
    connect(d->m_deleteAction, SIGNAL(triggered()), this, SLOT(del()));

    // TODO: DESIGNER ACTIONS
    //    Core::ActionContainer *menu = am->createMenu(Constants::MENU_ID);
    //    menu->menu()->setTitle(tr("Nodes"));
    //    menu->addAction(cmd);
    //    am->actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    //    m_cutAction = new QAction(tr("Cu&t"), this);
    //    cmd = Core::ActionManager::registerAction(m_cutAction, "Liger.Designer.Cut",designerContext);
    //    m_editMenu->addAction(cmd, "Liger.Designer.Group.Edit.Cut");
    //    m_cutAction->setIcon(QIcon(":/designer/images/cut.png"));
    //    m_cutAction->setShortcut(tr("Ctrl+X"));
    //    connect(m_cutAction, SIGNAL(triggered()), this, SLOT(cut()));
    // TODO: DESIGNER ACTIONS
    //    connect(cdm->currentEditorView(), SIGNAL(cutDesignClicked()), this, SLOT(cut()));


    //    m_copyAction = new QAction(tr("&Copy"), this);
    //    cmd = Core::ActionManager::registerAction(m_copyAction, "Liger.Designer.Copy",designerContext);
    //    m_editMenu->addAction(cmd, "Liger.Designer.Group.Edit.Copy");
    //    m_copyAction->setIcon(QIcon(":/designer/images/copy.png"));
    //    m_copyAction->setShortcut(tr("Ctrl+C"));
    //    connect(m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    // TODO: DESIGNER ACTIONS
    //    connect(cdm->currentEditorView(), SIGNAL(copyDesignClicked()), this, SLOT(copy()));

    //    m_pasteAction = new QAction(tr("&Paste"), this);
    //    cmd = Core::ActionManager::registerAction(m_pasteAction, "Liger.Designer.Paste",designerContext);
    //    m_editMenu->addAction(cmd, "Liger.Designer.Group.Edit.Paste");
    //    m_pasteAction->setIcon(QIcon(":/designer/images/paste.png"));
    //    m_pasteAction->setShortcut(tr("Ctrl+V"));
    //    connect(m_pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    // TODO: DESIGNER ACTIONS
    //    connect(cdm->currentEditorView(), SIGNAL(pasteDesignClicked()), this, SLOT(paste()));
    //    connect(cdm->currentEditorView(), SIGNAL(runDesignClicked()),  this, SLOT(runDesign()));
    //    connect(cdm->currentEditorView(), SIGNAL(stopDesignClicked()), this, SLOT(stopDesign()));

    d->m_bringToFrontAction = new QAction(tr("Bring to &Front"), this);
    cmd = Core::ActionManager::registerAction(d->m_bringToFrontAction, "Liger.Designer.BringToFront",designerContext);
    d->m_editMenu->addAction(cmd, "Liger.Designer.Group.Edit.BringToFront");
    d->m_bringToFrontAction->setIcon(QIcon(":/designer/images/bringtofront.png"));
    connect(d->m_bringToFrontAction, SIGNAL(triggered()),
            this, SLOT(bringToFront()));

    d->m_sendToBackAction = new QAction(tr("&Send to Back"), this);
    cmd = Core::ActionManager::registerAction(d->m_sendToBackAction, "Liger.Designer.SendToBack",designerContext);
    d->m_editMenu->addAction(cmd, "Liger.Designer.Group.Edit.SendToBack");
    d->m_sendToBackAction->setIcon(QIcon(":/designer/images/sendtoback.png"));
    connect(d->m_sendToBackAction, SIGNAL(triggered()),
            this, SLOT(sendToBack()));

    d->m_propertiesAction = new QAction(tr("P&roperties..."), this);
    cmd = Core::ActionManager::registerAction(d->m_propertiesAction, "Liger.Designer.Properties",designerContext);
    d->m_editMenu->addAction(cmd, "Liger.Designer.Group.Edit.Properties");
    connect(d->m_propertiesAction, SIGNAL(triggered()),
            this, SLOT(properties()));
}

void LDesignerWidget::createToolBars()
{
    //    m_editToolBar->addAction(m_addNodeAction);
    //    m_editToolBar->addAction(m_addLinkAction);

    //    m_editToolBar->addAction(m_deleteAction);
    //   m_editToolBar->addSeparator();
    //    m_editToolBar->addAction(m_cutAction);
    //    m_editToolBar->addAction(m_copyAction);
    //    m_editToolBar->addAction(m_pasteAction);
    //    m_editToolBar->addSeparator();

    //    m_editToolBar->addAction(m_bringToFrontAction);
    //    m_editToolBar->addAction(m_sendToBackAction);
}


void LDesignerWidget::setZValue(int z)
{
    IProcessNode *node = selectedProcessNode();
    if (node) {
        node->setZValue(z);
    }
}

void LDesignerWidget::setupPortLink(IPortLink* link)
{
    d->m_document->addPortLink(link);
    link->setColor(d->m_lineColor);
    d->m_scene->addItem(link);
    link->trackPorts();
    //link->trackPortsAvoid(d->m_document->processNodes());
    d->m_scene->update();
    bringToFront();
}

void LDesignerWidget::setupProcessNode(IProcessNode *node)
{
    //d->m_document->addProcessNode(node); no longer needed
    d->m_scene->addItem(node);
    ++d->m_seqNumber;
    d->m_scene->clearSelection();
    node->setSelected(true);

    // notify ldesigner when data changed in operators.
    connect(node, SIGNAL(nodePropertiesChangedSignal()),
            this->DesignDocument()->document(), SLOT(setModified()));

    bringToFront();
}

void LDesignerWidget::zoom(qreal scaleFactor)
{
    d->m_view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    qreal widthAfterScaling = d->m_view->transform()
            .scale(scaleFactor, scaleFactor)
            .mapRect(QRectF(0, 0, 1, 1)).width();
    if (widthAfterScaling < 0.07 || widthAfterScaling > 100) {
        return;
    }
    d->m_view->scale(scaleFactor, scaleFactor);
}

IProcessNodeFactory* LDesignerWidget::selectedProcessNodeFactory() const
{
    IProcessNodeFactory* selectedPf = 0;
    QList<IProcessNodeFactory*> process_factories =
            ExtensionSystem::PluginManager::instance()->getObjects<IProcessNodeFactory>();
    Q_FOREACH(IProcessNodeFactory* pf, process_factories) {
        if(pf->isSelected()) {
            selectedPf = pf;
            break;
        }
    }
    return selectedPf;
}

IProcessNode *LDesignerWidget::selectedProcessNode() const
{
    QList<QGraphicsItem *> items = d->m_scene->selectedItems();
    if (items.count() == 1) {
        return dynamic_cast<IProcessNode *>(items.first());
    } else {
        return 0;
    }
}

IPort* LDesignerWidget::selectedPort() const
{
    QList<QGraphicsItem *> items = d->m_scene->selectedItems();
    if (items.count() == 1) {
        return dynamic_cast<IPort*>(items.first());
    } else {
        return 0;
    }
}

IPortLink *LDesignerWidget::selectedPortLink() const
{
    QList<QGraphicsItem *> items = d->m_scene->selectedItems();
    if (items.count() == 1) {
        return dynamic_cast<IPortLink *>(items.first());
    } else {
        return 0;
    }
}

LDesignerWidget::ProcessNodePair LDesignerWidget::selectedProcessNodePair() const
{
    //    QList<QGraphicsItem *> items = m_scene->selectedItems();
    //    if (items.count() == 2) {
    //        ProcessNode *first = dynamic_cast<ProcessNode *>(items.first());
    //        ProcessNode *second = dynamic_cast<ProcessNode *>(items.last());
    //        if (first && second)
    //            return ProcessNodePair(first, second);
    //    }
    return ProcessNodePair();
}

void LDesignerWidget::bringToFront()
{
    ++d->m_maxZ;
    setZValue(d->m_maxZ);
}

void LDesignerWidget::sendToBack()
{
    --d->m_minZ;
    setZValue(d->m_minZ);
}

void LDesignerWidget::properties()
{
    IProcessNode *node = selectedProcessNode();
    IPortLink *link = selectedPortLink();
    Q_UNUSED(node)
    Q_UNUSED(link)
    //TODO: Migrate the PropertiesDialog
    //    if (node) {
    //        PropertiesDialog dialog(node, this);
    //        dialog.exec();
    //    } else if (link) {
    //        QColor color = QColorDialog::getColor(link->color(), this);
    //        if (color.isValid())
    //            link->setColor(color);
    //    }
}


void LDesignerWidget::configure()
{
    Core::MimeType mimeType;

    if (designerDocument())
        mimeType = Core::MimeDatabase::findByFile(designerDocument()->filePath());

    configure(mimeType);
}

void LDesignerWidget::configure(const QString &mimeType)
{
    configure(Core::MimeDatabase::findByType(mimeType));
}

void LDesignerWidget::configure(const Core::MimeType &mimeType)
{
    if (!mimeType.isNull()) {
        const QString &type = mimeType.type();
        setMimeType(type);
        //        QString definitionId = Manager::instance()->definitionIdByMimeType(type);
        //        if (definitionId.isEmpty())
        //            definitionId = findDefinitionId(mimeType, true);
    }

    emit configured(designer());
}

QString LDesignerWidget::findDefinitionId(const Core::MimeType &mimeType,
                                          bool considerParents) const
{
    Q_UNUSED(mimeType)
    Q_UNUSED(considerParents)
    //    QString definitionId = Manager::instance()->definitionIdByAnyMimeType(mimeType.aliases());
    //    if (definitionId.isEmpty() && considerParents) {
    //        definitionId = Manager::instance()->definitionIdByAnyMimeType(mimeType.subClassesOf());
    //        if (definitionId.isEmpty()) {
    //            foreach (const QString &parent, mimeType.subClassesOf()) {
    //                const Core::MimeType &parentMimeType =
    //                    Core::ICore::mimeDatabase()->findByType(parent);
    //                definitionId = findDefinitionId(parentMimeType, considerParents);
    //            }
    //        }
    //    }
    //    return definitionId;
    return 0;
}

} // namespace Designer
