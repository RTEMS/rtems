#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
""" Resolve the mechanical conflicts of a merge from the rtems.org repository. """

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
# The eb repository builds a qualified subset of RTEMS.  It therefore deletes
# large parts of the build specification which the rtems.org repository keeps
# extending.  Every extension inside a deleted region conflicts as a
# delete-versus-modify hunk whose 'ours' side is empty.
#
# Such a hunk is resolved in favour of 'ours'.  This encodes the policy that a
# new source of the rtems.org repository stays outside the qualification scope
# until it is added deliberately.  Silently inheriting it would change what
# gets certified.
#
# Every other hunk is left untouched.  A hunk with content on both sides is a
# qualification scope decision and must be made by a human.

import argparse
import pathlib
import subprocess
import sys
import textwrap

_BEGIN = "<<<<<<< "

_BASE = "||||||| "

_SEPARATOR = "======="

_END = ">>>>>>> "


def _is(line: str, marker: str) -> bool:
    return line.startswith(marker)


def _is_separator(line: str) -> bool:
    return line.rstrip("\r\n") == _SEPARATOR


def _parse(lines: list[str]) -> list[tuple]:
    """ Splits the lines into text blocks and conflict hunks. """
    blocks: list[tuple] = []
    text: list[str] = []
    index = 0
    count = len(lines)
    while index < count:
        if not _is(lines[index], _BEGIN):
            text.append(lines[index])
            index += 1
            continue
        if text:
            blocks.append(("text", text))
            text = []
        begin = index
        index += 1
        ours: list[str] = []
        while index < count and not _is(lines[index],
                                       _BASE) and not _is_separator(
                                           lines[index]):
            ours.append(lines[index])
            index += 1
        # The diff3 and zdiff3 conflict styles add the merge base.
        if index < count and _is(lines[index], _BASE):
            index += 1
            while index < count and not _is_separator(lines[index]):
                index += 1
        theirs: list[str] = []
        if index < count and _is_separator(lines[index]):
            index += 1
            while index < count and not _is(lines[index], _END):
                theirs.append(lines[index])
                index += 1
        if index >= count:
            raise RuntimeError("unterminated conflict hunk")
        index += 1
        blocks.append(("conflict", ours, theirs, lines[begin:index]))
    if text:
        blocks.append(("text", text))
    return blocks


def _resolve(path: str) -> tuple[str, int, int]:
    """ Returns the resolved content and the hunk counts of the file. """
    with open(path, "r", encoding="utf-8", errors="surrogateescape") as src:
        lines = src.read().splitlines(keepends=True)
    out: list[str] = []
    resolved = 0
    remaining = 0
    for block in _parse(lines):
        if block[0] == "text":
            out.extend(block[1])
            continue
        _, ours, _theirs, raw = block
        if ours:
            remaining += 1
            out.extend(raw)
        else:
            resolved += 1
    return "".join(out), resolved, remaining


# A build item lists its sources either in a flat "source:" list or inside
# "install:" groups keyed by a destination.  The files are generated, so the
# layout is regular enough to edit by line, which keeps the rest of the file
# byte for byte as it was.
#
#   install:
#   - destination: ${BSP_INCLUDEDIR}/bsp
#     source:
#     - bsps/include/bsp/fdt.h
#   source:
#   - bsps/shared/dev/i2c/eeprom.c

_EXTRA = "objextra.yml"


def _entries(text: str) -> set[tuple[str, str]]:
    """ Returns the (destination, source) pairs of a build item.  The flat
    source list uses an empty destination. """
    found: set[tuple[str, str]] = set()
    where = None
    destination = ""
    for line in text.splitlines():
        if line == "source:":
            where, destination = "flat", ""
        elif line == "install:":
            where = "install"
        elif line.startswith("- destination: ") and where == "install":
            destination = line[len("- destination: "):]
        elif line == "  source:" and where == "install":
            pass
        elif where == "flat" and line.startswith("- "):
            found.add(("", line[2:]))
        elif where == "install" and line.startswith("  - "):
            found.add((destination, line[4:]))
        elif line and not line[0].isspace() and not line.startswith("- "):
            where = None
    return found


def _is_key(line: str) -> bool:
    """ Returns true, if the line starts a top level key of the item. """
    return bool(line) and not line[0].isspace() and not line.startswith("- ")


def _slots(
    lines: list[str]
) -> tuple[dict[int, str], dict[str, tuple[int, str]], int]:
    """ Returns the key of every entry line, for every list the index of its
    last entry with the prefix to use for a new one, and the index of the last
    line of the install list. """
    key_of: dict[int, str] = {}
    last: dict[str, tuple[int, str]] = {}
    install_last = -1
    where = None
    destination = ""
    for index, line in enumerate(lines):
        if line in ("source:", "  source:"):
            where, destination = ("flat", "") if line == "source:" else (
                "install", destination)
            continue
        if line == "install:":
            where, destination = "install", ""
            continue
        if where == "install" and line.startswith("- destination: "):
            destination = line[len("- destination: "):]
            install_last = index
            continue
        if _is_key(line) or not line.strip():
            where, destination = None, ""
            continue
        if where and line.lstrip().startswith("- "):
            key = "" if where == "flat" else destination
            key_of[index] = key
            last[key] = (index, line[:line.index("- ")] + "- ")
            if where == "install":
                install_last = index
    return key_of, last, install_last


def _edit(text: str, add: set[tuple[str, str]],
          drop: set[tuple[str, str]]) -> tuple[str, int, int]:
    """ Adds and drops entries of a build item.  Only the lines of an added or
    a dropped entry are touched, so a file with nothing to change comes back
    byte for byte as it was. """
    lines = text.splitlines()
    key_of, last, install_last = _slots(lines)
    out: list[str] = []
    added = dropped = 0
    for index, line in enumerate(lines):
        key = key_of.get(index)
        if key is not None:
            source = line.lstrip()[2:]
            if (key, source) in drop:
                dropped += 1
                continue
        out.append(line)
        if key is not None and last.get(key, (None, ""))[0] == index:
            prefix = last[key][1]
            for source in sorted(s for d, s in add if d == key):
                out.append(f"{prefix}{source}")
                added += 1
        # A destination which the item does not have yet needs its own group,
        # placed at the end of the install list rather than the end of the
        # file, where it would read as a flat source entry.
        if index == install_last:
            for destination in sorted({d for d, _ in add
                                       if d and d not in last}):
                out.append(f"- destination: {destination}")
                out.append("  source:")
                for source in sorted(s for d, s in add if d == destination):
                    out.append(f"  - {source}")
                    added += 1
    # Keep the file exactly as it was, including a missing final newline.
    return "\n".join(out) + ("\n" if text.endswith("\n") else ""), added, dropped


def _stage(number: int, path: str) -> str | None:
    result = subprocess.run(["git", "show", f":{number}:{path}"],
                            check=False,
                            capture_output=True,
                            encoding="utf-8")
    return result.stdout if result.returncode == 0 else None


def _merge_build_item(path: str, dry_run: bool) -> str | None:
    """ Merges a conflicting build item by moving what the rtems.org
    repository added or removed into or out of its objextra sibling.  Returns a
    report line, or None if the item has no such sibling. """
    sibling = str(pathlib.PurePosixPath(path).parent / _EXTRA)
    if path.endswith(_EXTRA) or not pathlib.Path(sibling).is_file():
        return None
    if sibling in _conflicted_files():
        # Editing a file which still holds conflict markers would write them
        # into the result.  Leave both to the hunk rule.
        return None
    stages = [_stage(n, path) for n in (1, 2, 3)]
    if any(s is None for s in stages):
        return None
    base, ours, theirs = (_entries(s) for s in stages)
    before = pathlib.Path(sibling).read_text(encoding="utf-8")
    extra = _entries(before)
    add = (theirs - base) - ours - extra
    drop = (base - theirs) & extra
    text, added, dropped = _edit(before, add, drop)
    # Never let an entry go missing quietly.  A destination which the sibling
    # does not have yet is the case which is easy to drop by accident.
    result = _entries(text)
    lost = add - result
    kept = drop & result
    if lost or kept:
        raise RuntimeError(f"{sibling}: {len(lost)} entries were not added and "
                           f"{len(kept)} were not removed: "
                           f"{sorted(lost | kept)[:4]}")
    if not dry_run:
        pathlib.Path(sibling).write_text(text, encoding="utf-8")
        subprocess.run(["git", "add", "--", sibling], check=True)
        pathlib.Path(path).write_text(stages[1], encoding="utf-8")
        subprocess.run(["git", "add", "--", path], check=True)
    return _fill(f"{path} is unchanged; its sibling {sibling} gained "
                 f"{added} and lost {dropped} entries.")


def _conflicted_files() -> list[str]:
    stdout = subprocess.check_output(
        ["git", "diff", "--name-only", "--diff-filter=U"], encoding="utf-8")
    return [line for line in stdout.splitlines() if line.strip()]


# The report becomes the message of the merge commit, so it is plain text
# wrapped at 72 columns and carries no markup.  GitHub shows the same text as
# the body of the pull request, where it is rendered as Markdown, so every
# entry is a paragraph of its own.  A line break inside a paragraph is not one
# there, and a trailing space to force one does not survive the commit.
_MOVED = """\
The eb repository builds a qualified subset, so it moves whatever is
outside that subset from a build item into its objextra sibling.  The
rtems.org repository keeps changing the original item, and every such
change conflicts.  Each conflict below was resolved by keeping the
qualified subset exactly as it is and applying what the rtems.org
repository did to the objextra sibling instead.  The build item itself
is unchanged and therefore absent from the diff."""

_DECLINED = """\
A conflict inside a region which the eb repository deleted was
declined, so an upstream build change does not enter the qualified
subset by itself."""


def _hunks(count: int) -> tuple[str, str]:
    """ Returns the hunk count and its verb. """
    if count == 1:
        return "1 hunk", "was"
    return f"{count} hunks", "were"


def _fill(text: str) -> str:
    """ Returns the text wrapped as one paragraph of the report. """
    # A path is longer than what is left of the line more often than not, so
    # the continuation is indented to keep the entry readable.
    return textwrap.fill(text, width=72, subsequent_indent="  ")


def main(argv: list[str]) -> int:
    """ Resolves the mechanical conflicts of the in-progress merge. """
    parser = argparse.ArgumentParser(description=sys.modules[__name__].__doc__)
    parser.add_argument("--dry-run",
                        action="store_true",
                        help="do not write the files and do not stage them")
    args = parser.parse_args(argv[1:])
    files = _conflicted_files()
    if not files:
        # Anything which is not the report goes to stderr.  Whatever this
        # writes to stdout becomes the message of the merge commit.
        print("There are no conflicted files.", file=sys.stderr)
        return 0
    moved: list[str] = []
    declined: list[str] = []
    blocked = False
    for path in files:
        try:
            line = _merge_build_item(path, args.dry_run)
        except RuntimeError as err:
            print(f"The merge of {path} could not be applied: {err}",
                  file=sys.stderr)
            return 1
        if line is not None:
            moved.append(line)
            continue
        content, resolved, remaining = _resolve(path)
        hunks, was = _hunks(resolved)
        if remaining:
            blocked = True
            need = "needs" if remaining == 1 else "need"
            declined.append(
                _fill(f"{hunks} of {path} {was} declined; {remaining} {need} "
                      "a qualification scope decision."))
            continue
        declined.append(
            _fill(f"{hunks} of {path} {was} declined and the file is "
                  "unchanged."))
        if args.dry_run:
            continue
        with open(path, "w", encoding="utf-8",
                  errors="surrogateescape") as dst:
            dst.write(content)
        subprocess.run(["git", "add", "--", path], check=True)
    sections = [
        "\n\n".join([intro] + lines)
        for intro, lines in ((_MOVED, moved), (_DECLINED, declined)) if lines
    ]
    if sections:
        print("\n\n".join(sections))
    return 1 if blocked else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
