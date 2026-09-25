#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
""" Compare the delivered specification commits with their copies. """

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
import subprocess
import sys

# The specification commits of the RTEMS Qualification Data Package which the
# European Space Agency published.  The range starts at the initial import and
# ends at the last item change.
DELIVERED_FIRST = "a40216a0aeeacb489105a512cd4bff3ffdcab2fa"

DELIVERED_LAST = "5d24a4163e70504bb5c0f67d875e3c8389313aae"

# The properties which a copy must reproduce.  The committer and the parent
# change, so the object name of a copy differs from the object name of the
# delivered commit.
_FORMAT = "%H%x01%an%x01%ae%x01%aI%x01%B%x02"

_FIELDS = ("author", "email", "author date", "message")


def _git(*args: str) -> str:
    return subprocess.check_output(["git", *args], encoding="utf-8")


def _properties(commits: list[str]) -> dict[str, tuple[str, ...]]:
    text = subprocess.run(["git", "log", "--no-walk", f"--format={_FORMAT}",
                           "--stdin"],
                          input="\n".join(commits),
                          capture_output=True,
                          check=True,
                          encoding="utf-8").stdout
    result = {}
    for record in text.split("\x02"):
        if record.strip():
            fields = record.strip("\n").split("\x01", 4)
            result[fields[0]] = tuple(fields[1:])
    return result


def _patch_id(commit: str) -> str:
    patch = _git("show", "--format=", "--patch", commit)
    text = subprocess.run(["git", "patch-id", "--stable"],
                          input=patch,
                          capture_output=True,
                          check=True,
                          encoding="utf-8").stdout.split()
    return text[0] if text else ""


def _delivered() -> list[str]:
    return _git("rev-list", "--reverse",
                f"{DELIVERED_FIRST}^..{DELIVERED_LAST}").split()


def _copies(ref: str, base: str) -> list[str]:
    """ Returns the commits of the ref above the base which change a
    specification item outside of the build specification. """
    commits = _git("rev-list", "--reverse", "--no-merges", f"{base}..{ref}",
                   "--", "spec/").split()
    result = []
    for commit in commits:
        files = _git("show", "--pretty=format:", "--name-only",
                     commit).split()
        if files and all(
                name.startswith("spec/") and not name.startswith("spec/build/")
                for name in files):
            result.append(commit)
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("ref", help="the ref which carries the copies")
    parser.add_argument("base", help="the ref below the copies")
    args = parser.parse_args()
    delivered = _delivered()
    copies = _copies(args.ref, args.base)[:len(delivered)]
    if len(copies) != len(delivered):
        print(f"error: {args.ref} carries {len(copies)} specification commits "
              f"above {args.base}, {len(delivered)} are needed")
        return 1
    properties = _properties(delivered + copies)
    failures = 0
    for source, copy in zip(delivered, copies):
        differences = [
            name for index, name in enumerate(_FIELDS)
            if properties[source][index] != properties[copy][index]
        ]
        if _patch_id(source) != _patch_id(copy):
            differences.append("patch")
        if differences:
            failures += 1
            print(f"error: {copy[:11]} differs from the delivered "
                  f"{source[:11]} in the {', '.join(differences)}")
    print(f"{len(delivered) - failures} of {len(delivered)} delivered "
          "specification commits are reproduced")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
