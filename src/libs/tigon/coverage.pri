unix:!macx{
  GCOV = $$system(whereis gcov)
  LCOV = $$system(whereis lcov)
  !isEmpty(GCOV) {
    !isEmpty(LCOV) {
      QMAKE_CXXFLAGS += -O0 -fprofile-arcs -ftest-coverage
      LIBS += -lgcov

      coverage.commands += lcov --capture --directory $$OBJECTS_DIR --output-file tigon_coverage.info > /dev/null $$escape_expand(\n\t)
      coverage.commands += lcov --remove tigon_coverage.info "/usr/include/*" "/usr/lib/*" "boost*" "*/eigen/*" "*/json*" "botan.h" "fann_cpp.h" "tinyxml2.h" "*/WFG/wfg_toolkit/*" -o coverage.info $$escape_expand(\n\t)
      coverage.commands += genhtml coverage.info --output-directory coverage-report
      QMAKE_EXTRA_TARGETS += coverage
    }
  }
}
