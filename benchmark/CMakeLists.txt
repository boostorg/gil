message(STATUS "Boost.GIL: Configuring benchmarks")

add_library(benchmark_dependencies INTERFACE)

if(WIN32)
  find_library(SHLWAPI Shlwapi.lib)
  target_link_libraries(benchmark_dependencies INTERFACE SHLWAPI)
endif()

if (GIL_USE_CONAN)
    target_link_libraries(benchmark_dependencies
      INTERFACE
        CONAN_PKG::google-benchmark)
else()
  find_package(benchmark REQUIRED)
  target_link_libraries(gil_dependencies benchmark)
endif()

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12.0)
  file(GLOB_RECURSE _benchmarks ${CMAKE_CURRENT_LIST_DIR}/*.cpp CONFIGURE_DEPEND)
else()
  file(GLOB_RECURSE _benchmarks ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
endif()

foreach(_benchmark ${_benchmarks})
  get_filename_component(_name ${_benchmark} NAME_WE)
  add_executable(benchmark_${_name} ${_name}.cpp)
  target_compile_definitions(benchmark_${_name} PRIVATE BOOST_GIL_USE_CONCEPT_CHECK=1)
  # Unfortunately, ALIAS of imported target is not supported
  # see https://github.com/conan-io/conan/issues/2125
  if(GIL_USE_CONAN)
    target_link_libraries(benchmark_${_name}
      PRIVATE
        gil_compile_options
        gil_include_directories
        Boost::disable_autolinking
        Boost::filesystem
        CONAN_PKG::libjpeg
        CONAN_PKG::libpng
        CONAN_PKG::libtiff
        benchmark_dependencies)
  else()
    target_link_libraries(benchmark_${_name}
      PRIVATE
        gil_compile_options
        gil_include_directories
        gil_dependencies
        benchmark_dependencies)
  endif()

  unset(_name)
endforeach()

unset(_benchmarks)
unset(_benchmark)
