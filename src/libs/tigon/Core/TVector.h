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
#ifndef TVECTOR_H
#define TVECTOR_H

#include <vector>
#include <iterator>
#include <iostream>
#include <algorithm>

namespace Tigon {

template<typename T> using TVector = std::vector<T>;

template<typename T, typename V> inline TVector<T> &operator<<(TVector<T>& v, const V &e)
{
    v.push_back(static_cast<T>(e));
    return v;
}

template<typename T> inline TVector<T> &operator<<(TVector<T>& v, const TVector<T> &e)
{
    std::copy(e.begin(), e.end(), std::back_inserter(v));
    return v;
}

template<typename T> inline void fill(TVector<T>& v, const T &e, int size = -1)
{
    if(size > 0) {
        v.resize(size);
    }
    std::fill(v.begin(), v.end(), e);
    return;
}

template<typename T> inline int vectorIndexOf(const TVector<T>& v, const T &e)
{
    std::size_t index = std::distance(v.begin(),
                                      std::find(v.begin(), v.end(), e));
    if(index != v.size()) {
        return static_cast<int>(index);
    } else {
        return -1;
    }
}

template <typename T> inline bool vectorContains(const TVector<T> &v, const T & e)
{
    return std::find(v.begin(), v.end(), e) != v.end();
}

template <typename T> int removeAll(TVector<T> &v, const T & e)
{
    std::size_t oldSize = v.size();
    v.erase(std::remove(v.begin(), v.end(), e), v.end());
    return static_cast<int>(oldSize - v.size());
}

template <typename T> void removeAt(TVector<T> &v, size_t index)
{
    if (index >= v.size()) {
        return;
    }
    v.erase(v.begin()+index);
}

template <typename T> inline TVector<T> mid(const TVector<T> &v,
                                            int pos, int length = -1)
{
    if(length == -1) {
        return TVector<T>(v.begin()+pos, v.begin() + v.size());
    } else {
        size_t pp = pos+length;
        if(pp > v.size()) {
            return TVector<T>(v.begin()+pos, v.begin() + v.size());
        } else {
            return TVector<T>(v.begin()+pos, v.begin() + pos + length);
        }
    }
}

template <typename T> inline int vectorCount(const TVector<T> &v, const T &e)
{
    return std::count(v.begin(), v.end(), e);
}

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
    if (!v.empty()) {
        out << "[";
        std::copy(v.begin(), v.end()-1, std::ostream_iterator<T>(out, ", "));
        out << v.back();
        out << "]";
    }
    return out;
}

template <typename T>
std::ostream& operator<< (std::ostream& out, const TVector<TVector<T>>& matrix) {
    for(auto vec : matrix) {
        out << vec;
    }
    return out;
}

}
#endif // TVECTOR_H
