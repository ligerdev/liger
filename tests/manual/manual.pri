include(../qttest.pri)
# prefix test binary with bench_
!contains(TARGET, ^manual_.*):TARGET = $$join(TARGET,,"manual_")

win32 {
  QMAKE_CXXFLAGS += /O2 /F1000000000000000
}
unix {
  QMAKE_CXXFLAGS += -O2
}
