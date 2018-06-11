# Tests in Liger Dev

## Types 
We perform two types of tests during our development process, unit test and manual test.
- **Unit test**. By definition, the unit test should be designed to justify the correctness of the code and identify potential bugs. For unit testing, we utilise [Qt Test framework](http://doc.qt.io/qt-5/qtest-overview.html). **Note that, unit tests should be short and quick to run. Lengthy tests, e.g. long-running benchmarks, should be placed in this type.** As part of the continuous integration process, we run all unit tests every time when new changes are committed. A merge request can only be accepted if all unit tests pass successfully.
- **Manual test**. All tests that are not unit tests, including benchmarks, are assigned to this category. These tests do not adopt the Qt Test framework: they are simply a normal C++ file with a main function. These tests will not be run during the continuous integration process. 

## Remarks
- All modules in Liger must be unit tested. 
- All unit tests should be created in folder [auto](./auto)
- All other tests should be created in folder [manual](./manual)
