#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
""" Extract the upstreamable commits of a pull request. """

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
import logging
from pathlib import Path
import subprocess
import sys
import tempfile

from inspect_changes import CATEGORY_SOURCE, get_category


def _git(*args: str, cwd: Path | str | None = None) -> str:
    return subprocess.check_output(["git", *args],
                                   cwd=cwd,
                                   encoding="utf-8",
                                   stderr=subprocess.DEVNULL)


def _git_ok(*args: str, cwd: Path | str | None = None) -> bool:
    return subprocess.run(["git", *args],
                          cwd=cwd,
                          check=False,
                          capture_output=True).returncode == 0


def _lines(text: str) -> list[str]:
    return [line for line in text.splitlines() if line.strip()]


def _get_repository_path() -> Path:
    repository = Path(".").absolute()
    while not (repository / ".git").exists():
        if repository.parent == repository:
            raise RuntimeError("no Git repository found")
        repository = repository.parent
    return repository


def _get_source_commits(base_ref: str, head_ref: str,
                        upstream_ref: str) -> list[tuple[str, str]]:
    """ Returns the upstreamable commits of the range. """
    log = _git("log", "--no-merges", "--reverse", "--format=format:%H %s",
               f"{base_ref}..{head_ref}", f"^{upstream_ref}")
    commits = []
    for line in _lines(log):
        commit, _, subject = line.partition(" ")
        files = _lines(
            _git("diff-tree", "--no-commit-id", "--name-only", "-r", commit))
        categories = {get_category(name) for name in files}
        if categories == {CATEGORY_SOURCE}:
            commits.append((commit, subject))
    return commits


def _cherry_pick(worktree: Path, commit: str, subject: str,
                 errors: list[str]) -> bool:
    if _git_ok("cherry-pick", commit, cwd=worktree):
        return True
    unmerged = _lines(
        _git("diff", "--name-only", "--diff-filter=U", cwd=worktree))
    if not unmerged:
        # The change is already present in the rtems.org repository.
        subprocess.run(["git", "cherry-pick", "--skip"],
                       cwd=worktree,
                       check=False,
                       capture_output=True)
        logging.info("skip empty commit %s %s", commit[:10], subject)
        return True
    subprocess.run(["git", "cherry-pick", "--abort"],
                   cwd=worktree,
                   check=False,
                   capture_output=True)
    errors.append(f"The commit `{commit[:10]}` (`{subject}`) does not apply. "
                  "Merge the rtems.org branch into the pull request branch "
                  "and resolve the conflicts in:\n\n```\n" +
                  "\n".join(unmerged) + "\n```")
    return False


def _verify(worktree: Path, errors: list[str]) -> bool:
    """ Verifies that the extracted tree is a consistent specification. """
    try:
        result = subprocess.run(["specverify", "spec"],
                                cwd=worktree,
                                check=False,
                                capture_output=True,
                                encoding="utf-8")
    except FileNotFoundError:
        errors.append("The specverify tool is not available, so the extracted "
                      "tree was not verified.")
        return False
    if result.returncode == 0:
        return True
    # A link to an item which stays in the eb repository surfaces here.  The
    # path based category cannot see it, since the file itself is upstreamable.
    detail = [
        line for line in result.stderr.splitlines()
        if "Error" in line or "error" in line or "links to" in line
    ]
    errors.append(
        "The extracted tree is not a consistent specification.  An "
        "upstreamable file most likely links to an item which stays in the "
        "eb repository:\n\n```\n" +
        "\n".join(detail or result.stderr.splitlines()[-5:]) + "\n```")
    return False


def main(argv: list[str]) -> int:
    """ Extracts the upstreamable commits onto a branch. """
    parser = argparse.ArgumentParser(description=sys.modules[__name__].__doc__)
    parser.add_argument("--upstream-ref",
                        default="rtems.org/main",
                        help="the rtems.org branch to extract onto")
    parser.add_argument("--branch",
                        required=True,
                        help="the upstreaming branch to create")
    parser.add_argument("--output", help="write the report to this file")
    parser.add_argument("base_ref", metavar="BASE_REF")
    parser.add_argument("head_ref", metavar="HEAD_REF")
    args = parser.parse_args(argv[1:])
    logging.basicConfig(level=logging.INFO, format="%(levelname)s %(message)s")
    repository = _get_repository_path()
    errors: list[str] = []
    report: list[str] = []
    commits = _get_source_commits(args.base_ref, args.head_ref,
                                  args.upstream_ref)
    if not commits:
        errors.append(
            "The pull request has no commit of the `source` category, so "
            "there is nothing to upstream.")
        commits = []
    picked: list[tuple[str, str]] = []
    if commits:
        with tempfile.TemporaryDirectory() as tmp_dir:
            worktree = Path(tmp_dir) / "extract"
            _git("worktree", "add", "--detach", str(worktree),
                 args.upstream_ref, cwd=repository)
            try:
                for commit, subject in commits:
                    if not _cherry_pick(worktree, commit, subject, errors):
                        break
                    picked.append((commit, subject))
                else:
                    if _verify(worktree, errors):
                        head = _git("rev-parse", "HEAD",
                                    cwd=worktree).strip()
                        _git("branch", "-f", args.branch, head,
                             cwd=repository)
                        logging.info("created %s at %s", args.branch,
                                     head[:10])
            finally:
                subprocess.run(
                    ["git", "worktree", "remove", "--force",
                     str(worktree)],
                    cwd=repository,
                    check=False,
                    capture_output=True)
    if picked:
        report.append(f"Extracted {len(picked)} commit(s) onto "
                      f"`{args.upstream_ref}`:\n")
        for commit, subject in picked:
            report.append(f"* `{commit[:10]}` {subject}")
        report.append("")
    for error in errors:
        report.append(f":x: {error}\n")
    text = "\n".join(report)
    if args.output:
        Path(args.output).write_text(text, encoding="utf-8")
    else:
        sys.stdout.write(text)
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
