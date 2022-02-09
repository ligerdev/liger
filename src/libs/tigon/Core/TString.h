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

#include <string>
#include <vector>
#include <locale>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace Tigon {

typedef std::string TString;
typedef std::vector<TString> TStringList;

inline void removeAll(TStringList & list, const TString & str)
{
    list.erase(std::remove(list.begin(), list.end(), str), list.end());
}

inline bool endsWith(const TString &str, const TString &ending)
{
    return boost::algorithm::ends_with(str, ending);
}

inline bool contains(const TString &str, const TString &substr)
{
    return boost::algorithm::contains(str, substr);
}

inline bool contains(const TStringList & list, const TString & str)
{
    return std::find(list.begin(), list.end(), str) != list.end();
}

inline void replaceAll(TString& str, const TString & search, const TString & format)
{
    boost::algorithm::replace_all(str, search, format);
}

inline void trim(TString & str)
{
    boost::algorithm::trim(str);
}

inline TStringList split(const TString &str, const TString & delimiter)
{
    TStringList list;
    boost::algorithm::split(list, str, boost::is_any_of(delimiter));
    return list;
}

inline int indexOf(const TStringList& list, const TString& str)
{
    std::size_t index = std::distance(list.begin(),
                                      std::find(list.begin(), list.end(), str));
    if(index != list.size()) {
        return static_cast<int>(index);
    } else {
        return -1;
    }
}

inline int toInt(const TString & str)
{
    return std::stoi(str);
}

inline bool toBool(const TString &str)
{
    TString tmp;
    std::transform(str.begin(), str.end(), std::back_inserter(tmp), ::tolower);
    std::istringstream is(tmp);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}

inline double toDouble(const TString & str)
{
    return std::stod(str);
}

template<typename T>
inline T convertStringTo(const TString& str)
{
    return boost::lexical_cast<T>(str);
}

template<>
inline bool convertStringTo(const TString& str)
{
    return toBool(str);
}

template<>
inline TString convertStringTo(const TString& str)
{
    return str;
}

template<typename T>
inline TString convertToString(T value)
{
    std::ostringstream oss;
    oss.precision(std::numeric_limits<T>::digits10);
    oss << value;
    return oss.str();
}

template<>
inline TString convertToString(TString value)
{
    return value;
}

template<>
inline TString convertToString(bool value)
{
    if(value) {
        return "true";
    } else {
        return "false";
    }
}

template<typename T>
inline TString convertToString(const std::vector<T> &vec)
{
    TString str;
    for(std::size_t i=0; i<vec.size(); i++) {
        str.append(convertToString<T>(vec[i]));
        if (i < vec.size()-1) {
            str += ",";
        }
    }

    return str;
}

template<typename T>
inline TString convertToString(const std::vector<std::vector<T>> &vec)
{
    TString str;
    for(std::size_t i=0; i<vec.size(); i++) {
        str.append(convertToString<T>(vec[i]));
        if (i < vec.size()-1) {
            str += ";";
        }
    }

    return str;
}

inline TString join(const TStringList & strList, const TString & str)
{
    return boost::join(strList, str);
}
}
