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

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

#include <mutex>

namespace Tigon {
class IMatlabEngine;

#ifdef MATLAB_API_C
class MatlabEngineC;
#endif

#ifdef MATLAB_API_CPP
class MatlabEngineX;
#endif
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
    IMatlabEngine* aquireEngine();
    void releaseEngine(IMatlabEngine* eng);
    void emptyPool();

    ~MatlabPool();

private:
    MatlabPool();

    TVector<IMatlabEngine*> m_unlocked; //!< Holds free <MatlabEngine>s to be used.
    TVector<IMatlabEngine*> m_locked;  //!< Holds <MatlabEngine>s currently being used.
    std::mutex m_mutex;
    size_t m_maxUnlocked;
};

} // Tigon
