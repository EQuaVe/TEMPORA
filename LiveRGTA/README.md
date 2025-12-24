# LiveRGTA

This tool implements reachability and liveness algorithms for GTA systems.
It is built on top of TChecker library, with the documentaiton for the library
present in `TChecker_README.md`.

## Installation

Note: The installion is only tested on Ubuntu OS, but we believe
it should compile on Windows and Mac OS too.

The installation is identical to the installation of TChecker library,
instructions for which can be found at `INSTALL.md`.

For Linux users, we also provide a script `./install.sh` which builds the library in Release mode and installs the executables inside a newly created `./build/bin` directory.

## Input Syntax for GTA

The basic syntax for input model remains the same as TChecker's original
syntax, as described in `doc/file-format.md`, but with some additional
attributes, and a few restrictions as described below:

### New attributes

- Attribute `type` for `clock` declaration: These can take one of
  three values: - `history_zero` (default if no value is passed): Declares the clock as a history clock with initial value zero.  
   - `history_inf`: Declares the clock as a history clock with initial value infinity. - `prophecy`: Declares the clock as a prophecy clock with a non-deterministic initial value.

- Attribute `gta_program` for edges: Declares the GTA program associated with edges. The syntax is declared below:

#### GTA Program

A GTA program is an alternating list of guards and actions, each ending with a semi-colon, that is of the form: `<guards>; <actions>; ...`

`<guards` is a (possibly empty) list of conjunction of atomic guards, delimited by `&&`. An atomic guard is either `x op c` or `x - y op c` where `x, y` are clocks, `op` is one of ` < | <= | > | >= | ==` and `c` is either an integer constant or `inf | -inf`.

We disallow guards of the form `x - y op' c` where `op' = > | >= | ==`. This is because such constraints cannot be rewritten directly into a conjunction of `x - y < | <= c` constraints. As an example, `x - y >= 2` is not equivalent to `y - x <= -2` under extended algebra, since the first constraint allows `x = y = inf` while the second constraint does not. The user should be aware of this when trying to write an equivalent constraint for diagonal terms.

`actions` is a (possibly empty) list of atomic actions delimited by `,`. An atomic action is either a release/reset action for clock, represented by `[x]` for clock `x`, or a update action, represented by `x = y` for clocks `x` and `y`.

Refer to `gta-examples/toy_example` for an example of a valid GTA model.

Syntax validity can be checked used `tck-syntax` executable by passing argument `gta` to the `c` flag (`tck-syntax -c gta <input-file>`).

### Restrictions

- Array of clocks is not allowed in models.

- Clock constraints in `provided` attribute of edges and `invariant` attribute of locations will not be checked, all clock constraints must be added to `gta_program` attribute.

- Clock actions in `do` attribute of edges will be not performed, all actions must be added to `gta_program` attribute.

## Using LiveRGTA

In addition to all the algorithms present in tools provided
by TChecker, we add two new algorithms:

1. `gtacovreach` (in `tck-reach` executable): This performs
   reachability analysis over GTA zone graphs. The
   usage is similar to using `covreach` provided
   by TChecker as described in `doc/usage.md`, with the only
   difference being the input file should follow the syntax for
   GTA models.

2. `gtascc` (in `tck-liveness` executable): This performs
   liveness analysis over GTA zone graphs. The
   usage is also similar to using `couvscc` provided by
   TChecker.

## References

- Model semantics and reachability algorithm: A Unified Model for Real-Time Systems: Symbolic
  Techniques and Implementation: Akshay, Paul Gastin, R. Govind, Aniruddha Joshi, B. Srivathsan. CAV '23

- Liveness algorithm: MITL Model Checking via Generalized Timed Automata and a New Liveness Algorithm: S. Akshay, Paul Gastin, R. Govind, B. Srivathsan. CONCUR '24
