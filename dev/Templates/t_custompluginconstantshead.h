/****************************************************************************
**
** Copyright (C) 2012-2016 The University of Sheffield (www.sheffield.ac.uk)
** Copyright (C) 2012-2016 Ioannis Giagkiozis (www.sheffield.ac.uk)
**
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
#ifndef {% filter upper %}{{ plugin_name }}{% endfilter %}CONSTANTS_H
#define {% filter upper %}{{ plugin_name }}{% endfilter %}CONSTANTS_H

namespace {{ plugin_name }} {
namespace Constants {

const char * const ACTION_ID = "{{ plugin_name }}.Action";
const char * const MENU_ID   = "{{ plugin_name }}.Menu";

} // namespace Constants
} // namespace {{ plugin_name }}


#endif // {% filter upper %}{{ plugin_name }}{% endfilter %}CONSTANTS_H
