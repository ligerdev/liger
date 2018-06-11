# Include this file in a project to get the function to copy them to a directory.
# The project must specify the directory to copy the resources to.
defineTest(copyToDir) {
    files = $$1
    DDIR = $$2
    win32:DDIR ~= s,/,\\,g

    if(isEmpty(DDIR)) {
        win32 {
        } else {
            DDIR = ./
        }
    }

        for(FILE, files) {
            SOURCEPATH = $$absolute_path($$FILE)
            # Replace slashes in paths with backslashes for Windows
            win32{
                SOURCEPATH ~= s,/,\\,g
                DESTPATH ~= s,/,\\,g
            }

            QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$SOURCEPATH) $$quote($$DDIR) $$escape_expand(\\n\\t)
        }

        export(QMAKE_POST_LINK)
    }
