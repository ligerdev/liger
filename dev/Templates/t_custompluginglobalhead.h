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
#ifndef {% filter upper %}{{ plugin_name }}{% endfilter %}_GLOBAL_H
#define {% filter upper %}{{ plugin_name }}{% endfilter %}_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined({% filter upper %}{{ plugin_name }}{% endfilter %}_LIBRARY)
#   define {% filter upper %}{{ plugin_name }}{% endfilter %}_EXPORT Q_DECL_EXPORT
#else
#   define {% filter upper %}{{ plugin_name }}{% endfilter %} Q_DECL_IMPORT
#endif

#endif // {% filter upper %}{{ plugin_name }}{% endfilter %}_GLOBAL_H


