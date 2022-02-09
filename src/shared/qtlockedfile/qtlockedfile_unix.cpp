/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
** This is a modified version of qtlockedfile_unix.cpp from Qt Creator version 2.6.81
** Filename: qtlockedfile_unix.cpp 
** Path:     QTC_ROOT/src/shared/qtlockedfile 
** Date:     11/11/2012
**
****************************************************************************/

#include "qtlockedfile.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

namespace SharedTools {

bool QtLockedFile::lock(LockMode mode, bool block)
{
    if (!isOpen()) {
        qWarning("QtLockedFile::lock(): file is not opened");
        return false;
    }

    if (mode == NoLock)
        return unlock();

    if (mode == m_lock_mode)
        return true;

    if (m_lock_mode != NoLock)
        unlock();

    struct flock fl;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_type = (mode == ReadLock) ? F_RDLCK : F_WRLCK;
    int cmd = block ? F_SETLKW : F_SETLK;
    int ret = fcntl(handle(), cmd, &fl);

    if (ret == -1) {
        if (errno != EINTR && errno != EAGAIN)
            qWarning("QtLockedFile::lock(): fcntl: %s", strerror(errno));
        return false;
    }


    m_lock_mode = mode;
    return true;
}


bool QtLockedFile::unlock()
{
    if (!isOpen()) {
        qWarning("QtLockedFile::unlock(): file is not opened");
        return false;
    }

    if (!isLocked())
        return true;

    struct flock fl;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_type = F_UNLCK;
    int ret = fcntl(handle(), F_SETLKW, &fl);

    if (ret == -1) {
        qWarning("QtLockedFile::lock(): fcntl: %s", strerror(errno));
        return false;
    }

    m_lock_mode = NoLock;
    remove();
    return true;
}

QtLockedFile::~QtLockedFile()
{
    if (isOpen())
        unlock();
}

} // namespace SharedTools
