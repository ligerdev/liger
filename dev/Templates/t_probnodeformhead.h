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
#ifndef {% filter upper %}{{ ClassName }}{% endfilter %}FORM_H
#define {% filter upper %}{{ ClassName }}{% endfilter %}FORM_H

#include <{{ plugin_name }}_global.h>
#include <QWidget>
#include <QDialogButtonBox>

namespace Ui {
class {{ ClassName }}Form;
}

namespace {{ Namespace }} {

class {% filter upper %}{{ plugin_name }}{% endfilter %}_EXPORT {{ ClassName }}Form : public QWidget
{
    Q_OBJECT
    
public:
    explicit {{ ClassName }}Form(QWidget *parent = 0);
    ~{{ ClassName }}Form();

    qint32 nvar();
    qint32 nobj();

    QDialogButtonBox* m_buttonBox;   

    void nobjEditEnabled(bool b);
    void nvarEditEnabled(bool b);
    void setNvar(qint32 nvar);
    void setNobj(qint32 nobj); 

public slots:
     void updateParameters();

private slots:
     void nobjEditTextChanged();
     void nvarEditTextChanged();     

private:
    qint32 p_nvar();
    qint32 p_nobj();

    
    qint32 m_nvar;
    qint32 m_nobj;

    Ui::{{ ClassName }}Form *ui;
};

} // namespace {{ Namespace }}

#endif // {% filter upper %}{{ ClassName }}{% endfilter %}FORM_H
