#!/usr/bin/python
# This script updates the licence files to the current year.
# The folder of execution is assumed to be LIGER_SOURCE_ROOT/dev/Scripts/
# The development starting date of Liger is assumed 2012A.C
#

import fileinput
import string
import os
import sys
import datetime

startYearNum = 2012
curYearNum = datetime.datetime.now().year

oldLicTemplate = string.Template("""**
** Copyright (C) $startYear-$prevYear The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.""")
newLicTemplate = string.Template("""**
** Copyright (C) $startYear-$curYear The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.""")

if(len(sys.argv)==1):
  thisDirectory = os.getcwd()
else:
  thisDirectory = sys.argv[1]

LIGER_ROOT = os.path.join(thisDirectory, "../../")
print(LIGER_ROOT)

for root, dirs, files in os.walk(LIGER_ROOT):
  for fileName in files:
    if(fileName != "update_lic.py"):
      filePath = os.path.join(root, fileName)
      curText = open(filePath, 'r').read()
      for prevYearNum in range(startYearNum, curYearNum):
        if string.find(curText, oldLicTemplate.substitute(startYear=str(startYearNum), prevYear=str(prevYearNum))) != -1:
          newFileText= string.replace(curText, oldLicTemplate.substitute(startYear=str(startYearNum), prevYear=str(prevYearNum)), newLicTemplate.substitute(startYear=str(startYearNum), curYear=str(curYearNum)))
          fileWriter = open(filePath, 'w')
          fileWriter.write(newFileText)
          break