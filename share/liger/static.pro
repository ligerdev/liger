include(../../liger.pri)

TEMPLATE = aux

STATIC_BASE = $$PWD
STATIC_OUTPUT_BASE = $$LIGER_DATA_PATH
STATIC_INSTALL_BASE = $$INSTALL_DATA_PATH

DATA_DIRS = \
    examples \
    generic-highlighter

for(data_dir, DATA_DIRS) {
    files = $$files($$PWD/$$data_dir/*, true)
    # Info.plist.in are handled below
    for(file, files):!contains(file, ".*/Info\\.plist\\.in$"):!exists($$file/*): \
        STATIC_FILES += $$file
}

# used in custom compilers which just copy files
defineReplace(stripStaticBase) {
    return($$relative_path($$1, $$STATIC_BASE))
}

# handle conditional copying based on STATIC_BASE compared to STATIC_OUTPUT_BASE
!isEmpty(STATIC_FILES) {
    isEmpty(STATIC_BASE): \
        error("Using STATIC_FILES without having STATIC_BASE set")
    isEmpty(STATIC_OUTPUT_BASE): \
        error("Using STATIC_FILES without having STATIC_OUTPUT_BASE set")
    !osx:isEmpty(STATIC_INSTALL_BASE): \
        error("Using STATIC_FILES without having STATIC_INSTALL_BASE set")

    !isEqual(STATIC_BASE, $$STATIC_OUTPUT_BASE) {
        copy2build.input += STATIC_FILES
        copy2build.output = $$STATIC_OUTPUT_BASE/${QMAKE_FUNC_FILE_IN_stripStaticBase}
        isEmpty(vcproj):copy2build.variable_out = PRE_TARGETDEPS
        win32:copy2build.commands = $$QMAKE_COPY \"${QMAKE_FILE_IN}\" \"${QMAKE_FILE_OUT}\"
        unix:copy2build.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
        copy2build.name = COPY ${QMAKE_FILE_IN}
        copy2build.config += no_link
        QMAKE_EXTRA_COMPILERS += copy2build
    }

    static.files = $$STATIC_FILES
    static.base = $$STATIC_BASE
    static.path = $$STATIC_INSTALL_BASE
    INSTALLS += static
}

DISTFILES += \
    examples/Parametric_Multi-Objective_TF1.lgr \
