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

import gzip
import io
import logging
from pathlib import Path
import shutil
import subprocess
import sys
import tarfile

from specitems import (ItemCacheConfig, JSONItemCache, get_arguments,
                       is_enabled)
from specmake.directorystate import DirectoryState
from specmake.pkgfactory import create_build_item_factory
from specmake.pkgitems import BuildItemTypeProvider, PackageBuildDirector

_KEEP = {
    "aarch64": ("/pkg/deployment/qemu", ),
    "arm": ("/pkg/deployment/qemu", ),
    "i386": ("/pkg/deployment/qemu", ),
    "microblaze": ("/pkg/deployment/qemu", ),
    "mips": ("/pkg/deployment/gdb-sim", "/pkg/deployment/qemu"),
    "moxie": ("/pkg/deployment/gdb-sim", ),
    "or1k": ("/pkg/deployment/qemu", ),
    "powerpc": ("/pkg/deployment/gdb-sim", ),
    "riscv": ("/pkg/deployment/qemu", "/pkg/deployment/sis"),
    "sparc": ("/pkg/deployment/sis", ),
    "x86_64": ("/pkg/deployment/qemu", ),
}


# The files which control the build of the tools, besides the specification
# items of the RTEMS repository.
_RECIPE = ("Makefile", "build_tools.py", "pyproject.toml", "spec-pkg-tools",
           "uv.lock")

_SOURCE_TYPES = ("pkg/directory-state/repository",
                 "pkg/directory-state/unpacked-archive")


def _rmdir(path: str) -> Path:
    the_path = Path(path).absolute()
    logging.info("recursively remove directory: %s", the_path)
    try:
        shutil.rmtree(the_path)
    except FileNotFoundError:
        pass
    return the_path


def _add_to_tar(tar_file: tarfile.TarFile, path: Path) -> None:
    info = tar_file.gettarinfo(str(path), str(path))
    info.mtime = 0
    info.uid = 0
    info.gid = 0
    info.uname = ""
    info.gname = ""
    if info.isreg():
        with open(path, "rb") as src:
            tar_file.addfile(info, src)
    else:
        tar_file.addfile(info)
    if info.isdir():
        for child in sorted(path.iterdir()):
            _add_to_tar(tar_file, child)


def _write_recipe(destination: Path) -> None:
    # The archive has no time stamp and no owner, so the same recipe yields
    # the same archive in every job.
    buffer = io.BytesIO()
    with tarfile.open(fileobj=buffer, mode="w",
                      format=tarfile.PAX_FORMAT) as tar_file:
        for name in _RECIPE:
            _add_to_tar(tar_file, Path(name))
    with open(destination, "wb") as dst:
        with gzip.GzipFile(filename="", fileobj=dst, mode="wb",
                           mtime=0) as compressed:
            compressed.write(buffer.getvalue())


def _write_patches(source: DirectoryState, stem: str,
                   source_directory: Path) -> list[str]:
    names: list[str] = []
    for patch in source.item["archive-patches"]:
        if not is_enabled(source.enabled_set, patch["enabled-by"]):
            continue
        name = f"{stem}-{len(names) + 1:04d}.patch"
        if patch["type"] == "inline":
            (source_directory / name).write_text(patch["patch"],
                                                 encoding="utf-8")
        else:
            shutil.copyfile(patch["file"], source_directory / name)
        names.append(name)
    return names


def _add_license_files(lines: list[str], source: DirectoryState) -> None:
    directory = Path(Path(source.directory).name)
    lines.extend(["", "=" * 79, f"Directory - {directory}", "=" * 79])
    for name in source.item["license-files"]["files"]:
        lines.extend(
            ["", "-" * 79, f"File - {directory / name}", "-" * 79, ""])
        lines.append((Path(source.directory) /
                      name).read_text(encoding="utf-8").rstrip())


def _distribute(spec_directory: Path, keep: list[str],
                distribution_directory: Path) -> None:
    """
    Write the corresponding source and the license summary of the kept
    directory states to the distribution directory.
    """
    item_cache = JSONItemCache(ItemCacheConfig(paths=[str(spec_directory)]),
                               type_provider=BuildItemTypeProvider({}))
    director = PackageBuildDirector(item_cache, "/pkg/component",
                                    create_build_item_factory())
    seen: set[str] = set()
    for uid in keep:
        director.create_with_dependencies(uid, seen)
    source_directory = distribution_directory / "source"
    source_directory.mkdir(parents=True)
    recipe = "recipe.tar.gz"
    _write_recipe(source_directory / recipe)
    commit = subprocess.run(["git", "rev-parse", "HEAD"],
                            check=True,
                            capture_output=True,
                            text=True).stdout.strip()
    readme = [
        "The directory source/ contains the corresponding source of the "
        "tools.", "LICENSES.txt contains the license files of the sources.",
        "", f"source/{recipe} contains the files which control the build:",
        ", ".join(_RECIPE) + ".",
        "The build used the specification items of the RTEMS repository at",
        f"commit {commit}.", "", "Sources:"
    ]
    licenses = [
        "The paths in this file are the paths in the archives of source/.  A "
        "file of", "the tools states the license of its work only.  This "
        "file states the", "licenses of the sources from which the tools "
        "were built."
    ]
    for uid in sorted(seen):
        source = director[uid]
        if source.item.type not in _SOURCE_TYPES:
            continue
        assert isinstance(source, DirectoryState)
        with source.component.scope():
            if source.item.type == "pkg/directory-state/repository":
                name = Path(source.directory).name
                archive = f"{name}-{source.item['commit']}.tar.gz"
                logging.info("%s: write archive: %s", uid, archive)
                subprocess.run([
                    "git", "-C", source.directory, "archive",
                    "--format=tar.gz", f"--prefix={name}/", "--output",
                    str((source_directory / archive).absolute()),
                    source.item["commit"]
                ],
                               check=True)
                readme.append(f"- {uid}: {archive}")
            else:
                archive = source.item["archive-file"]
                logging.info("%s: copy archive: %s", uid, archive)
                shutil.copyfile(
                    Path("src") / archive, source_directory / archive)
                patches = _write_patches(source,
                                         archive.split(".tar")[0],
                                         source_directory)
                readme.append(f"- {uid}: {archive}" + (
                    f", patched with {', '.join(patches)}" if patches else ""))
            _add_license_files(licenses, source)
    (distribution_directory / "README.txt").write_text("\n".join(readme) +
                                                       "\n",
                                                       encoding="utf-8")
    (distribution_directory / "LICENSES.txt").write_text("\n".join(licenses) +
                                                         "\n",
                                                         encoding="utf-8")


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
        parser.add_argument(
            "--rtems-version",
            help="the RTEMS version (default: 7)",
            default="7")
        parser.add_argument("--do-not-decimate",
                            action="store_true",
                            help="do not decimate the deployment directory "
                            "after building the tools")
        parser.add_argument(
            "--distribution-directory",
            help="write the corresponding source and the license summary of "
            "the tools to this directory")
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
accepted-licenses:
- BSD-2-Clause
ident: workspace
name: rtems
rtems-version: '{args.rtems_version}'
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
license: CC-BY-SA-4.0
links: []
pkg-type: workspace
type: pkg
workspace-type: component
""",
        encoding="utf-8")
    # The package states the licenses of its works.  The specification items
    # provide the license items where they exist, otherwise the BSP package
    # configuration does.
    for name in ["bsd-2-clause.yml", "cc-by-sa-4.0.yml"]:
        if not (Path("spec") / "license" / name).exists():
            license_item = tools_config_directory / "license" / name
            logging.info("copy license item: %s", license_item)
            license_item.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(
                Path("config-bsps") / "spec" / "license" / name, license_item)
    archive = spec_pkg / "deployment" / "archive.yml"
    logging.info("create disabled archive: %s", archive)
    archive.parent.mkdir(parents=True)
    archive.write_text(
        """SPDX-License-Identifier: CC-BY-SA-4.0 OR BSD-2-Clause
archive-file: ${.:/component/package-directory:basename}.tar.xz
archive-strip-prefix: ${.:/component/prefix-directory}/
copyrights:
- Copyright (C) 2026 embedded brains GmbH & Co. KG
directory: ${.:/component/deployment-directory}
directory-state-type: archive
enabled-by: false
files: []
hash: null
license-info: []
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
        keep.add(f"/pkg/deployment/dtc")
        keep.update(_KEEP.get(arch, set()))
    cmd = ["specbuild"]
    if not args.use_git:
        cmd.append("--do-not-use-git")
    cmd.extend(filter(lambda x: x.startswith("--log-"), argv))
    cmd.extend(["spec", "spec-pkg-tools", str(tools_config_directory)])
    logging.info("run: %s", " ".join(cmd))
    subprocess.run(cmd, check=True)
    deployment_directory = tools_directory / args.rtems_version
    spec_directory = deployment_directory / "build" / "spec"
    if args.distribution_directory:
        distribution_directory = _rmdir(args.distribution_directory)
        _distribute(spec_directory, sorted(keep), distribution_directory)
    if not args.do_not_decimate:
        cmd = [
            "specmakedecimate", "--spec-directory",
            str(spec_directory),
        ] + sorted(keep)
        logging.info("run: %s", " ".join(cmd))
        subprocess.run(cmd, check=True)
    if args.distribution_directory:
        shutil.copyfile(distribution_directory / "LICENSES.txt",
                        deployment_directory / "LICENSES.txt")


if __name__ == "__main__":
    main(sys.argv)
