# Liger

[![Build Status](https://travis-ci.com/ligerdev/liger.svg?branch=master)](https://travis-ci.com/ligerdev/liger)

## Introduction

Liger is an open source integrated optimization environment which is designed to be extensible and have a smooth learning curve so that it can be used by the non-expert in industry. Liger is built about a visual programming language, by which optimization work-flows can be created.

Although there exists a number of optimization frameworks, only commercial and closed source software addresses, to an extent, real-world optimization problems. Typically these packages are low-level libraries which require a large time investment on behalf of the practitioner. Liger aims to reduce this time and provide an intuitive and easy to use environment for optimization.

## CAVEAT

This is alpha quality software -- at best!  

While we work to get Liger quality up, we will be stinting new
features, and will prioritise reported bugs along with the many
existing, known bugs.  The best chance of getting a problem fixed is
to submit a patch that fixes it (along with a unit test case that
verifies the fix)!

## Supported Platforms

* Windows 7 (64-bit) or later
* (K)Ubuntu Linux 10.04 (64-bit) or later
* Mac OS 10.6 or later

## Compiling Liger from source

### Prerequisites:

   * Qt 5.6.0 or later
   * Boost 
   * Python2.7 (for python interface)
   * Optional: MATLAB (for MATLAB interface)
   * On Windows:
     - Microsoft Visual C++ 2015 to 2019
     - [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk)
     - jom
   * On Mac: Xcode (Clang)
   * On Linux: 
     - g++
     - mesa-common-dev, libglu1-mesa-dev, libboost-all-dev

The installed toolchains have to match the one Qt was compiled with.

To install boost:
* Windows: download boost source from boost.org, and unzip. Open the command prompt and change your current directory to the Boost root directory. Then, type the following commands:
```
bootstrap.bat
```
It will generate a file called project-config.jam. 

To use the Python interface, add the following lines to the end of project-config.jam

```
using python 
    : 2.7                           # Version
    : C:\\Python27\\python.exe      # Python Path
    : C:\\Python27\\include         # include path
    : C:\\Python27\\libs            # lib path(s)
    : <define>BOOST_ALL_NO_LIB=1
    ;
```

Now type the following command to build necessary boost libraries 
```
b2 -j4 --with-python --with-filesystem --with-system address-model=64 link=shared threading=multi toolset=msvc variant=release,debug install
```

* Mac: suggest to use home brew
```
    brew install boost
    brew install boost-python
```
* Linux (Ubuntu): 
```
    sudo apt-get install libboost-all-dev
```

### Build from source

You can build Liger with the following commands.
```
    cd $SOURCE_DIRECTORY
    mkdir build
    cd build/
```

Windows 
```
    qmake ../liger.pro -r -spec win32-msvc2015 "BOOST_INCLUDE=/path/to/boost/headers" "BOOST_LIB=/path/to/bootlibs" "BOOST_PYTHON_LIB=/path/to/bootlibs"
    jom
```

Mac: 
```
    qmake ../liger.pro -r -spec macx-clang "BOOST_INCLUDE=/path/to/boost/headers" "BOOST_LIB=/path/to/bootlibs" "BOOST_PYTHON_LIB=/path/to/bootlibs"
    make 
```

Linux: 
```
    export BOOST_INCLUDE=/path/to/boost/headers
    export BOOST_LIB=/path/to/bootlibs
    export BOOST_PYTHON_LIB=/path/to/bootlibs
    qmake ../liger.pro -r -spec linux-g++
    make 
```

Installation (```make install```) is not needed. It is however possible, using

``` 
make install INSTALL_ROOT=$INSTALL_DIRECTORY 
```

Alternatively, Qt Creator can be used to configure and compile Liger. This 
is recommended for Developers.

## License

This software is Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)

This program is free software (software libre); you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License (LGPL) version 3 as published by the Free Software Foundation. Please review the following information to ensure the GNU Lesser General Public License version 3 requirements will be met: https://www.gnu.org/licenses/lgpl-3.0.html.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

Although Liger is released as Free Software, it does not excuse you from scientific propriety, which obligates you to give appropriate credit! If you write a scientific paper describing research that made substantive use of this program, it is your obligation as a scientist to (a) mention the fashion in which this software was used in the Methods section; (b) mention the algorithm in the References section. The appropriate citation is:

João A. Duro, Yiming Yan, Ioannis Giagkiozis, Stefanos Giagkiozis, Shaul Salomon,
Daniel C. Oara, Ambuj K. Sriwastava, Jacqui Morison, Claire M. Freeman, Robert J.
Lygoe, Robin C. Purshouse, and Peter J. Fleming. 2021. Liger: A cross-platform
open-source integrated optimization and decision-making environment. Applied
Soft Computing 98 (2021), 106851. https://doi.org/10.1016/j.asoc.2020.106851

