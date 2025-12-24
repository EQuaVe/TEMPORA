# MITL2GTA

This tool translates an MITL formula into a language preserving GTA for pointwise semantics, which can be used for model checking or formula satisfiability checking.

We support both past and future modalities, and also singular intervals to the top level of the formula. We also support language preservation for both finite traces and infinite traces

## Requirements

- C++ compiler with C++17 support
- CMake version >= 3.15
- Bison
- Flex
- Argp

## Installation

The installation has been tested on Ubuntu 22.04 OS, but should work for any system which has the requirements listed above.

We use `cmake` to build the project, and follows standard CMake installation workflow:

```
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=<build-directory> -DCMAKE_BUILD_TYPE=[Release | Debug]
cmake --build <build-directory> -j
cmake --install <build-directory>
```

We also provide a `install.sh` script to install the tool inside a `build` directory in Release mode.

## Usage

### Input

The input to the tool is an MITL formula. The grammar is as follows:

- Interval: We allow specifying open and close bounds for both upper and lower bound of the interval, and the special Inf upper bound. Some examples are:

[1, 2], (1, 2), [1, 2), (1, 2], [1, Inf)

- Atomics: Atomic propositions (p, q, etc.) or boolean constant values True, False

- Boolean operators: or: "||", and: "&&", not: "!"

- Temporal operators: (with an optional interval):

Unary operators:

X (next), Y (yesterday), G (globally), F (finally)

Binary operators:

U (Until), S (Since)

Note: We allow singular intervals only on the top most temporal operator

Some valid formulae are:

- p U q
- p U[1, 2] q
- p U[1, 3] (p S[1, 2) r)
- X[1, Inf) q
- p U[1, 1] q

### Command line interface

The compiled executable is installed in `build/apps/mitl2gta` if built using `install.sh`. We illustrate usage by translating the mitl formula in `examples/toy-example.txt`. All the commands present below should be executed in the main directory of this project. Users also need to install LiveRGTA to perform reachability/liveness analysis. We assume LiveRGTA is installed in `../LiveRGTA`

We support translation for both finite and infinite traces. We support

### 1. Formula satisfiability:

The formula is translated to a GTA with a corresponding reachability query (for finite traces) / liveness query (for infinite traces). The command to generate the output GTA is

- For finite traces:

```
./build/apps/mitl2gta -f examples/toy-example.txt -m satisfiability -o output-gta.txt -t finite
```

This writes the GTA to `output-gta.txt`, with the following printed to stdout:

```
Parsed formula: (p U[1,2] q)
Labels: control_final,final0
```

We now perform reachability analysis for these labels using `gta-reach` (provided by LiveRGTA).

```
../LiveRGTA/build/bin/tck-reach -a gtacovreach -l control_final,final0 output-gta.txt
```

It prints the following to stdout:

```
COVERED_STATES 22
MEMORY_MAX_RSS 8960
REACHABLE true
RUNNING_TIME_SECONDS 0.000315105
STORED_STATES 38
VISITED_STATES 43
VISITED_TRANSITIONS 59
```

The labels being reachable implies the formula was satisfiable over finite traces.

- For infinite traces

```
./build/apps/mitl2gta -f examples/toy-example.txt -m satisfiability -o output-gta.txt -t infinite
```

The output:

```
Parsed formula: (p U[1,2] q)
Labels: final0,final1
```

The corresponding liveness query to `tck-liveness` is

```
../LiveRGTA/build/bin/tck-liveness -a gtascc -l final0,final1 output-gta.txt
```

The output is

```
CYCLE true
MEMORY_MAX_RSS 8124
RUNNING_TIME_SECONDS 0.000473588
STORED_STATES 60
VISITED_STATES 42
VISITED_TRANSITIONS 70
```

A live cycle implies the formula is satisfiable over infinite traces.

We also provide an example of unsatisfiable formula in `./examples/unsat-example.txt`. Performing the same steps as above for this example will result in an unreachable result for the first case and no live cycle for the second case.

### 2. Model Checking:

We translate the negation of a formula to a GTA which can be synchronised with a model to perform model checking. We illustrate this by checking the specification in `./examples/toy-example.txt`.

```
./build/apps/mitl2gta -f examples/toy-example.txt -m model_checking -o output-gta.txt -t infinite
```

The output:

```
Parsed formula: (p U[1,2] q)
p:global0
q:global1
Labels: final0,final1
```

The atomic proposition variables (p and q in the example) are translated to integer variables in the translation, with the mapping printed in the tool. The model can set these variables 0 or 1, to simulate the truth values of atomic propositions. The model must also synchronise each transition with the event `controller@move_model` of the translation. We illustrate this in `./examples/model-checking.txt`. The model (which satisfies p U[1, 2] q) is added after the line `## Model`.

We perform liveness query for model checking using:

```
 ../LiveRGTA/build/bin/tck-liveness -a gtascc -l final0,final1 examples/model-checking.txt
```

Since our model satisfied, the intersection of negation of formula and model resulted in a GTA with empty language. Hence we get the output:

```
CYCLE false
MEMORY_MAX_RSS 19580
RUNNING_TIME_SECONDS 0.000331534
STORED_STATES 25
VISITED_STATES 25
VISITED_TRANSITIONS 26
```

### Flags

We also provide flags to disable optimisations. Run the following command to get the list of all flags:

```
./build/apps/mitl2gta --help
```
