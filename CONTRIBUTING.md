# Contributing

This repository holds the pre-qualified RTEMS of embedded brains. The
[Software Development Handbook](https://embedded-brains.github.io/rtems-docs-ecss/doc/technical-notes/sdh/index.html)
states the rules for the code, the specification items and the documentation.

## Issues

Report an issue in the
[issue tracker](https://github.com/embedded-brains/rtems/issues) of this
repository. Name the branch, the BSP and the configuration. Attach the test
output where a test fails.

## Branches

| Branch       | Content                                                                                          | Changes enter through                     |
| ------------ | ------------------------------------------------------------------------------------------------ | ----------------------------------------- |
| `eb/main`    | the baseline, the fixes of the regression test runs, and the build and CI infrastructure         | a pull request against `eb/main`          |
| `eb/qual`    | `eb/main` plus the specification items in `spec/` and the pre-qualified RTEMS, ready for an ISVV | a piecewise integration from `eb/staging` |
| `eb/staging` | `eb/qual` plus the work which waits for the integration into `eb/qual`                           | a pull request against `eb/staging`       |

The workflow Harmonia merges every change of `eb/main` into `eb/qual`, and
every change of `eb/qual` into `eb/staging`, through a pull request.

A change takes the branch of its content:

| Change                                                 | Branch       |
| ------------------------------------------------------ | ------------ |
| a kernel or BSP fix, a build item, the CI, the tooling | `eb/main`    |
| a generated validation test and its build item         | `eb/main`    |
| a specification item outside `spec/build/`             | `eb/staging` |

A kernel fix enters before the validation test which checks it.

## Commit messages

A commit holds the files of one category. The categories are `source`, `spec`,
`pkg` and `ci`. The table `_CATEGORIES` in `.github/inspect_changes.py` assigns
the category of a path. A `spec` commit may leave its generated files to the
next commit, which must be a `source` commit.

The subject is `<path prefix>: <Summary>`, for example
`bsps/aarch64/raspberrypi: Add framebuffer driver`. It has at most 50
characters. The second line is blank. The body wraps at 72 columns.

The body states the problem, then the solution in the imperative. Reference an
issue with `Closes #NNNN`.

An assisted commit carries `Assisted-by:` immediately before `Signed-off-by:`:

```
Assisted-by: Claude:claude-opus-5-5 claude-code
Signed-off-by: Jane Doe <jane.doe@example.com>
```

`Signed-off-by:` certifies the [Developer Certificate of Origin](DCO.txt). Add
it with `git commit -s`. Use your own name, never the name of an assistant.

## Local checks

These commands run the checks of the CI on your machine:

| Command                                                 | Check                                                   |
| ------------------------------------------------------- | ------------------------------------------------------- |
| `make tools TOOLS_ARCH=<arch>`                          | builds the tools of an architecture                     |
| `make bsps TOOLS_ARCH=<arch>`                           | builds all BSPs of an architecture                      |
| `make tests TOOLS_ARCH=<arch>`                          | runs the test suites on the simulators                  |
| `make config-tests CONFIG=<name>`                       | builds a special configuration and runs its test suites |
| `make pkg TOOLS_ARCH=sparc PKG_BSP=<bsp>`               | builds a package                                        |
| `python .github/inspect_changes.py <URL> <base> <head>` | inspects the commits of a change set                    |

The inspection needs the tools of `.venv/bin` in `PATH`. `make prepare` creates
the virtual environment. `python3 run_tests.py configurations` lists the
special configurations. The CI runs the tests with `TIMEOUT_SCALER=4.0`, which
gives the test timeouts twice the margin of the test runner items.

## CI report

The workflow Olympos checks a push and a pull request of each branch. It runs
these jobs:

| Job        | Check                                                                                  |
| ---------- | -------------------------------------------------------------------------------------- |
| Argos      | inspects the category, the item format and the export of each commit of a pull request |
| Hephaistos | builds the tools of each architecture                                                  |
| Sisyphos   | builds all BSPs of each architecture and runs their test suites                        |
| Daidalos   | builds each special configuration and runs its test suites                             |
| Zerberus   | builds the packages of the gr712rc, gr740 and gr765 BSPs                               |

The workflow Hermes posts the report of Argos and the package summaries of
Zerberus as a comment on the pull request. It updates the comment after each
push.
