/****************************************************************************
**
** Copyright (C) 2012-2017 The University of Sheffield (www.sheffield.ac.uk)
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
#pragma once

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

#include <mutex>

namespace Tigon {
class MatlabEngine;
/*!
 * \brief A resource pool of <MatlabEngine>'s
 *
 * The class implements the object pool pattern
 * (see <a href="https://sourcemaking.com/design_patterns/object_pool">here</a>)
 * for <MatlabEngine>s.
 * This class is implemented as a singleton with access being granted via the
 * static <getPoolInstance()> function and is thread-safe.
 *
 * A user of this class can request an instance of MatlabEngine via the acquireEngine() function.
 * If no MatlabEngine is available then a new one is created.
 * The function is guaranteed to provide the user with a reference to an available MatlabEngine.
 * New <MatlabEngine>s are created if no available engines exist.
 * A user of this class is responsible for releasing the MatlabEngine when
 * access is no longer required via the <releaseEngine()> operation.
 */
class LIGER_TIGON_EXPORT MatlabPool
{
public:
    void defineMaxPoolSize(int size);
    static MatlabPool& getInstance(); // Meyers singleton
    static bool useMatlab();
    MatlabEngine* aquireEngine();
    void releaseEngine(MatlabEngine* eng);
    void emptyPool();

    ~MatlabPool();

private:
    MatlabPool();

    TVector<MatlabEngine* > m_unlocked; //!< Holds free <MatlabEngine>s to be used.
    TVector<MatlabEngine* > m_locked;  //!< Holds <MatlabEngine>s currently being used.
    std::mutex m_mutex;
    int m_maxUnlocked;
};

} // Tigon
