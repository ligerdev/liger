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
#include <qtigon/qtigonutils.h>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>
#include <QMetaProperty>

using namespace Tigon;

namespace QTigon {

bool assignPropertyToObject(TObject *object, QXmlStreamReader *xmlReader)
{
    bool res = false;
    if(object) {
        TString name = xmlReader->name().toString().toStdString();
        TString value = xmlReader->readElementText().toStdString();
        if(!(name.empty() || value.empty())) {
            object->setProperty(name, value);
            res = true;
        }
    }
    return res;
}

void propertiesToXml(TObject *object, QXmlStreamWriter *xmlWriter)
{
    int count = object->propertyCount();
    TStringList pnames = object->propertyNames();
    for(int i=0; i<count; i++) {
        TString name = pnames[i];
        TString pvalue = object->propertyValue(name);
        if (!pvalue.empty()) {
            xmlWriter->writeTextElement(QString::fromStdString(name),
                                        QString::fromStdString(pvalue));
        }
    }
}

}
