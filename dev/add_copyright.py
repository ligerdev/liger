#!/usr/bin/python
import fileinput, glob, string, sys, os
from os.path import join
import datetime

qtcreator_notice = """**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**"""
liger_notice = """**
** This file is part of Liger.
**
** Liger is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version
**
** Liger is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Liger.  If not, see <http://www.gnu.org/licenses/>.
**"""

if(len(sys.argv)==1):
  thisDirectory = os.getcwd()
else:
  thisDirectory = sys.argv[1]

for root, dirs, files in os.walk(thisDirectory):
  for f in files:
    if(f != "replace_lic.py"):
      #print os.path.join(root,f)
      inbuff = open(os.path.join(root,f)).read()
      inf    = open(os.path.join(root,f),"w")
      relloc = root.replace(thisDirectory,"")
      relloc = "QTC_ROOT/"+relloc
      curstr = liger_notice
      inf.write(inbuff.replace(qtcreator_notice,curstr))      
      print relloc



























