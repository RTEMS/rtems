#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
""" Check that no build item refers to a deleted file. """

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
#
# This repository moves whatever is outside the qualified subset from a build
# item into its objextra sibling.  When the rtems.org repository then deletes
# such a file, the deletion merges without a conflict and the sibling is left
# pointing at a file which no longer exists.  The build specification is then
# broken in a way which no diff shows.
#
# Only the files deleted by the change are looked at, so the check is cheap
# and reports what the change did rather than what the tree already carried.

import argparse
from pathlib import Path
import subprocess
import sys
import textwrap

_SPEC = "spec/build"


def deleted_files(base_ref: str, head_ref: str | None = None,
                  cwd: Path | str | None = None) -> list[str]:
    """ Returns the files deleted between the base reference and the head
    reference, or the work tree if there is no head reference. """
    # Without --no-renames a moved file is reported as a rename rather than a
    # deletion, and a move is exactly how the rtems.org repository relocates
    # sources.  The old path has to count as deleted.
    command = [
        "git", "diff", "--name-only", "--no-renames", "--diff-filter=D",
        base_ref
    ]
    if head_ref:
        command.append(head_ref)
    stdout = subprocess.check_output(command, cwd=cwd, encoding="utf-8")
    return [line for line in stdout.splitlines() if line.strip()]


def find_references(paths: list[str],
                    cwd: Path | str | None = None) -> dict[str, list[str]]:
    """ Returns the build items which still list each of the paths. """
    found: dict[str, list[str]] = {}
    if not paths:
        return found
    root = Path(cwd or ".") / _SPEC
    if not root.is_dir():
        return found
    wanted = {path: f"- {path}" for path in paths}
    for item in root.rglob("*.yml"):
        try:
            lines = item.read_text(encoding="utf-8",
                                   errors="replace").splitlines()
        except OSError:
            continue
        listed = {line.strip() for line in lines if line.lstrip().startswith("- ")}
        for path, entry in wanted.items():
            if entry in listed:
                found.setdefault(path, []).append(
                    str(item.relative_to(Path(cwd or "."))))
    return found


def report(found: dict[str, list[str]]) -> str:
    """ Returns a plain text report of the dangling references. """
    # Every entry is a paragraph of its own, since the report is shown as
    # Markdown as well, where a line break inside a paragraph is not one.
    parts = [
        "The change deletes files which a build item still lists, so the\n"
        "build specification refers to files which do not exist."
    ]
    parts += [
        textwrap.fill(f"{item} still lists {path}.",
                      width=72,
                      subsequent_indent="  ") for path in sorted(found)
        for item in sorted(found[path])
    ]
    return "\n\n".join(parts)


def main(argv: list[str]) -> int:
    """ Reports build items which refer to a file the change deleted. """
    parser = argparse.ArgumentParser(description=sys.modules[__name__].__doc__)
    parser.add_argument("base_ref", metavar="BASE_REF")
    parser.add_argument("head_ref", metavar="HEAD_REF", nargs="?")
    args = parser.parse_args(argv[1:])
    deleted = deleted_files(args.base_ref, args.head_ref)
    found = find_references(deleted)
    # The report becomes part of the message of the merge commit, so it is
    # plain text wrapped at 72 columns and carries no markup.
    if not found:
        if len(deleted) > 1:
            print(f"None of the {len(deleted)} deleted files is listed by a "
                  "build item.")
        elif deleted:
            print("The deleted file is not listed by a build item.")
        else:
            print("The change deletes no file.")
        return 0
    print(report(found))
    return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
