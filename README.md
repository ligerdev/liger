# Liger

[![build status](http://ligerdev.sheffield.ac.uk/liger-development-team/liger/badges/develop/build.svg)](http://ligerdev.sheffield.ac.uk/liger-development-team/liger/commits/master) 
[![coverage report](http://ligerdev.sheffield.ac.uk/liger-development-team/liger/badges/develop/coverage.svg)](http://ligerdev.sheffield.ac.uk/liger-development-team/liger/commits/master)

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
     - jom
     - MSVC 2015
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
    qmake ../liger.pro -r -spec win32-msvc2015 "BOOST_INCLUDE=/path/to/boost/headers" "BOOST_LIB=/path/to/bootlibs"
    jom
```

Mac: 
```
    qmake ../liger.pro -r -spec macx-clang "BOOST_INCLUDE=/path/to/boost/headers" "BOOST_LIB=/path/to/bootlibs"
    make 
```

Linux: 
```
    qmake ../liger.pro -r -spec linux-g++ "BOOST_INCLUDE=/path/to/boost/headers" "BOOST_LIB=/path/to/bootlibs"
    make 
```

Installation (```make install```) is not needed. It is however possible, using

``` 
make install INSTALL_ROOT=$INSTALL_DIRECTORY 
```

Alternatively, Qt Creator can be used to configure and compile Liger. This 
is recommended for Developers.

## Developer Notes

There is a number of TODOs of known problems. So there's much to be done!
