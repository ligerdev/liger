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
#include <tigon/Utils/TigonIOUtils.h>
#include <fstream>
#include <cstdio>

using namespace std;

namespace Tigon {

TString readAll(const TString &file)
{
    ifstream ifs(file);
    TString content;
    if(ifs.is_open()) {
        content.assign((istreambuf_iterator<char>(ifs)),
                       (istreambuf_iterator<char>()));
        ifs.close();
        return content;
    }
    ifs.close();
    return "";
}

bool writeToFile(const TString &content, const TString &file)
{
    ofstream ofile(file);
    if(ofile.is_open()) {
        ofile << content;
        ofile.close();
        return true;
    }
    return false;
}

bool removeFile(const TString &file)
{
    try {
        remove(file.c_str());
        return true;
    } catch(...) {
        return false;
    }
}

}
