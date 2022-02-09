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
#include <matlabplugin/Utils/MatlabPool.h>

#ifdef MATLAB_API_C
#include <matlabplugin/Utils/MatlabEngineC.h>
#endif
#ifdef MATLAB_API_CPP
#include <matlabplugin/Utils/MatlabEngineX.h>
#endif

#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
static bool ms_useMatlab = false;
/*!
 * \brief Provides access to the pool
 *
 * Returns a pointer to the singleton instance of MatlabPool. Creates the new pool if this is the first call of this operation.
 * \return A reference to the singleton MatlabPool
 */
MatlabPool& MatlabPool::getInstance()
{
    static MatlabPool singleInstance;
    ms_useMatlab = true;
    return singleInstance;
}

bool MatlabPool::useMatlab()
{
    return ms_useMatlab;
}

/*!
 * \brief Private constructor for the pool.
 *
 * Constructor defines the initial pool size.
 */
MatlabPool::MatlabPool()
    : m_mutex()
    , m_maxUnlocked(2)

{
    // Avoid resizing the vectors later.
    // Realistically there should never be this many instances of MATLAB running
    m_unlocked.reserve(5);
    m_locked.reserve(5);
}

/*!
 * \brief MatlabPool destructor
 *
 * Cleans up the pool, deleting all managed instances of MatlabEngine.
 * This should be the one of the last destructors to run in the
 * Tigon application. The pool will only be destroyed when the application is closed,
 * therefore it is safe to destroy all <MatlabEngine>s.
 */
MatlabPool::~MatlabPool()
{
    clearPointerVector(m_unlocked);
    clearPointerVector(m_locked);
}

/*!
 * \brief Get a MatlabEngine.
 *
 * Returns a reference to a free MatlabEngine. If no free engine exists, one is created.
 *
 * \return A clean MatlabEngine for use
 */
IMatlabEngine* MatlabPool::aquireEngine()
{
    IMatlabEngine* eng = nullptr;
    m_mutex.lock();

    if(m_unlocked.size() > 0) {
        // Engines are available, use one of them.
        m_locked.push_back(m_unlocked.front());
        m_unlocked.erase(m_unlocked.begin());
        eng = m_locked.back();
    } else {
        // If I can make a new one do that.
#ifdef MATLAB_API_C
        m_locked.push_back(new MatlabEngineC());
#endif
#ifdef MATLAB_API_CPP
        m_locked.push_back(new MatlabEngineX());
#endif
        eng = m_locked.back();
    }

    m_mutex.unlock();
    return eng;
}

/*!
 * \brief Call this to return a MatlabEngine back to the pool.
 *
 * The MatlabEngine being returned is added to the <unlocked> storage or
 * deleted depending on the allowed size of the free pool.
 * \param[in] eng   The MatlabEngine being returned.
 */
void MatlabPool::releaseEngine(IMatlabEngine* eng)
{
    m_mutex.lock();

    eng->resetEngine(); // Resetting the engine to a clean state

    bool found = false;
    for(size_t i=0; i < m_locked.size(); i++) {
        if(eng == m_locked[i]) {
            found = true;
            if(m_unlocked.size() == m_maxUnlocked) {
                // Already have enough spare engines
                delete eng;
                m_locked.erase(m_locked.begin()+static_cast<int>(i));
            } else {
                m_unlocked.push_back(eng);
                m_locked.erase(m_locked.begin()+static_cast<int>(i));
            }
        }
    }

    if(!found) {
        // Lost engine so delete anyway
        delete eng;
    }

    m_mutex.unlock();
}

/*!
 * \brief // Clears resources held by the pool
 *
 * Clears resources held in the unlocked pool, leaves the locked pool untouched. This must be called
 * before the destructor of this class. This performs the clean up operations the destructor should do.
 *
 * MATLAB's API (specifically engClose) hangs if calls are made during the program closing. Also to
 * note is that sending the command "force quit" to MATLAB also causes the API to hang.
 *
 */
void MatlabPool::emptyPool()
{
    for(size_t i=0; i<m_unlocked.size(); i++) {
        m_unlocked.at(i)->closeEngine();
    }
    clearPointerVector(m_unlocked);

    for(size_t i=0; i<m_locked.size(); i++) {
        m_locked.at(i)->closeEngine();
    }
    clearPointerVector(m_locked);
    ms_useMatlab = false;
}

} // namespace Tigon
