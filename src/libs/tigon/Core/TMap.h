/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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

#include <map>

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace Tigon {

template<typename K, typename T> using TMap = std::map<K, T>;

template<typename K, typename T> std::vector<K> getMapKeys(const TMap<K, T> &map)
{
    std::vector<K> keys;
    boost::copy(map | boost::adaptors::map_keys, std::back_inserter(keys));
    return keys;
}

template<typename K, typename T> bool mapHasKey(const TMap<K, T> &map,
                                                const K &key)
{
    return map.find(key) != map.end();
}

}
