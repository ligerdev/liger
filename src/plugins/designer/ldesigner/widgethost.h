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
#ifndef WIDGETHOST_H
#define WIDGETHOST_H

#include <QScrollArea>
#include <ldesigner/ldesigner.h>

namespace SharedTools {

namespace Internal { class DesignerWidgetContainer; }

/* A scroll area that embeds a Designer form window */

class DESIGNER_EXPORT WidgetHost : public QScrollArea
{
    Q_OBJECT
public:
    explicit WidgetHost(QWidget *parent = 0, Designer::LDesigner *formWindow = 0);
    virtual ~WidgetHost();   

    inline Designer::LDesigner *designWindow() const { return m_designWindow; }

    QWidget *integrationContainer() const;

protected:
    void setFormWindow(Designer::LDesigner *fw);

private:
    QSize formWindowSize() const;

    Designer::LDesigner *m_designWindow;
    Internal::DesignerWidgetContainer *m_designerWidgetContainer;
    QSize m_oldFakeWidgetSize;
};

} // namespace SharedTools

#endif // WIDGETHOST_H
