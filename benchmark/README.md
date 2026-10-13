# GIL Benchmarks

This guide assumes that minimal GIL can already be built, e.g. dependency on
dependent Boost libraries are satisfied.

## Dependencies

The benchmarks depend on the popular micro-benchmarking framework:

* https://github.com/google/benchmark

It is configured using
[ExternalProject_Add](https://cmake.org/cmake/help/latest/module/ExternalProject.html)
with a custom target that drives download, build and deployment of the dependency.

## Writing Benchmarks

Google Benchmark provides facilities to avoid optimizing away some portions of the code or
effects on variables. It might get bypassed by compilers in the future though, as they get
smarter. Thus, providing some side effect outside of benchmarked section is advised.

---

### Using Google Benchmark

Overall structure of the file should look roughly like this:

    <includes>
    <function(s)>
    BENCHMARK(function1_name)
    BENCHMARK(function2_name)
    ...

    BENCHMARK_MAIN()

Lets have a closer look at a benchmarking function:

    void function_name(benchmark::State& state) {
        <setup code, to not be included into timings>

        for (auto _: state) {
            <one iteration of a code to be benchmarked>
        }
        <side effect on variables that are used in the benchmark>
    }

Do note that as long as `_` is in scope, the runtime will be counted towards benchmarking results,
whereas the rest will not be included. Not writing a side effect (like a check can throw,
or printing on the console) might result in code being optimized away, thus making whole
benchmarking function useless.

---

### Building the benchmarks

To build a benchmark, simply put the benchmark `.cpp` file into `<GIL root>/benchmark`
and reconfigure the project.
Then, the targets to build are `benchmark_<cpp filename without extension>`
(`benchmark_ocv_<name>` and `benchmark_ipp_<name>` for the OpenCV and IPP
comparison benchmarks under `opencv/` and `ipp/`).

---

### Running benchmarks

Pairing the benchmarking framework with a good plotting tool of your choice is advised.
It provides options to control which benchmarks to run, for how long,
and output formats. Running `--help` on executables will print available options.
