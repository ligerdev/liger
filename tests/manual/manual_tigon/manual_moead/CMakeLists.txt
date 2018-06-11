include_directories (${PROJECT_SOURCE_DIR}/src/libs)
include_directories (${PROJECT_SOURCE_DIR}/src/libs/tigon)
add_executable(mannual_moead manual_moead.cpp)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (mannual_moead Tigon)
