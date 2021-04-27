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
#ifndef DESIGNERWIDGETCONTAINER_H
#define DESIGNERWIDGETCONTAINER_H

#include <ldesigner/ldesigner.h>
#include <QWidget>
#include <QVector>

QT_FORWARD_DECLARE_CLASS(QFrame)

namespace SharedTools {
namespace Internal {

class DESIGNER_EXPORT DesignerWidgetContainer : public QWidget
{
    Q_OBJECT
public:

    DesignerWidgetContainer(QWidget *parent = 0);
    ~DesignerWidgetContainer();

    void setFormWindow(Designer::LDesigner *fw);

private:
    QWidget *mainContainer();

    QFrame *m_frame;
    Designer::LDesigner * m_designWindow;
};

}
} // namespace SharedTools

#endif // DESIGNERWIDGETCONTAINER_H
