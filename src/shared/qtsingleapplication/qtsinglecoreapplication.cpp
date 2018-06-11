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
** This is a modified version of qtsinglecoreapplication.cpp from Qt Creator version 2.6.81
** Filename: qtsinglecoreapplication.cpp 
** Path:     QTC_ROOT/src/shared/qtsingleapplication 
** Date:     11/11/2012
**
****************************************************************************/

#include "qtsinglecoreapplication.h"
#include "qtlocalpeer.h"

namespace SharedTools {

QtSingleCoreApplication::QtSingleCoreApplication(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    peer = new QtLocalPeer(this);
    connect(peer, SIGNAL(messageReceived(QString)), SIGNAL(messageReceived(QString)));
}


QtSingleCoreApplication::QtSingleCoreApplication(const QString &appId, int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    peer = new QtLocalPeer(this, appId);
    connect(peer, SIGNAL(messageReceived(QString)), SIGNAL(messageReceived(QString)));
}


bool QtSingleCoreApplication::isRunning()
{
    return peer->isClient();
}


bool QtSingleCoreApplication::sendMessage(const QString &message, int timeout)
{
    return peer->sendMessage(message, timeout);
}


QString QtSingleCoreApplication::id() const
{
    return peer->applicationId();
}

} // namespace SharedTools
