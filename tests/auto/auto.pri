include(../qttest.pri)
QT += testlib

# prefix test binary with test_
!contains(TARGET, ^test_.*):TARGET = $$join(TARGET,,"test_")
