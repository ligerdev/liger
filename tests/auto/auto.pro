# NOTE: If you are trying to debug (i.e. step through) the tests, make sure you disable
# QML in the Debugger settings. Namely, go to Projects locate Debugger Settings and
# uncheck "Enable QML".

# NOTE: Tests in this directory are for unit testing only, not for benchmarking.
# The goal of unit testing is to isolate each part of the program and show that
# the individual parts are correct. A unit test provides a strict, clear and
# explicit written contract that the piece of code must satisfy. (From Wikipedia)
# Unit tests are normally short code fragments and run fast.
# If your piece of code does not serve the above purposes, think twice before you
# include it here.

TEMPLATE = subdirs

SUBDIRS += \
    #test_network \
    #test_eigen \
    test_tigon
