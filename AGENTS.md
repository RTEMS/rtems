# AGENTS.md

Read [CONTRIBUTING.md](CONTRIBUTING.md) before a commit or a pull request. It
holds the branches, the commit message rules and the sign-off.

## Spec work

Spec work adds or changes an action requirement below `spec/`. `specwareexport`
generates the validation test from it. The
[Software Development Handbook](https://embedded-brains.github.io/rtems-docs-ecss/doc/technical-notes/sdh/index.html)
states the rules for the items.

Work in a worktree of `eb/qual`. It holds the items, the export configuration
`specitems.yml` and all of `eb/main`.

## Set up

```sh
git worktree add -b <topic> ../rtems-<topic> eb/qual
cd ../rtems-<topic>
make prepare
printf '[sparc/gr712rc]\nBUILD_VALIDATIONTESTS = True\n' > uni.ini
printf '[sparc/gr712rc]\nBUILD_VALIDATIONTESTS = True\nRTEMS_SMP = True\n' > smp.ini
for c in uni smp; do
  WAFLOCK=.lock-waf-$c ./waf configure -o build/$c --rtems-config=$c.ini \
    --rtems-tools=<tools>
done
```

`make prepare` creates `.venv` with the spec tools. `<tools>` is the prefix
which `make tools TOOLS_ARCH=sparc` fills, `tools/7` of a checkout. `WAFLOCK`
gives each build directory its own lock file, so the two builds coexist.

## Steps

1. Edit the item.

2. Format each changed file. Name the files:

   ```sh
   .venv/bin/specverify --format-items --do-not-indent-lists \
     --clang-format-path .venv/bin/clang-format \
     --clang-format-style default:file:_clang-format <file> ...
   ```

   A directory argument rewrites every item of the tree.

3. Validate with `.venv/bin/specverify spec`. It checks links and schema, but
   no `${...}` substitution.

4. Export:

   ```sh
   .venv/bin/specwareexport --format-code --no-documentation \
     --clang-format-path .venv/bin/clang-format \
     --clang-format-style file:_clang-format [<target> ...]
   ```

   The export resolves every substitution and names the first broken one. The
   `clang-format` of `.venv` is required, because the system `clang-format`
   cannot read `_clang-format`. The export overwrites `testsuites/validation/`,
   `testsuites/unit/`, most `cpukit/include/rtems/rtems/*.h`,
   `cpukit/doxygen/appl-config.h` and `bsps/include/grlib/*.h`. Change the
   item, then export.

5. Build and run both configurations:

   ```sh
   for c in uni smp; do
     WAFLOCK=.lock-waf-$c ./waf
     <tools>/bin/sis -leon3 -m 2 -r \
       build/$c/sparc/gr712rc/testsuites/validation/<suite>.exe
   done
   ```

   `<suite>` is the test suite whose build item lists the test, for example
   `ts-validation-no-clock-0`. The step is done when the test case reports
   `F:0` in both runs and `git status` is clean after the export.

6. Split the commits. The item goes to a branch of `eb/qual`. The generated
   test and its line in the build item of the suite go to a branch of
   `eb/main`. A kernel fix which the test needs goes there first. A commit
   holds one category: `source`, `spec`, `build-qual`, `pkg` or `ci`. The
   category gives the subject prefix, see [CONTRIBUTING.md](CONTRIBUTING.md). A
   `spec` commit may leave its generated files to the next commit, a `source`
   commit.

7. Inspect each branch:

   ```sh
   PATH=$PWD/.venv/bin:$PATH python .github/inspect_changes.py \
     https://github.com/embedded-brains/rtems <base> <head>
   ```
