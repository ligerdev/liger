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
#ifndef LDESIGNERWIDGET_H
#define LDESIGNERWIDGET_H

#include <designer/designer_global.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/editormanager/editormanager.h>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QToolBar>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPlainTextEdit>
#include <QMutex>

// Suppressing hiding-overloaded-virtual-function-warning
// This is done on purpose.
#if defined(__clang__)
  #pragma clang diagnostic ignored "-Woverloaded-virtual" // -Woverloaded-virtual
#endif

QT_BEGIN_NAMESPACE
class QToolBar;
class QTimeLine;
class QPrinter;
class QColor;
QT_END_NAMESPACE

namespace Utils {
    class LineColumnLabel;
    class ChangeSet;
}

namespace Designer {
struct RefactorMarker;
class LDesignerWidgetPrivate;
class IProcessNodeFactory;
class IProcessNode;
class IPort;
class IPortLink;
class IEngine;

namespace Internal {
    typedef QList<RefactorMarker> RefactorMarkers;
    typedef QString (QString::*TransformationMethod)() const;
}

class LDesignDocument;
class LDesigner;

class DESIGNER_EXPORT LDesignerAnimator : public QObject
{
    Q_OBJECT

public:
    LDesignerAnimator(QObject *parent);

    inline void setPosition(int position) { m_position = position; }
    inline int position() const { return m_position; }

    void setData(QFont f, QPalette pal, const QString &text);

    void draw(QPainter *p, const QPointF &pos);
    QRectF rect() const;

    inline qreal value() const { return m_value; }
    inline QPointF lastDrawPos() const { return m_lastDrawPos; }

    void finish();

    bool isRunning() const;

signals:
    void updateRequest(int position, QPointF lastPos, QRectF rect);

private slots:
    void step(qreal v);

private:
    QTimeLine *m_timeline;
    qreal m_value;
    int m_position;
    QPointF m_lastDrawPos;
    QFont m_font;
    QPalette m_palette;
    QString m_text;
    QSizeF m_size;
};

class DESIGNER_EXPORT LDesignerWidget : public QAbstractScrollArea
{
    Q_OBJECT

public:
    explicit LDesignerWidget(QWidget *parent);
    explicit LDesignerWidget(const QString &text, QWidget *parent = 0);
    ~LDesignerWidget();

    void configure(const QString& mimeType);
    void configure(const Core::MimeType &mimeType);
    void duplicateFrom(LDesignerWidget *designer);

    QWidget* extraArea() const;
    int  extraAreaWidth(int *markWidthPtr = 0) const;
    void extraAreaPaintEvent(QPaintEvent *);
    void extraAreaLeaveEvent(QEvent *);
    void extraAreaContextMenuEvent(QContextMenuEvent *);
    void extraAreaMouseEvent(QMouseEvent *);

    QString documentTitle();
    void setDocumentTitle(QString doc);

    QGraphicsScene*  scene() const;
    QGraphicsView*   getView();
    void setScene(QGraphicsScene* scene);

    IEngine* engine() const;

    const Utils::ChangeSet &changeSet() const;
    void setChangeSet(const Utils::ChangeSet &changeSet);

    // DesignerInterface
    Core::IDocument *designerDocument() const;
    virtual bool open(QString *errorString, const QString &fileName,
                      const QString &realFileName);
    QString displayName() const;

    // IDesigner
    LDesigner *designer() const;
    void setDesigner(LDesigner* designer);

    LDesignDocument *DesignDocument() const;
    void setDesignDocument(LDesignDocument *doc);

    void print(QPrinter *);

    void setSuggestedFileName(const QString &suggestedFileName);
    QString mimeType() const;
    virtual void setMimeType(const QString &mt);

    void appendMenuActionsFromContext(QMenu *menu, const Core::Id menuContextId);
    void appendStandardContextMenuActions(QMenu *menu);

    void setMouseNavigationEnabled(bool b);
    bool mouseNavigationEnabled() const;

    void setScrollWheelZoomingEnabled(bool b);
    bool scrollWheelZoomingEnabled() const;

    void setConstrainTooltips(bool b);
    bool constrainTooltips() const;

    QMimeData *duplicateMimeData(const QMimeData *source) const;

public slots:
    void setDisplayName(const QString &title);
    void del();
    void cut();
    void copy();
    void paste();
    void bringToFront();
    void sendToBack();
    void runDesign();
    void pauseDesign();
    void resumeDesign();
    void stopDesign();

    void zoomIn(qreal scaleFactor  = 1.1);
    void zoomOut(qreal scaleFactor = 0.9);
    void zoomReset();
    void fitToWindow(qreal scaleFactor = 0.75);

    void sceneModified();
    bool loadDesign();

signals:
    //void requestZoomReset();
    void configured(Core::IEditor *designer);
    void changed();

    // IDesigner
    void contentsChanged();
    void plainSceneChanged();
    void sceneChanged();
    void undoAvailable(bool b);
    void redoAvailable(bool b);
    void copyAvailable(bool b);
    void selectionChanged();

    void updateRequest(const QRect &rect, int dy);
    void modificationChanged(bool);

protected:
    QMimeData *createMimeDataFromSelection() const;
    bool canInsertFromMimeData(const QMimeData *source) const;
    void insertFromMimeData(const QMimeData *source);
    static QString msgTextTooLarge(quint64 size);
    void setDefaultPath(const QString &defaultPath);
    bool viewportEvent(QEvent *event);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent* mouseEvent);
    void mouseDoubleClickEvent(QMouseEvent *);
    void leaveEvent(QEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *e);
    void dragMoveEvent(QGraphicsSceneDragDropEvent* e);
    void dropEvent(QGraphicsSceneDragDropEvent* e);
    void showDefaultContextMenu(QContextMenuEvent *e, const Core::Id menuContextId);
    bool mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    bool mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    bool mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
    bool event(QEvent *e);
    void changeEvent(QEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    bool eventFilter(QObject *, QEvent *);
    void showEvent(QShowEvent *);

protected slots:
    virtual void slotUpdateExtraArea();
    virtual void slotUpdateExtraAreaWidth();
    virtual void slotModificationChanged(bool);
    virtual void slotUpdateRequest(const QRect &r, int dy);

private slots:
    void configure();
    void designerContentsChange(int position, int charsRemoved, int charsAdded);
    void properties();
    void updateActions();

private:
    typedef QPair<IProcessNode *, IProcessNode *> ProcessNodePair;
    void createMenus();
    void createToolBars();
    void setZValue(int z);
    void setupPortLink(IPortLink* link);
    void setupProcessNode(IProcessNode* node);
    void zoom(qreal scaleFactor);

    IProcessNodeFactory* selectedProcessNodeFactory() const;
    IProcessNode*   selectedProcessNode()     const;
    IPort*          selectedPort()            const;
    IPortLink*      selectedPortLink()        const;
    ProcessNodePair selectedProcessNodePair() const;

    QString findDefinitionId(const Core::MimeType &mimeType, bool considerParents) const;

    LDesignerWidgetPrivate *d;
};

} // namespace Designer


#endif // LDESIGNERWIDGET_H
