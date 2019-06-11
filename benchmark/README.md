# GIL Benchmarks

This guide assumes that minimal GIL can already be built, e.g. dependency on
dependent Boost libraries are satisfied.

## Dependencies

The benchmarks depend on popular micro-benchmarking frameworks:

* https://github.com/DigitalInBlue/Celero
* https://github.com/google/benchmark

All are configured using
[ExternalProject_Add](https://cmake.org/cmake/help/latest/module/ExternalProject.html)
with custom targets that drive download, build and deployment of the dependencies.

## Writing Benchmarks

Both frameworks provide facilities to avoid optimizing away some portions of the code or
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

### Using Celero

Writing a Celero benchmark is a bit more involved process.
As most benchmarks will require *some* setup code not to be counted in the benchmarking section,
the guide ignores function only benchmarks.
To provide setup code and side effects, one has to write a fixture:

    class FixtureName : public celero::TextFixture {
    public:
        <members to use inside benchmark>
        FixtureName() = default; //trivial default constructor

        virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override {
            <code that will generate sizes of input>
        }

        virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override {
            <code to generate input based on one of the values received from getExperimentValues()>
        }

         virtual void tearDown() override {
             <side effects>
             <resource releasing>
         }
    }

Fixture is sort of data provider. It can be used to control what input will be generated,
e.g. size, variety, data type. Do note that `celero::TestFixture::ExperimentValue`
can be extended using inheritance, and in original form is just a combination of
numerical value and number of iterations.

Then, to specify code to be benchmarked, use the following:

    BASELINE_F(GroupName, BenchmarkName, FixtureName, SampleCount, IterationCount) {
        <one iteration baseline-code>
    }

    BENCHMARK_F(GroupName, BenchmarkName, FixtureName, SampleCount, IterationCount) {
        <one iteration alternative-code>
    }

    <More BENCHMARK_F's>

    <CELERO_MAIN>

Baseline should be the code to be used as a reference: performance goal, original performance, etc.
Other benchmarks should be competitors of the baseline. Do note that the code inside
`BASELINE_F` and `BENCHMARK_F` will be timed, the rest will not.

---

### Building the benchmarks

To build a benchmark, simply put the benchmark `.cpp` file into `<GIL root>/benchmark`
and reconfigure the project.
Then, the targets to build are `benchmark_<framework>_<cpp filename without extension>`.

---

### Running benchmarks

Pairing both benchmarking frameworks with a good plotting tool of your choice is advised.
Both frameworks provide options to control which benchmarks to run, for how long,
and output formats. Running `--help` on executables will print available options.
