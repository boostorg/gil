# Copyright (C) 2020 Samuel Debionne, ESRF.

# Use, modification and distribution is subject to the Boost Software
# License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

#-----------------------------------------------------------------------------
# Find package IPP
#-----------------------------------------------------------------------------
#find_package(IPP REQUIRED)
find_path(IPP_INCLUDE_DIR ipp.h PATHS ${IPP_ROOT}/include)
message("IPP_INCLUDE_DIR: ${IPP_INCLUDE_DIR}")
find_library(IPP_LIB_CORE ippcore PATHS ${IPP_ROOT}/lib)
find_library(IPP_LIB_I ippi PATHS ${IPP_ROOT}/lib)
message("IPP_LIB_CORE: ${IPP_LIB_CORE}")
message("IPP_LIB_I: ${IPP_LIB_I}")
add_library(ipp::ipp INTERFACE IMPORTED)
target_include_directories(ipp::ipp INTERFACE ${IPP_INCLUDE_DIR})
target_link_libraries(ipp::ipp INTERFACE ${IPP_LIB_CORE} ${IPP_LIB_I})

#-----------------------------------------------------------------------------
# Build benchmarks
#-----------------------------------------------------------------------------
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12.0)
  file(GLOB _benchmarks ${CMAKE_CURRENT_LIST_DIR}/*.cpp CONFIGURE_DEPEND)
else()
  file(GLOB _benchmarks ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
endif()

foreach(_benchmark ${_benchmarks})
  get_filename_component(_name ${_benchmark} NAME_WE)
  set(_target benchmark_google_ipp_${_name})
  add_executable(${_target} ${_name}.cpp)
  target_compile_definitions(${_target} PRIVATE BOOST_GIL_USE_CONCEPT_CHECK=1)
  target_link_libraries(${_target}
    PRIVATE
      gil_compile_options
      gil_include_directories
      gil_dependencies
      gil_googlebenchmark
      ipp::ipp)
endforeach()

unset(_benchmarks)
unset(_benchmark)
