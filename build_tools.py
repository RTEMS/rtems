#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
""" Provide a command line interface to build tools. """

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

import logging
from pathlib import Path
import shutil
import subprocess
import sys

from specitems import get_arguments

_KEEP = {
    "aarch64": ("/pkg/deployment/qemu", ),
    "arm": ("/pkg/deployment/qemu", ),
    "riscv": ("/pkg/deployment/qemu", "/pkg/deployment/sis"),
    "sparc": ("/pkg/deployment/sis", ),
}


def _rmdir(path: str) -> Path:
    the_path = Path(path).absolute()
    logging.info("recursively remove directory: %s", the_path)
    try:
        shutil.rmtree(the_path)
    except FileNotFoundError:
        pass
    return the_path


def main(argv: list[str]) -> None:
    """ Build the tools for the specified architectures. """

    def _add_arguments(parser):
        parser.add_argument(
            "--tools-directory",
            help="the path to the tools directory (default: tools)",
            default="tools")
        parser.add_argument(
            "--tools-configuration-directory",
            help=
            "the path to the tools configuration directory (default: config-tools)",
            default="config-tools")
        parser.add_argument("--do-not-decimate",
                            action="store_true",
                            help="do not decimate the deployment directory "
                            "after building the tools")
        parser.add_argument("--use-git",
                            help="use git to track changes in the workspace",
                            action="store_true")
        parser.add_argument("archs",
                            metavar="ARCH",
                            nargs="+",
                            help="the tool architecture")

    args = get_arguments(argv[1:],
                         description=sys.modules[__name__].__doc__,
                         add_arguments=(_add_arguments, ))
    tools_directory = _rmdir(args.tools_directory)
    tools_config_directory = _rmdir(args.tools_configuration_directory)
    spec_pkg = tools_config_directory / "pkg"
    component = spec_pkg / "component.yml"
    logging.info("create package component: %s", component)
    component.parent.mkdir(parents=True)
    component.write_text(
        f"""SPDX-License-Identifier: CC-BY-SA-4.0 OR BSD-2-Clause
name: rtems
rtems-version: '6'
prefix-directory: {tools_directory}
package-directory: ${{.:/rtems-version}}
package-version: ''
pkgversion: ${{.:/name}}/${{.:/rtems-version}}
deployment-directory: ${{.:/prefix-directory}}/${{.:/package-directory}}
build-directory: ${{.:/deployment-directory}}/build
copyrights:
- Copyright (C) 2026 embedded brains GmbH & Co. KG
component-type: generic
enabled-by: true
enabled-set:
- pkg.feature.strip-target-libraries
enabled-set-actions: []
enabled-set-feedback-mapping: {{}}
links: []
pkg-type: workspace
type: pkg
workspace-type: component
""",
        encoding="utf-8")
    archive = spec_pkg / "deployment" / "archive.yml"
    logging.info("create disabled archive: %s", archive)
    archive.parent.mkdir(parents=True)
    archive.write_text(
        """SPDX-License-Identifier: CC-BY-SA-4.0 OR BSD-2-Clause
archive-file: ${.:/component/package-directory:basename}.tar.xz
archive-strip-prefix: ${.:/component/prefix-directory}/
copyrights:
- Copyright (C) 2026 embedded brains GmbH & Co. KG
copyrights-by-license: {}
directory: ${.:/component/deployment-directory}
directory-state-type: archive
enabled-by: false
files: []
hash: null
links:
- hash: null
  name: component
  role: input
  uid: ../component
pkg-type: directory-state
type: pkg
verification-script: verify_package.py
""",
        encoding="utf-8")
    keep: set[str] = {"/pkg/deployment/gdb-multiarch", "/pkg/deployment/rtems-tools"}
    for arch in args.archs:
        arch_component = spec_pkg / arch / "component.yml"
        arch_component.parent.mkdir(parents=True)
        logging.info("create %s component: %s", arch, arch_component)
        arch_component.write_text(
            f"""SPDX-License-Identifier: CC-BY-SA-4.0 OR BSD-2-Clause
arch: {arch}
binutils-install-excludes:
- '*/info/*'
- '*/man/*'
gcc-install-excludes:
- '*/info/*'
- '*/man/*'
gcc-reported-version: 13.4.0
gcc-version: 13.4.0
copyrights:
- Copyright (C) 2026 embedded brains GmbH & Co. KG
component-type: generic
enabled-by: true
enabled-set: []
enabled-set-actions: []
enabled-set-feedback-mapping: {{}}
ident: ${{.:/component/name}}/{arch}
links:
- role: use-package-component-template
  uid: /pkg/template/arch/component
- hash: null
  name: component
  role: input
  uid: ../component
pkg-type: workspace
type: pkg
workspace-type: component
""",
            encoding="utf-8")
        keep.add(f"/pkg/{arch}/binutils")
        keep.add(f"/pkg/{arch}/gcc")
        keep.update(_KEEP.get(arch, set()))
    cmd = ["specbuild"]
    if not args.use_git:
        cmd.append("--do-not-use-git")
    cmd.extend(filter(lambda x: x.startswith("--log-"), argv))
    cmd.extend(["spec", "spec-pkg-tools", str(tools_config_directory)])
    logging.info("run: %s", " ".join(cmd))
    subprocess.run(cmd, check=True)
    if not args.do_not_decimate:
        cmd = [
            "specmakedecimate", "--spec-directories",
            str(tools_directory / "6" / "build" / "spec"), "--"
        ] + sorted(keep)
        logging.info("run: %s", " ".join(cmd))
        subprocess.run(cmd, check=True)


if __name__ == "__main__":
    main(sys.argv)
