include(../../liger.pri)

FILES = $$PWD/failure_reporter.py

### Tests
linux-* {
  FILES += $$PWD/run_all_unit_tests_linux.sh  \
           $$PWD/run_all_manual_tests_linux.sh
}

mac {
  FILES += $$PWD/run_all_unit_tests.sh  \
           $$PWD/run_all_manual_tests.sh

  unittest.commands = chmod +x $$quote($$TEST_BIN_PATH/run_all_unit_tests.sh); $$TEST_BIN_PATH/run_all_unit_tests.sh
  QMAKE_EXTRA_TARGETS += unittest

  manualtest.commands = chmod +x $$quote($$TEST_BIN_PATH/run_all_manual_tests.sh); $$TEST_BIN_PATH/run_all_manual_tests.sh
  QMAKE_EXTRA_TARGETS += manualtest
}

win32 {

}

# Copy files
OTHER_FILES += $$FILES
for(f, FILES) {
  tmp = $$absolute_path($$f) $$quote($$TEST_BIN_PATH/)
  win32:tmp ~= s,/,\\,g
  copydata.commands += $$QMAKE_COPY $$tmp $$escape_expand(\\n\\t)
}

first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
