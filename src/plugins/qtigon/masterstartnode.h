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
#pragma once

#include <qtigon/qtigon_global.h>
#include <designer/iengine/iprocessnode.h>

#include <QObject>

class StartNodeDialog;

namespace QTigon {
/*!
 * \brief The MasterStartNode class contains PSetBase;
 */
class QTIGON_EXPORT MasterStartNode : public Designer::IProcessNode
{
    Q_OBJECT

    Q_PROPERTY(bool isUserDefinedSeed READ userDefinedSeed WRITE setUserDefinedSeed)
    Q_PROPERTY(uint userDefinedSeed READ seed WRITE setSeed)

public:
    MasterStartNode();
    ~MasterStartNode();

    bool userDefinedSeed() const;
    void setUserDefinedSeed(bool userDefinedSeed);

    uint seed() const;
    void setSeed(uint seed);

private slots:
    void acceptChangesSlot();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void readDataProperties(QXmlStreamReader &xmlReader);
    void writeDataProperties(QXmlStreamWriter &xmlWriter);

private:
    StartNodeDialog* m_dialog;
    bool m_userDefinedSeed;
    uint m_seed;
};

} // namespace QTigon
