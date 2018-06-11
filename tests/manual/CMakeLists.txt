if (WIN32)
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /O2 /F1000000000000000" )
endif()

if (UNIX)
  SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O2" )
endif()

add_subdirectory (manual_tigon)
