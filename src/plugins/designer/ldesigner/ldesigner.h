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
#ifndef LDESIGNER_H
#define LDESIGNER_H

#include <designer/ldesigner/ldesignerwidget.h>
#include <coreplugin/editormanager/ieditor.h>

namespace Core {
class MimeType;
}

namespace Designer {

class LDesignerWidget;

class DESIGNER_EXPORT LDesigner : public Core::IEditor
{
    Q_OBJECT

public:
    enum Side {
        Left,
        Right
    };

    LDesigner(LDesignerWidget *);
    ~LDesigner();

    friend class LDesignerWidget;

    bool open(QString *errorString, const QString &fileName, const QString &realFileName);
    Core::IDocument* document();

    QWidget* toolBar();

    bool isTemporary() const;
    Core::Id id() const;

    LDesignerWidget *widget() const;

    // This returns true if the m_scene it the LDesignerWidget has focus.
    bool hasFocus() const;
    bool isActive() const;

    QString displayName() const;
    void setDisplayName(const QString &title);

    void insertExtraToolBarWidget(Side side, QWidget *widget);

    // IDesigner
    QString contextHelpId() const; // from IContext

signals:
    void contentsChanged();
    void contentsChangedBecauseOfUndo();
    void markContextMenuRequested(LDesigner *designer, int line, QMenu *menu);
    void tooltipOverrideRequested(LDesigner *designer, const QPoint &globalPos, int position, bool *handled);
    void tooltipRequested(LDesigner *designer, const QPoint &globalPos, int position);
    void markTooltipRequested(LDesigner *designer, const QPoint &globalPos, int line);
    void contextHelpIdRequested(LDesigner *designer, int position);

public slots:
    bool loadDesign();

private:
    LDesignerWidget* e;
    QToolBar*        m_toolBar;
    QWidget*         m_stretchWidget;

};

} // namespace Designer

#endif // LDESIGNER_H
