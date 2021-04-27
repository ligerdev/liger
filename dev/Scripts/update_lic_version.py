#!/usr/bin/python
# This script updates the licence version number.
# The folder of execution is assumed to be LIGER_SOURCE_ROOT/dev/Scripts/
# The development starting date of Liger is assumed 2012A.C
#

import fileinput
import string
import os
import sys

oldLicTemplate = """** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html."""

newLicTemplate = """** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html."""

if(len(sys.argv)==1):
  thisDirectory = os.getcwd()
else:
  thisDirectory = sys.argv[1]

LIGER_ROOT = os.path.join(thisDirectory, "../../")
print(LIGER_ROOT)

for root, dirs, files in os.walk(LIGER_ROOT):
  for fileName in files:
    if(fileName != "update_lic.py"):
      if(fileName != "update_lic_version.py"):
        filePath = os.path.join(root, fileName)
        curText = open(filePath, 'r').read()
        if curText.find(oldLicTemplate) != -1:
          newFileText = curText.replace(oldLicTemplate, newLicTemplate)
          fileWriter = open(filePath, 'w')
          fileWriter.write(newFileText)
        