#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
""" Provide a command line interface to run the tests on a simulator. """

# Copyright (C) 2026 embedded brains GmbH & Co. KG
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
from typing import Any

_Data = dict[str, Any]

# The specification item root of the simulator support.  A package build reads
# the items where they lie, so an item UID is the path below this directory.
_SPEC_DIRECTORY = Path("config-bsps/spec")

# The directory with the test runner specification items.  They are ordinary
# package items and are used unchanged by a package build.
_RUNNER_DIRECTORY = _SPEC_DIRECTORY / "pkg/test-runner"

# The directory with the target specification items.  One target is one
# machine, so a simulator is a target of its own beside the board.
_TARGET_DIRECTORY = _SPEC_DIRECTORY / "target"

# The test states which the test itself prints.  A test in one of them is not
# expected to pass, so a failure is not a regression.
_TOLERATED_STATES = frozenset(
    ("EXPECTED_FAIL", "USER_INPUT", "INDETERMINATE", "BENCHMARK"))

# The substitution variables of the simulator deployment directories.  Each
# test runner item refers to the package which provides its simulator.
_QEMU_DIRECTORY = "../deployment/qemu:/directory"
_SIS_DIRECTORY = "../deployment/sis:/directory"
_GDB_SIM_DIRECTORY = "../deployment/gdb-sim:/directory"

# The substitution variable of the directory which holds the device tree of
# the PowerPC simulator.  A package build deploys the file.  A local run reads
# it where it lies beside the test runner items.
_PSIM_DIRECTORY = "../deployment/psim-device-tree:/directory"

# The default simulator deployment directory.  A package build installs every
# simulator below the tools prefix.
_DEFAULT_TOOLS = "tools/7"

# The files which define the tools and the simulators.  Their hash is the tools
# key.
_TOOLS_FILES = (Path("build_tools.py"), Path("spec-pkg-tools"))

# The file beside the test log which holds the tools key of the run.
_TOOLS_KEY_FILE = "tools-key.txt"


class Target:
    """
    Describe how the tests of one BSP are built and run.

    The build options live here rather than in the Makefile, so that adding a
    BSP is one edit in one file.  The simulator command lives in the test
    runner item of the BSP, which a package build uses unchanged.
    """

    def __init__(self,
                 simulator: str = "qemu",
                 options: dict[str, str] | None = None,
                 do_not_run: tuple[str, ...] = (),
                 known_failures: dict[str, str] | None = None) -> None:
        # The last element of the target UID.  It names the deployment which
        # provides the simulator, so a BSP which gains a second simulator
        # gains a target of its own for it.
        self.simulator = simulator
        self.options = {"BUILD_TESTS": "True"}
        self.options.update(options or {})
        self.do_not_run = do_not_run
        # Failures caused by the simulator rather than by the BSP.  They stay
        # here because they are a property of our tooling.  A failure which is
        # a property of the BSP belongs into a set-test-state action of
        # spec/build/bsps, which is upstreamed.
        self.known_failures = known_failures or {}


def _runner(bsp: str) -> str:
    """ Get the name of the test runner item of a BSP. """
    return "simulator-" + bsp.replace("/", "-")


TARGETS: dict[str, Target] = {
    "aarch64/a53_lp64_qemu":
    Target(
        options={
            # The BSP takes 128 MiB of the memory area.  dl09 allocates
            # four blocks of 32 MiB next to the objects it loads, so the
            # fourth allocation finds no space.  The machine of the
            # runner has 4 GiB.
            "BSP_A53_RAM_LENGTH": "0x10000000",
        }),
    "arm/xilinx_zynq_a9_qemu":
    Target(),
    # The pc686 variant is built without SSE.  The C library selects the SSE
    # path of the floating point environment from the CPUID bit, while RTEMS
    # sets CR4.OSFXSR only when it is compiled for SSE, so psxfenv01 takes an
    # undefined opcode exception there.  This variant is built with SSE and
    # enables the feature in the CPU.
    "i386/pc586-sse":
    Target(
        options={
            # The board of the runner has no monitor.  Without this the
            # printk() output goes to the VGA console until the boot
            # command line is parsed in bsp_start(), so a test which
            # terminates before that prints nothing at all.
            "USE_COM1_AS_CONSOLE": "True",
        }),
    "microblaze/petalogix_s3adsp1800":
    Target(
        known_failures={
            # The test busy waits exactly one clock tick period and
            # asserts that a tick elapsed, so it has no timing margin.
            # The CPU counter of this BSP is a register of the AXI timer
            # and the busy wait reads that register in a loop.  Qemu
            # takes the tick interrupt later out of such a loop than out
            # of the poll loop which the test synchronizes with, and the
            # difference is larger than the margin.
            "spcpucounter01.exe": "QEMU_TICK_LATENCY",
        }),
    "mips/jmr3904":
    Target("gdb-sim"),
    "mips/malta":
    Target(),
    "moxie/moxiesim":
    Target("gdb-sim"),
    "or1k/generic_or1k":
    Target(),
    "powerpc/psim":
    Target("gdb-sim"),
    "riscv/rv64imafdc":
    Target(),
    "sparc/gr712rc":
    Target("sis"),
    "sparc/gr740":
    Target("sis"),
    "sparc/gr765":
    Target("sis"),
    "x86_64/amd64":
    Target(),
}


# The multiprocessing tests need two nodes, which run in two simulator
# instances at the same time.  A test run starts one simulator per executable.
_MP_NODE_TESTS = ("base_mp_node1.exe", "base_mp_node2.exe") + tuple(
    f"mp{test:02d}_node{node}.exe"
    for test in (1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14)
    for node in (1, 2))


class Configuration:
    """
    Describe a special configuration of a BSP.

    A special configuration builds one BSP with options beyond the default
    configuration and runs its tests.  It has a build directory and a test
    log directory of its own.
    """

    def __init__(self,
                 bsp: str,
                 options: dict[str, str],
                 do_not_run: tuple[str, ...] = ()) -> None:
        self.bsp = bsp
        self.options = options
        self.do_not_run = do_not_run


CONFIGURATIONS: dict[str, Configuration] = {
    "gr712rc-smp-posix":
    Configuration("sparc/gr712rc", {
        "RTEMS_POSIX_API": "True",
        "RTEMS_SMP": "True"
    }),
    "gr740-smp-debug":
    Configuration("sparc/gr740", {
        "RTEMS_DEBUG": "True",
        "RTEMS_SMP": "True"
    }),
    "gr740-smp-posix":
    Configuration("sparc/gr740", {
        "RTEMS_POSIX_API": "True",
        "RTEMS_SMP": "True"
    }),
    "gr765-smp-posix":
    Configuration("sparc/gr765", {
        "RTEMS_POSIX_API": "True",
        "RTEMS_SMP": "True"
    }),
    "psim-mp":
    Configuration("powerpc/psim", {"RTEMS_MULTIPROCESSING": "True"},
                  _MP_NODE_TESTS),
}


class RunTestsError(Exception):
    """ Indicates an invalid usage of the command. """


def _key_values(pairs: list[str], what: str) -> dict[str, str]:
    """ Get the key value map of a list of KEY=VALUE arguments. """
    values: dict[str, str] = {}
    for pair in pairs:
        key, separator, value = pair.partition("=")
        if not separator:
            raise RunTestsError(f"the {what} '{pair}' is not a KEY=VALUE pair")
        values[key] = value
    return values


def _get_target(bsp: str) -> Target:
    """ Get the target of a BSP. """
    try:
        return TARGETS[bsp]
    except KeyError as err:
        known = ", ".join(sorted(TARGETS))
        raise RunTestsError(
            f"there is no simulator target for '{bsp}'; there is one for "
            f"{known}") from err


def _get_configuration(name: str) -> Configuration:
    """ Get the special configuration of the name. """
    try:
        return CONFIGURATIONS[name]
    except KeyError as err:
        known = ", ".join(sorted(CONFIGURATIONS))
        raise RunTestsError(
            f"there is no special configuration '{name}'; there is one for "
            f"{known}") from err


def _section(bsp: str, target: Target, options: dict[str, str]) -> str:
    values = dict(target.options)
    values.update(options)
    lines = [f"[{bsp}]"]
    lines.extend(f"{key} = {value}" for key, value in sorted(values.items()))
    return "\n".join(lines) + "\n"


def _config(args: argparse.Namespace) -> int:
    """
    Print the configuration file sections.

    Without a BSP list this prints the sections of the simulator BSPs alone,
    which is what local work wants.  With one it prints a section for every
    BSP and adds the test options to the simulator BSPs, which is what the
    architecture wide build wants.

    A default reaches every BSP of the file.  An option reaches the BSPs
    named on the command line and overrides the option of the target.
    """
    defaults = _key_values(args.default, "default")
    options = _key_values(args.option, "option")
    bsps = list(args.bsp)
    if args.configuration:
        configuration = _get_configuration(args.configuration)
        bsps.append(configuration.bsp)
        options = {**configuration.options, **options}
    targets = {bsp: _get_target(bsp) for bsp in bsps}
    if defaults:
        sys.stdout.write("[DEFAULT]\n")
        for key, value in sorted(defaults.items()):
            sys.stdout.write(f"{key} = {value}\n")
    if not args.all_bsps:
        for bsp, target in targets.items():
            sys.stdout.write(_section(bsp, target, options))
        return 0
    seen: set[str] = set()
    for line in sys.stdin:
        variant = line.strip()
        if not variant:
            continue
        target = targets.get(variant)
        if target is not None:
            seen.add(variant)
            sys.stdout.write(_section(variant, target, options))
        else:
            sys.stdout.write(f"[{variant}]\n")
    missing = sorted(set(targets) - seen)
    if missing:
        names = ", ".join(f"'{bsp}'" for bsp in missing)
        raise RunTestsError(f"the BSP list has no {names}; a simulator BSP "
                            "must be built with the tests")
    return 0


def _target_uid(bsp: str, target: Target) -> str:
    """ Get the UID of the target item which runs the tests of a BSP. """
    return f"/target/{bsp}/{target.simulator}/target"


def _timeout_key(enable: list[str]) -> str:
    """
    Get the key of the test timeouts of a build.

    A test of an SMP build runs longer than the same test of a uniprocessor
    build, so each configuration measures its own durations.
    """
    return "smp" if "RTEMS_SMP" in enable else "default"


def _test_timeouts(bsp: str, target: Target, timeout_key: str) -> Path | None:
    """
    Get the test timeouts item of a BSP if it holds the key of the build.

    The test runner rejects a key which the item does not carry, and a
    configuration runs before anything measures it.
    """
    from specitems import load_data  # pylint: disable=import-outside-toplevel

    path = _TARGET_DIRECTORY / bsp / target.simulator / "test-timeouts.yml"
    if not path.is_file():
        return None
    if timeout_key not in load_data(str(path))["timeouts"]:
        return None
    return path


def _directory_defines(args: argparse.Namespace) -> dict[str, str]:
    return {
        _QEMU_DIRECTORY: args.qemu_directory,
        _SIS_DIRECTORY: args.sis_directory,
        _GDB_SIM_DIRECTORY: args.gdb_sim_directory,
        _PSIM_DIRECTORY: str(_RUNNER_DIRECTORY),
    }


def _write_runner(bsp: str,
                  target: Target,
                  output_directory: Path,
                  timeout: float = 0.0,
                  timeout_scaler: float = 0.0) -> Path:
    """
    Write the test runner item used by this run.

    The item in the repository describes one BSP and is used unchanged by a
    package build.  The tests which must not be launched at all are a property
    of the local run, so they are added to a copy.

    A timeout overrides the timeouts of the item.  A simulator which hangs on
    many tests would otherwise spend hours in the default timeout, which is
    not what a first triage of an architecture needs.

    A timeout scaler overrides the scaler of the item, unless a timeout is
    given.  The measured durations come from one host, and a slower or a
    shared host needs a larger margin.
    """
    runner = _runner(bsp)
    source = _RUNNER_DIRECTORY / f"{runner}.yml"
    if not source.is_file():
        raise RunTestsError(f"there is no test runner item '{source}'")
    text = source.read_text(encoding="utf-8")
    if target.do_not_run:
        entries = "".join(f"\n- {name}" for name in sorted(target.do_not_run))
        text = text.replace("do-not-run: []", f"do-not-run:{entries}")
    if timeout > 0.0:
        text = re.sub(r"^default-timeout-in-seconds: .*$",
                      f"default-timeout-in-seconds: {timeout}",
                      text,
                      count=1,
                      flags=re.M)
        text = re.sub(r"^min-timeout-in-seconds: .*$",
                      f"min-timeout-in-seconds: {timeout}",
                      text,
                      count=1,
                      flags=re.M)
        text = re.sub(r"^timeout-scaler: .*$",
                      "timeout-scaler: 1.0",
                      text,
                      count=1,
                      flags=re.M)
    elif timeout_scaler > 0.0:
        text = re.sub(r"^timeout-scaler: .*$",
                      f"timeout-scaler: {timeout_scaler}",
                      text,
                      count=1,
                      flags=re.M)
    destination = output_directory / f"{runner}.yml"
    destination.write_text(text, encoding="utf-8")
    return destination


def _enabled_by_build(build_directory: str, bsp: str) -> list[str]:
    """
    Get the enabled set of the build.

    The test runner item carries a command line argument of the simulator for
    each build option which changes the machine, such as the processor count
    of an SMP build.  The build records its enabled set in the configuration
    cache of the variant, so the run needs no option of its own.
    """
    cache = Path(build_directory) / "c4che" / f"{bsp}_cache.py"
    if not cache.is_file():
        raise RunTestsError(
            f"there is no configuration cache '{cache}'; configure the build "
            f"of {bsp} first")
    namespace: dict[str, Any] = {}
    exec(compile(cache.read_text(encoding="utf-8"), str(cache), "exec"),
         namespace)  # pylint: disable=exec-used
    return list(namespace.get("ENABLE", []))


def _states_by_executable(test_log: _Data) -> dict[str, str]:
    """ Get the test state which each executable printed. """
    states: dict[str, str] = {}
    for report in test_log.get("reports", []):
        info = report.get("info", {})
        state = info.get("state", "")
        if state:
            states[os.path.basename(report["executable"])] = state.strip()
    return states


def _reconcile(test_log: _Data,
               target: Target) -> tuple[list[str], list[str], list[str], int]:
    """
    Judge the run.

    A failure is a regression unless the test printed a state which says that
    it is not expected to pass, or unless it is a known failure of the
    simulator.  A test which passes although it is expected to fail is
    reported too, because that is the moment to remove the expectation.
    """
    from specmake.ctrf import (  # pylint: disable=import-outside-toplevel
        CTRF_FAILED, report_to_ctrf_tests)

    states = _states_by_executable(test_log)
    failed_names: set[str] = set()
    total = 0
    for report in test_log.get("reports", []):
        for test in report_to_ctrf_tests(report):
            total += 1
            if test["status"] == CTRF_FAILED:
                failed_names.add(test["filePath"])
    regressions: list[str] = []
    tolerated: list[str] = []
    for name in sorted(failed_names):
        state = states.get(name, "")
        if state in _TOLERATED_STATES:
            tolerated.append(f"{name} ({state})")
        elif name in target.known_failures:
            tolerated.append(f"{name} ({target.known_failures[name]})")
        else:
            regressions.append(name)
    unexpected_passes = [
        name for name, state in sorted(states.items())
        if state == "EXPECTED_FAIL" and name not in failed_names
    ]
    return regressions, tolerated, unexpected_passes, total


def _report(bsp: str, regressions: list[str], tolerated: list[str],
            unexpected_passes: list[str], total: int) -> None:
    print(f"## Simulator tests for {bsp}")
    print()
    # A regression is the only result which fails the run, see _run().
    print("### ❌ Failed" if regressions else "### ✅ Passed")
    print()
    print(f"- test results: {total}")
    print(f"- regressions: {len(regressions)}")
    print(f"- tolerated failures: {len(tolerated)}")
    print(f"- unexpected passes: {len(unexpected_passes)}")
    for title, names in (("Regressions", regressions), ("Unexpected passes",
                                                        unexpected_passes),
                         ("Tolerated failures", tolerated)):
        if not names:
            continue
        print()
        print(f"### {title}")
        print()
        for name in names:
            print(f"- {name}")


def _tools_key() -> str:
    """
    Get the tools key.

    The key is the hash of the files which define the tools and the
    simulators.  A test log of other tools is no valid reuse source.
    """
    digest = hashlib.sha256()
    for root in _TOOLS_FILES:
        paths = sorted(root.rglob("*")) if root.is_dir() else [root]
        for path in paths:
            if path.is_file():
                digest.update(str(path).encode("utf-8"))
                digest.update(path.read_bytes())
    return digest.hexdigest()


def _run(args: argparse.Namespace) -> int:
    bsp = args.bsp
    target = _get_target(bsp)
    if args.configuration:
        configuration = _get_configuration(args.configuration)
        if configuration.bsp != bsp:
            raise RunTestsError(
                f"the special configuration '{args.configuration}' builds "
                f"{configuration.bsp}, not {bsp}")
        target = Target(target.simulator, target.options,
                        target.do_not_run + configuration.do_not_run,
                        target.known_failures)
    directory = Path(args.build_directory) / bsp
    if not directory.is_dir():
        raise RunTestsError(
            f"there is no build directory '{directory}'; build the tests of "
            f"{bsp} first")
    output_directory = Path(args.output_directory) / bsp
    output_directory.mkdir(parents=True, exist_ok=True)
    runner = _write_runner(bsp, target, output_directory, args.timeout_seconds,
                           args.timeout_scaler)
    test_log = output_directory / "test-log.json"
    previous = output_directory / "test-log-previous.json"
    if test_log.is_file():
        shutil.move(str(test_log), str(previous))
    key_file = output_directory / _TOOLS_KEY_FILE
    tools_key = _tools_key()
    reuse = args.reuse and previous.is_file()
    if reuse and (not key_file.is_file() or
                  key_file.read_text(encoding="utf-8").strip() != tools_key):
        print(f"note: the tools changed since the previous run of {bsp}, so "
              "no report is reused",
              file=sys.stderr)
        reuse = False
    defines = _directory_defines(args)
    defines.update(_key_values(args.define, "definition"))
    enable = _enabled_by_build(args.build_directory, bsp)
    enable.extend(args.enable)
    timeout_key = _timeout_key(enable)
    command = [
        "specruntests", "--output",
        str(test_log), "--ctrf",
        str(output_directory / "report.ctrf.json"), "--app-name", bsp,
        "--target",
        _target_uid(bsp, target), "--timeout-key", timeout_key,
        # The runner logs the start of each test.  The log of a run which a
        # timeout stops names the tests which ran last.
        "--verbose"
    ]
    timeouts = _test_timeouts(bsp, target, timeout_key)
    if timeouts is not None:
        command.extend(["--test-timeouts", str(timeouts)])
    if reuse:
        command.extend(["--reuse", str(previous)])
    for name in enable:
        command.extend(["-E", name])
    for key, value in sorted(defines.items()):
        command.extend(["-D", f"{key}={value}"])
    command.append(str(runner))
    command.append(str(directory / "testsuites"))
    if args.verbose:
        print(" ".join(command), file=sys.stderr)
    # The test runner reports a failed test with a non zero status.  The
    # judgement is made here, so the status of the run is not an error.
    subprocess.run(command, check=False)
    if not test_log.is_file():
        raise RunTestsError(f"the test runner wrote no test log '{test_log}'")
    key_file.write_text(f"{tools_key}\n", encoding="utf-8")
    with open(test_log, "r", encoding="utf-8") as src:
        data = json.load(src)
    regressions, tolerated, passes, total = _reconcile(data, target)
    _report(bsp, regressions, tolerated, passes, total)
    return 1 if regressions else 0


def _timeouts(args: argparse.Namespace) -> int:
    """
    Update the test timeouts items from the saved test logs.

    The measured durations belong to the commit which they describe, so the
    items are tracked and move with a rebase.  The lazy mode keeps the
    maximum of each test, so a run which is no slower changes no item.
    """
    logs: list[str] = []
    for bsp in args.bsp:
        _get_target(bsp)
        test_log = Path(args.output_directory) / bsp / "test-log.json"
        if not test_log.is_file():
            raise RunTestsError(f"there is no test log '{test_log}'; run the "
                                f"tests of {bsp} first")
        logs.append(str(test_log))
    command = [
        "specupdatetimeouts", "--lazy", "--spec-directory",
        str(_SPEC_DIRECTORY)
    ]
    if args.reset:
        command.append("--reset")
    command.extend(logs)
    if args.verbose:
        print(" ".join(command), file=sys.stderr)
    return subprocess.run(command, check=False).returncode


def _configurations(_args: argparse.Namespace) -> int:
    """ List the special configurations with their BSP. """
    for name, configuration in sorted(CONFIGURATIONS.items()):
        print(f"{name}\t{configuration.bsp}")
    return 0


def _list(args: argparse.Namespace) -> int:
    """
    List the simulator targets.

    An architecture selects its BSPs.  A build of one architecture needs the
    names twice, once to build the tests and once to run them.
    """
    prefix = f"{args.arch}/" if args.arch else ""
    for bsp in sorted(TARGETS):
        if bsp.startswith(prefix):
            print(f"{bsp}\t{_runner(bsp)}")
    return 0


def main(argv: list[str]) -> int:
    """ Run the tests of an architecture on a simulator. """
    parser = argparse.ArgumentParser(description=main.__doc__)
    commands = parser.add_subparsers(dest="command", required=True)

    config = commands.add_parser(
        "config", help="print the build configuration file sections")
    config.add_argument("--all-bsps",
                        action="store_true",
                        help="read the BSP list from standard input and print "
                        "a section for every BSP")
    config.add_argument("--default",
                        action="append",
                        default=[],
                        metavar="KEY=VALUE",
                        help="add the build option to the DEFAULT section, "
                        "which reaches every BSP of the file")
    config.add_argument("--option",
                        action="append",
                        default=[],
                        metavar="KEY=VALUE",
                        help="add the build option to the section of each "
                        "BSP named on the command line; it overrides the "
                        "option of the target")
    config.add_argument("--configuration",
                        metavar="NAME",
                        help="add the section of the BSP of the special "
                        "configuration with its options")
    config.add_argument("bsp",
                        nargs="*",
                        metavar="ARCH/BSP",
                        help="the BSP to configure")
    config.set_defaults(function=_config)

    run = commands.add_parser("run", help="run the tests on the simulator")
    run.add_argument("--build-directory",
                     default="build",
                     help="the path to the build directory (default: build)")
    run.add_argument("--output-directory",
                     default="test-logs",
                     help="the path to the test log directory "
                     "(default: test-logs)")
    run.add_argument("--qemu-directory",
                     default=os.environ.get("RTEMS_QEMU_DIRECTORY",
                                            _DEFAULT_TOOLS),
                     help="the deployment directory of Qemu")
    run.add_argument("--sis-directory",
                     default=os.environ.get("RTEMS_SIS_DIRECTORY",
                                            _DEFAULT_TOOLS),
                     help="the deployment directory of SIS")
    run.add_argument("--gdb-sim-directory",
                     default=os.environ.get("RTEMS_GDB_SIM_DIRECTORY",
                                            _DEFAULT_TOOLS),
                     help="the deployment directory of the GDB simulators")
    run.add_argument("-D",
                     "--define",
                     action="append",
                     default=[],
                     metavar="KEY=VALUE",
                     help="map a substitution variable to a value; overrides "
                     "the value of the target")
    run.add_argument("-E",
                     "--enable",
                     action="append",
                     default=[],
                     metavar="NAME",
                     help="add the name to the enabled set of the test "
                     "runner item; the set holds the enabled set of the "
                     "build already")
    run.add_argument("--timeout-seconds",
                     type=float,
                     default=0.0,
                     help="override the timeouts of the test runner item; "
                     "use it to triage an architecture which hangs")
    run.add_argument("--timeout-scaler",
                     type=float,
                     default=0.0,
                     help="override the scaler of the measured test "
                     "durations of the test runner item; use it on a host "
                     "which is slower than the host which measured them")
    run.add_argument("--reuse",
                     action="store_true",
                     help="reuse the reports of the previous run for the "
                     "executables which did not change")
    run.add_argument("-v",
                     "--verbose",
                     action="store_true",
                     help="print the test runner command line")
    run.add_argument("--configuration",
                     metavar="NAME",
                     help="the special configuration of the build; it adds "
                     "the tests which it does not run")
    run.add_argument("bsp", metavar="ARCH/BSP", help="the BSP to run")
    run.set_defaults(function=_run)

    timeouts = commands.add_parser(
        "update-timeouts", help="update the test timeouts of the saved logs")
    timeouts.add_argument("--output-directory",
                          default="test-logs",
                          help="the path to the test log directory "
                          "(default: test-logs)")
    timeouts.add_argument("--reset",
                          action="store_true",
                          help="drop the measured durations and take those "
                          "of the logs; use it after a toolchain change")
    timeouts.add_argument("-v",
                          "--verbose",
                          action="store_true",
                          help="print the update command line")
    timeouts.add_argument("bsp",
                          nargs="*",
                          default=sorted(TARGETS),
                          metavar="ARCH/BSP",
                          help="the BSP to update (default: every BSP)")
    timeouts.set_defaults(function=_timeouts)

    listing = commands.add_parser("list", help="list the simulator targets")
    listing.add_argument("arch",
                         nargs="?",
                         default="",
                         metavar="ARCH",
                         help="list the BSPs of this architecture alone")
    listing.set_defaults(function=_list)

    configurations = commands.add_parser(
        "configurations", help="list the special configurations")
    configurations.set_defaults(function=_configurations)

    args = parser.parse_args(argv[1:])
    try:
        return args.function(args)
    except RunTestsError as err:
        print(f"error: {err}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv))
