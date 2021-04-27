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
#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/chi_squared_distribution.hpp>

#include <tigon/tigon_global.h>
#include <mutex>

// Macro for static random number generator
#ifndef TRAND
#define TRAND Tigon::RandomGenerator::instance()
#endif

namespace Tigon {

class LIGER_TIGON_EXPORT RandomGenerator
{
public:
    static RandomGenerator& instance();

    void defineSeed(uint   seed);
    uint seed();
    void reset();
    uint currentState();
    void defineState(uint state);

    int    randInt(int nValues = 1,   int   begin =   0);
    double randUni(double range = 1.0, double begin = 0.0);
    double randNorm(double mu = 0.0,   double sigma = 1.0);
    double randChiSquared(double               dof = 1.0);

private:
    RandomGenerator();
    ~RandomGenerator();

    std::recursive_mutex m_mutex;

    uint m_seed;
    uint m_state;
    boost::random::mt19937* m_prng;

    // parametric distributions
    boost::random::variate_generator<boost::random::mt19937*,
    boost::random::uniform_real_distribution<double> >  m_randUni;
    boost::random::variate_generator<boost::random::mt19937*,
    boost::random::normal_distribution<double> >        m_randNorm;
};

} // namespace Tigon

#endif // RANDOMGENERATOR_H
