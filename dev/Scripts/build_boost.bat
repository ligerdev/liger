bootstrap.bat
ECHO using python >> project-config.jam
ECHO     : 2.7                           # Version >> project-config.jam
ECHO     : C:\\Python27\\python.exe      # Python Path >> project-config.jam
ECHO     : C:\\Python27\\include         # include path >> project-config.jam
ECHO     : C:\\Python27\\libs            # lib path(s) >> project-config.jam
ECHO     : <define>BOOST_ALL_NO_LIB=1 >> project-config.jam
ECHO     ; >> project-config.jam

REM b2 -j4 --with-python --with-filesystem --with-system address-model=64 link=shared threading=multi toolset=msvc variant=release,debug install