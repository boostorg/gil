message(STATUS "Boost.GIL: Configuring benchmarks: google")

#-----------------------------------------------------------------------------
# Deploy google/benchmark
#-----------------------------------------------------------------------------
include(ExternalProject)

# NOTE for Ninja generator:
# Ninja generator will try to find external project library at the expected location.
# BUILD_BYPRODUCTS option is added to specify build output and satisfy dependencies.
ExternalProject_Add(project_googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG "v1.5.0"
  GIT_SHALLOW 1
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/vendor/benchmark
  TIMEOUT 10
  BUILD_BYPRODUCTS <INSTALL_DIR>/lib/${CMAKE_STATIC_LIBRARY_PREFIX}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/vendor/benchmark
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DBENCHMARK_ENABLE_TESTING=OFF
  UPDATE_COMMAND ""
  TEST_COMMAND "")

ExternalProject_Get_Property(project_googlebenchmark INSTALL_DIR)

file(MAKE_DIRECTORY ${INSTALL_DIR}/include)
add_library(gil_googlebenchmark STATIC IMPORTED)
target_include_directories(gil_googlebenchmark INTERFACE ${INSTALL_DIR}/include)
set_property(TARGET gil_googlebenchmark PROPERTY IMPORTED_LOCATION
  "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  find_library(SHLWAPI Shlwapi.lib)
  target_link_libraries(gil_googlebenchmark INTERFACE SHLWAPI)
endif()

add_dependencies(gil_googlebenchmark project_googlebenchmark)

#-----------------------------------------------------------------------------
# Build benchmarks
#-----------------------------------------------------------------------------
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12.0)
  file(GLOB_RECURSE _benchmarks ${CMAKE_CURRENT_LIST_DIR}/*.cpp CONFIGURE_DEPEND)
else()
  file(GLOB_RECURSE _benchmarks ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
endif()

foreach(_benchmark ${_benchmarks})
  get_filename_component(_name ${_benchmark} NAME_WE)
  set(_target benchmark_google_${_name})
  add_executable(${_target} ${_name}.cpp)
  target_compile_definitions(${_target} PRIVATE BOOST_GIL_USE_CONCEPT_CHECK=1)
  target_link_libraries(${_target}
    PRIVATE
      gil_compile_options
      gil_include_directories
      gil_dependencies
      gil_googlebenchmark)
endforeach()

unset(_benchmarks)
unset(_benchmark)
