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
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/math/distributions/normal.hpp> // for normal_distribution

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/limits.hpp>
#include <boost/numeric/conversion/bounds.hpp>

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include <boost/variant.hpp>
using boost::variant;

#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;


/// Experiments
#include <string>
#include <tigon/Operators/Random/RGenerator.h>

// This is a typedef for a random number generator.
// Try boost::mt19937 or boost::ecuyer1988 instead of boost::minstd_rand
//typedef boost::minstd_rand base_generator_type;
typedef boost::random::mt19937 base_generator_type;

using namespace std;

class objectWuuid
{
public:
    objectWuuid()
        : tag(boost::uuids::random_generator()())
        , state(0)
    {}

    explicit objectWuuid(int state)
        : tag(boost::uuids::random_generator()())
        , state(state)
    {}

    objectWuuid(objectWuuid const& rhs)
        : tag(rhs.tag)
        , state(rhs.state)
    {}

    bool operator==(objectWuuid const& rhs) const {
        return tag == rhs.tag;
    }

    bool operator!=(objectWuuid const& rhs) const {
        return tag != rhs.tag;
    }

    objectWuuid& operator=(objectWuuid const& rhs) {
        tag = rhs.tag;
        state = rhs.state;
        return *this;
    }

    int get_state() const { return state; }
    void set_state(int new_state) { state = new_state; }
    void setTag(boost::uuids::uuid ta) { tag = ta; }
    boost::uuids::uuid getTag() { return tag; }
private:
    boost::uuids::uuid tag;
    int state;
};

void test_UUID()
{
    objectWuuid o1(1);
    objectWuuid o2 = o1;
    boost::uuids::string_generator string_gen;
    o2.set_state(2);
    cout << "o1 state: " << o1.get_state() << endl;
    cout << "o2 state: " << o2.get_state() << endl;

    cout << "o1 uuid : " << to_string(o1.getTag()) << endl;
    boost::uuids::uuid nid = string_gen(to_string(o1.getTag()));

    if(o1.getTag() == nid) {
        cout << "Yehaou" << endl;
    }
}

void test_BoostNumericalBounds()
{
    std::cout << "Boost::numeric::bounds versus numeric_limits example.\n";

    std::cout << "The maximum value for double:\n";
    std::cout << boost::numeric::bounds<double>::highest() << "\n";
    std::cout << std::numeric_limits<double>::max() << "\n";

    std::cout << "The minimum value for double:\n";
    std::cout << boost::numeric::bounds<double>::lowest() << "\n";
    std::cout << std::numeric_limits<double>::lowest() << "\n";

    std::cout << "The smallest positive value for double:\n";
    std::cout << boost::numeric::bounds<double>::smallest() << "\n";
    std::cout << std::numeric_limits<double>::min() << "\n";
}

void test_Distributions()
{
    double step = 1.; // in z
    double range = 4; // min and max z = -range to +range.
    int precision = 17; // traditional tables are only computed to much lower precision.
    // but std::numeric_limits<double>::max_digits10; on new Standard Libraries gives
    // 17, the maximum number of digits that can possibly be significant.
    // std::numeric_limits<double>::digits10; == 15 is number of guaranteed digits,
    // the other two digits being 'noisy'.

    // Construct a standard normal distribution s
    boost::math::normal s; // (default mean = zero, and standard deviation = unity)
    cout << "Standard normal distribution, mean = "<< s.mean()
         << ", standard deviation = " << s.standard_deviation() << endl;

    cout << "Probability distribution function values" << endl;
    cout << "  z " "      pdf " << endl;
    cout.precision(5);
    for (double z = -range; z < range + step; z += step)
    {
        cout << left << setprecision(3) << setw(6) << z << " "
             << setprecision(precision) << setw(12) << pdf(s, z) << endl;
    }
    cout.precision(6); // default

    // For a standard normal distribution
    cout << "Standard normal mean = "<< s.mean()
         << ", standard deviation = " << s.standard_deviation() << endl;
    cout << "Integral (area under the curve) from - infinity up to z " << endl;
    cout << "  z " "      cdf " << endl;
    for (double z = -range; z < range + step; z += step)
    {
        cout << left << setprecision(3) << setw(6) << z << " "
             << setprecision(precision) << setw(12) << cdf(s, z) << endl;
    }
    cout.precision(6); // default
}

void test_fileSystem()
{
    path p("sksso");
    cout << "File exists: " << exists(p) << endl;
}

template<typename Out>
class my_visitor_generic : public boost::static_visitor<Out>
{
public:
    Out operator()(double d) const { return static_cast<Out>(boost::get<double>(d)); }
    Out operator()(int d) const {return static_cast<Out>(boost::get<int>(d)); }
    Out operator()(long long d) const {return static_cast<Out>(boost::get<long long>(d)); }
};


void test_boostVariant()
{

    boost::variant<double, int, long long> v;

    class my_visitor : public boost::static_visitor<double>
    {
    public:
        double operator()(double d) const { return boost::get<double>(d); }
        double operator()(int d) const {return static_cast<double>(boost::get<int>(d)); }
        double operator()(long long d) const {return static_cast<double>(boost::get<long long>(d)); }
    };

    v = 3.14;
    std::cout << boost::get<double>(v) << '\n';
    std::cout << boost::apply_visitor(my_visitor(), v) << '\n';
    std::cout << boost::apply_visitor(my_visitor_generic<double>(), v) << '\n';
    std::cout << boost::apply_visitor(my_visitor_generic<int>(), v) << '\n';
    std::cout << v.which() << '\n';

    v = 2;
    std::cout << boost::get<int>(v) << '\n';
    std::cout << boost::apply_visitor(my_visitor(), v)<< '\n';
    std::cout << v.which() << '\n';

    v = boost::apply_visitor(my_visitor(), v);
    std::cout << v.which() << '\n';
}

int main()
{
    cout << "Running Benchmarks..." << endl;
    test_UUID();
    test_BoostNumericalBounds();
    test_Distributions();
    test_fileSystem();
    test_boostVariant();
    return 0;
}
