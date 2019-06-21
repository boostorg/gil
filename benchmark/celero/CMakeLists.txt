message(STATUS "Boost.GIL: Configuring benchmarks: celero")

#-----------------------------------------------------------------------------
# Deploy Celero
#-----------------------------------------------------------------------------
include(ExternalProject)

# NOTE for Ninja generator:
# Ninja generator will try to find external project library at the expected location.
# BUILD_BYPRODUCTS option is added to specify build output and satisfy dependencies.
ExternalProject_Add(project_celero
  GIT_REPOSITORY https://github.com/DigitalInBlue/Celero.git
  GIT_TAG "v2.5.0"
  GIT_SHALLOW 1
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/vendor/celero
  TIMEOUT 10
  BUILD_BYPRODUCTS
    <INSTALL_DIR>/lib/static/${CMAKE_STATIC_LIBRARY_PREFIX}celerod${CMAKE_STATIC_LIBRARY_SUFFIX}
    <INSTALL_DIR>/lib/static/${CMAKE_STATIC_LIBRARY_PREFIX}celero${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/vendor/celero
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCELERO_COMPILE_DYNAMIC_LIBRARIES:BOOL=OFF
    -DCELERO_ENABLE_TESTS:BOOL=OFF
    -DCELERO_ENABLE_EXPERIMENTS:BOOL=OFF
    -DCELERO_ENABLE_FOLDERS:BOOL=OFF)

ExternalProject_Get_Property(project_celero INSTALL_DIR)

file(MAKE_DIRECTORY ${INSTALL_DIR}/include)
add_library(gil_celero STATIC IMPORTED)
target_include_directories(gil_celero INTERFACE ${INSTALL_DIR}/include)
target_compile_definitions(gil_celero INTERFACE CELERO_STATIC=1)
set_property(TARGET gil_celero PROPERTY
  IMPORTED_LOCATION_DEBUG
  "${INSTALL_DIR}/lib/static/${CMAKE_STATIC_LIBRARY_PREFIX}celerod${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_property(TARGET gil_celero PROPERTY
  IMPORTED_LOCATION_RELEASE
  "${INSTALL_DIR}/lib/static/${CMAKE_STATIC_LIBRARY_PREFIX}celero${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(gil_celero PROPERTIES
  MAP_IMPORTED_CONFIG_MINSIZEREL Release
  MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  find_library(POWRPROF PowrProf.lib)
  target_link_libraries(gil_celero INTERFACE POWRPROF)
endif()

add_dependencies(gil_celero project_celero)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12.0)
  file(GLOB_RECURSE _benchmarks ${CMAKE_CURRENT_LIST_DIR}/*.cpp CONFIGURE_DEPEND)
else()
  file(GLOB_RECURSE _benchmarks ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
endif()

foreach(_benchmark ${_benchmarks})
  get_filename_component(_name ${_benchmark} NAME_WE)
  set(_target benchmark_celero_${_name})
  add_executable(${_target} ${_name}.cpp)
  target_compile_definitions(${_target} PRIVATE BOOST_GIL_USE_CONCEPT_CHECK=1)
  target_link_libraries(${_target}
    PRIVATE
      gil_compile_options
      gil_include_directories
      gil_dependencies
      gil_celero)
endforeach()

unset(_benchmarks)
unset(_benchmark)
