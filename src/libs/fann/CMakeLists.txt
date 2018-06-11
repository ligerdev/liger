include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
ADD_DEFINITIONS(-DFANN_DLL_EXPORTS)

add_library(Fann SHARED doublefann.c )
