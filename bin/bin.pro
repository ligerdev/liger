TEMPLATE = app
TARGET = liger.sh
OBJECTS_DIR =

PRE_TARGETDEPS = $$PWD/liger.sh

QMAKE_LINK = cp $$PWD/liger.sh $@ && : IGNORE REST OF LINE:
QMAKE_STRIP =
CONFIG -= qt separate_debug_info gdb_dwarf_index

QMAKE_CLEAN = liger.sh

target.path  = $$LIGER_BUILD_TREE/bin
INSTALLS    += target

OTHER_FILES = $$PWD/liger.sh
