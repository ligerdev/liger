/****************************************************************************
**
** Copyright (C) 2012-2013 The University of Sheffield (www.sheffield.ac.uk)
**
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
#ifndef {% filter upper %}{{StringName}}{% endfilter %}FORM_H
#define {% filter upper %}{{StringName}}{% endfilter %}FORM_H

#include <QMainWindow>
#include <QWebView>

namespace Ui {
class {{StringName}}Form;
}


namespace {{Namespace}} {

class {{StringName}}Form : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit {{StringName}}Form(QWidget *parent = 0);
    ~{{StringName}}Form();

    QWebView* webView();

private slots:
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();

private:
    Ui::{{StringName}}Form *ui;
    QWebView* m_webview;
};

} // namespace {{Namespace}}

#endif // {% filter upper %}{{StringName}}{% endfilter %}FORM_H
