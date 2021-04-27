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
#include <tigon/Random/RandomGenerator.h>

using namespace boost::random;

namespace Tigon {

RandomGenerator::RandomGenerator()
    : m_mutex(),
      m_prng(new mt19937(0)),
      m_randUni(m_prng, uniform_real_distribution<double>(0.0, 1.0)),
      m_randNorm(m_prng, normal_distribution<double>(0.0, 1.0))
{
    defineSeed(0);
}

RandomGenerator::~RandomGenerator()
{
    delete m_prng;
    m_prng = 0;
}

RandomGenerator& RandomGenerator::instance()
{
    static RandomGenerator singleRandGen;
    return singleRandGen;
}

void RandomGenerator::defineSeed(uint seed)
{
    m_mutex.lock();
    m_seed = seed;
    reset();
    m_mutex.unlock();
}

uint RandomGenerator::seed()
{
    m_mutex.lock();
    uint seed = m_seed;
    m_mutex.unlock();
    return seed;
}

void RandomGenerator::reset()
{
    m_mutex.lock();
    m_prng->seed(m_seed);
//    m_randUni.engine().seed(m_seed);
//    m_randUni.distribution().reset();
//    m_randNorm.engine().seed(m_seed);
//    m_randNorm.distribution().reset();
    m_state = 0;
    m_mutex.unlock();
}

uint RandomGenerator::currentState()
{
    m_mutex.lock();
    uint state = m_state;
    m_mutex.unlock();
    return state;
}

void RandomGenerator::defineState(uint state)
{
    m_mutex.lock();
    reset();
    for(m_state=0; m_state<state; m_state++) {
        (*m_prng)();
    }
    m_mutex.unlock();
}

int RandomGenerator::randInt(int nValues, int begin)
{
    if(nValues <= 0) {
        // TODO - Throw error
        return -1;
    }
    m_mutex.lock();
    m_state++;
    int ret = begin + (*m_prng)() % nValues;
    m_mutex.unlock();
    return ret;
}

double RandomGenerator::randUni(double range, double begin)
{
    m_mutex.lock();
    m_state++;
    double ret = begin + m_randUni() * range;
    m_mutex.unlock();
    return ret;
}

double RandomGenerator::randNorm(double mu, double sigma)
{
    m_mutex.lock();
    // boost::normal_distribution calls the prng twice
    m_state += 2;
    double ret =  mu + m_randNorm() * sigma;
    m_mutex.unlock();
    return ret;
}

double RandomGenerator::randChiSquared(double dof)
{
    if(dof<=0.0) {
        // ERROR
        dof = 1.0;
    }
    m_mutex.lock();
    // boost::chi_squared_distribution does not call the prng the same amount
    // of times for every sample. Therefore, the m_state is not advanced
//    m_state++;
    boost::random::variate_generator<boost::random::mt19937*,
    boost::random::chi_squared_distribution<double> > randChiSquared(
                m_prng, chi_squared_distribution<double>(dof));
    double ret =  randChiSquared.operator ()();
    m_mutex.unlock();
    return ret;
}

} // namespace Tigon
