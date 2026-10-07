#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
""" Provide a command line interface to inspect change sets. """

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
import re
import shutil
import subprocess
import sys
import tempfile

from check_sources import find_references
from specitems import CommonMarkContent, get_arguments

_OK = ":white_check_mark:"

_WARNING = ":warning:"

_ERROR = ":x:"

_SKIP = ":heavy_minus_sign:"

# The options of the export check.  All generated code is committed in the
# clang-format style of the repository.
_EXPORT_OPTIONS = ("--format-code", "--no-documentation")

# The change set categories.  Only CATEGORY_SOURCE is upstreamable to the
# rtems.org repository.
CATEGORY_SOURCE = "source"

CATEGORY_SPEC = "spec"

CATEGORY_PKG = "pkg"

CATEGORY_CI = "ci"

CATEGORY_BUILD_QUAL = "build-qual"

CATEGORY_UNKNOWN = "unknown"

# A merge commit has no category.  The table shows this name instead.
_MERGE = "merge"

# Maps a path prefix to a category.  A prefix ending with a slash matches a
# directory and everything below it, otherwise it matches a file exactly.  The
# longest matching prefix wins, so 'spec/build/' overrides 'spec/'.
_CATEGORIES: dict[str, str] = {
    # The RTEMS source tree.  These paths exist in the rtems.org repository and
    # changes to them are upstreamable.
    "CODEOWNERS": CATEGORY_SOURCE,
    "Doxyfile": CATEGORY_SOURCE,
    "LICENSE.md": CATEGORY_SOURCE,
    "README.md": CATEGORY_SOURCE,
    "_clang-format": CATEGORY_SOURCE,
    "bsps/": CATEGORY_SOURCE,
    "contrib/": CATEGORY_SOURCE,
    "cpukit/": CATEGORY_SOURCE,
    "gccdeps.py": CATEGORY_SOURCE,
    "make/": CATEGORY_SOURCE,
    "rtems-bsps": CATEGORY_SOURCE,
    "rtemslogo.png": CATEGORY_SOURCE,
    "spec/build/": CATEGORY_SOURCE,
    "testsuites/": CATEGORY_SOURCE,
    "waf": CATEGORY_SOURCE,
    "wscript": CATEGORY_SOURCE,
    "yaml/": CATEGORY_SOURCE,
    ".gitlab/": CATEGORY_SOURCE,
    # The specification items.  They are not upstreamed.
    "spec/": CATEGORY_SPEC,
    # The packaging and qualification machinery.  These paths are local to the
    # eb repository.
    ".gitattributes": CATEGORY_PKG,
    ".gitignore": CATEGORY_PKG,
    ".mdformat.toml": CATEGORY_PKG,
    "AGENTS.md": CATEGORY_PKG,
    "CONTRIBUTING.md": CATEGORY_PKG,
    "DCO.txt": CATEGORY_PKG,
    "LICENSE.LLVM": CATEGORY_PKG,
    "Makefile": CATEGORY_PKG,
    "build_tools.py": CATEGORY_PKG,
    "config-bsps/": CATEGORY_PKG,
    "config-tools/": CATEGORY_PKG,
    "pyproject.toml": CATEGORY_PKG,
    "run_tests.py": CATEGORY_PKG,
    "spec-pkg-bsps/": CATEGORY_PKG,
    "spec-pkg-tools/": CATEGORY_PKG,
    "specitems.yml": CATEGORY_PKG,
    "src/": CATEGORY_PKG,
    "test-logs/": CATEGORY_PKG,
    "testsuites/isvv/": CATEGORY_PKG,
    "testsuites/membench/": CATEGORY_PKG,
    # The build items which register the two eb-only test suites.  Without
    # these the build specification of the rtems.org repository would reference
    # test programs whose sources are not upstreamed.
    "spec/build/testsuites/isvv/": CATEGORY_PKG,
    "spec/build/testsuites/membench/": CATEGORY_PKG,
    "spec/build/testsuites/optisvv.yml": CATEGORY_PKG,
    "spec/build/testsuites/optmembench.yml": CATEGORY_PKG,
    "uv.lock": CATEGORY_PKG,
    # The continuous integration.  All scripts live below .github so that the
    # category of a change to them is unambiguous.
    ".github/": CATEGORY_CI,
}

# The build items of the pre-qualified build.  They do not exist in the
# rtems.org repository, so the pattern takes precedence over 'spec/build/'.
_BUILD_QUAL = re.compile(r"spec/build/.*(?:extra|qual)\.yml")

# A commit of one of these categories has the subject '<category>: ...'.  No
# commit of another category uses one of these subject prefixes.
_SUBJECT_CATEGORIES = (CATEGORY_SPEC, CATEGORY_PKG, CATEGORY_CI,
                       CATEGORY_BUILD_QUAL)

# Checked from the longest prefix to the shortest one.
_CATEGORY_PREFIXES: tuple[tuple[str, str], ...] = tuple(
    sorted(_CATEGORIES.items(), key=lambda item: len(item[0]), reverse=True))

_C_SUFFIXES = (".c", ".h")

# A change to one of these files changes the formatting of all items.  The
# uv.lock file pins the formatter and the specification types.
_FORMATTER_INPUTS = ("_clang-format", "uv.lock")


def get_category(path: str) -> str:
    """ Returns the category of the path. """
    if _BUILD_QUAL.fullmatch(path):
        return CATEGORY_BUILD_QUAL
    for prefix, category in _CATEGORY_PREFIXES:
        if prefix.endswith("/"):
            if path.startswith(prefix):
                return category
        elif path == prefix:
            return category
    return CATEGORY_UNKNOWN


def get_source_prefixes() -> list[str]:
    """ Returns the path prefixes of the upstreamable category. """
    return sorted(prefix for prefix, category in _CATEGORIES.items()
                  if category == CATEGORY_SOURCE)


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


def _restore(worktree: Path) -> None:
    """ Undoes what a check did to the worktree.  The export and the formatter
    write files in place, and the untracked ones would otherwise pile up from
    one commit to the next. """
    _git("checkout", "--", ".", cwd=worktree)
    subprocess.run(["git", "clean", "-fdq"],
                   cwd=worktree,
                   check=False,
                   capture_output=True)


class _Findings:
    """ Collects the errors and warnings of the inspection. """

    def __init__(self) -> None:
        self.content = CommonMarkContent(context="CC-BY-SA-4.0")
        self.error_count = 0

    def error(self, message: str, detail: str = "") -> None:
        """ Adds an error. """
        self.error_count += 1
        self._add(_ERROR, message, detail)

    def warning(self, message: str, detail: str = "") -> None:
        """ Adds a warning. """
        self._add(_WARNING, message, detail)

    def _add(self, mark: str, message: str, detail: str) -> None:
        self.content.add(f"{mark} {message}")
        if detail:
            self.content.add_code_block(_lines(detail), language="none")


def _get_rows(base_ref: str, head_ref: str,
              exclude_ref: str | None) -> list[tuple[str, bool, str]]:
    """ Returns the commits of the first-parent chain and whether each one is
    a merge.  The commits which a merge brings in were inspected on their
    own branch. """
    exclude = [f"^{exclude_ref}"] if exclude_ref else []
    log = _git("log", "--first-parent", "--reverse",
               "--format=format:%H %P%x00%s", f"{base_ref}..{head_ref}",
               *exclude)
    rows = []
    for line in _lines(log):
        hashes, _, subject = line.partition("\0")
        commit, *parents = hashes.split()
        rows.append((commit, len(parents) > 1, subject))
    return rows


def _get_commits(base_ref: str, head_ref: str,
                 exclude_ref: str | None) -> list[tuple[str, str]]:
    # Commits which are already in the rtems.org repository are not the
    # responsibility of this repository.  Excluding them keeps the merge pull
    # requests of Harmonia from being judged by the rules of the eb repository.
    exclude = [f"^{exclude_ref}"] if exclude_ref else []
    log = _git("log", "--no-merges", "--reverse", "--format=format:%H %s",
               f"{base_ref}..{head_ref}", *exclude)
    commits = []
    for line in _lines(log):
        commit, _, subject = line.partition(" ")
        commits.append((commit, subject))
    return commits


def _get_files(commit: str) -> list[str]:
    return sorted(
        _lines(
            _git("diff-tree", "--no-commit-id", "--name-only", "-r", commit)))


def _get_existing_files(commit: str, files: list[str]) -> list[str]:
    return [f for f in files if _git_ok("cat-file", "-e", f"{commit}:{f}")]


def _check_invariant(upstream_ref: str, findings: _Findings) -> None:
    """ Checks that the category table matches the two trees. """
    for prefix in get_source_prefixes():
        path = prefix.rstrip("/")
        if not _git_ok("cat-file", "-e", f"{upstream_ref}:{path}"):
            findings.error(
                f"The path prefix `{prefix}` has the category "
                f"`{CATEGORY_SOURCE}`, but it does not exist in "
                f"`{upstream_ref}`.  Upstreaming it would push content to the "
                "rtems.org repository which does not belong there.")
    for line in _lines(_git("ls-tree", "HEAD")):
        info, _, name = line.partition("\t")
        kind = info.split()[1]
        path = f"{name}/" if kind == "tree" else name
        if get_category(path) == CATEGORY_UNKNOWN:
            findings.error(
                f"The top-level path `{name}` has no category.  Add it to "
                "`_CATEGORIES` in `inspect_changes.py`.")


def _get_status(worktree: Path) -> list[str]:
    """ Returns the changed and the untracked files which are not ignored. """
    return _lines(
        _git("status", "--porcelain", "--untracked-files=all", cwd=worktree))


def _format_items(worktree: Path, items: list[str]) -> list[str] | None:
    """ Returns the items which are not formatted, or None if the
    clang-format tool is not available. """
    clang_format = shutil.which("clang-format")
    if clang_format is None:
        return None
    subprocess.run([
        "specverify", "--format-items", f"--clang-format-path={clang_format}",
        "--clang-format-style=default:file:_clang-format",
        "--do-not-indent-lists", *items
    ],
                   cwd=worktree,
                   check=False,
                   capture_output=True)
    unformatted = _lines(_git("diff", "--name-only", "--", *items,
                              cwd=worktree))
    _restore(worktree)
    return unformatted


def _check_spec_format(worktree: Path, items: list[str],
                       findings: _Findings, where: str) -> str:
    if not items:
        return _SKIP
    unformatted = _format_items(worktree, items)
    if unformatted is None:
        findings.error("The clang-format tool is not available.")
        return _ERROR
    if unformatted:
        findings.error(
            f"{where}, these specification items are not formatted:",
            "\n".join(unformatted))
        return _ERROR
    return _OK


class _ExportResult:
    """ Holds the result of an export of all items. """

    def __init__(self, ok: bool, stale: list[str], message: str):
        self.ok = ok
        self.stale = stale
        self.message = message

    @property
    def clean(self) -> bool:
        """ Is true, if the export succeeded and left the tree clean. """
        return self.ok and not self.stale

    def describe(self) -> str:
        """ Returns the error message or the list of changed files. """
        return self.message if not self.ok else "\n".join(self.stale)


def _export(worktree: Path) -> _ExportResult | None:
    """ Exports all items and returns the result, or None if the export tool
    is not available.  The documentation lives in another repository and is
    not exported. """
    try:
        result = subprocess.run(["specwareexport", *_EXPORT_OPTIONS],
                                cwd=worktree,
                                check=False,
                                capture_output=True,
                                encoding="utf-8")
    except FileNotFoundError:
        return None
    # A file which the export writes and which nobody commits belongs in
    # .gitignore.
    stale = _get_status(worktree)
    _restore(worktree)
    return _ExportResult(result.returncode == 0, stale,
                         (result.stderr or result.stdout).strip())


def _has_export_configuration(worktree: Path) -> bool:
    """ A tree without the export configuration, such as eb/main, generates
    no files. """
    return (worktree / "specitems.yml").exists()


class _PendingExport:
    """ Holds a commit whose tree the export does not reproduce.  The next
    commit has to provide the generated files of a specification commit.  It
    has to provide the items of a merge. """

    def __init__(self, url: str, result: _ExportResult, merge: bool = False):
        self.url = url
        self.result = result
        self.merge = merge


class _TreeResult:
    """ Holds the result of the checks of a whole tree. """

    def __init__(self, fmt: str, export: str, details: list[str]):
        self.fmt = fmt
        self.export = export
        self.details = details

    @property
    def clean(self) -> bool:
        """ Is true, if no check of the tree failed. """
        return not self.details


def _get_head_items(base_ref: str, head_ref: str) -> list[str]:
    """ Returns the items which the head has to format.  These are all items
    if the change set changes the formatter.  Otherwise, these are the items
    which the change set adds or modifies.  Each commit formats the items it
    touches, so an item which the change set leaves alone stays formatted. """
    changed = _lines(
        _git("diff", "--name-only", "--diff-filter=d", base_ref, head_ref))
    if any(path in _FORMATTER_INPUTS for path in changed):
        return ["spec"]
    return [
        path for path in changed
        if path.startswith("spec/") and path.endswith(".yml")
    ]


def _check_tree(worktree: Path, items: list[str],
                verify: bool) -> _TreeResult:
    """ Checks the tree.  The items are formatted, all items pass the
    verification, the export reproduces the tree, and the tree is clean. """
    details: list[str] = []
    fmt = _SKIP
    unformatted = _format_items(worktree, items) if items else []
    if unformatted is None:
        details.append("The clang-format tool is not available.")
        fmt = _ERROR
    elif items and not unformatted:
        fmt = _OK
    elif unformatted:
        details.append("These specification items are not formatted:\n" +
                       "\n".join(unformatted))
        fmt = _ERROR
    if verify:
        result = subprocess.run(["specverify", "spec"],
                                cwd=worktree,
                                check=False,
                                capture_output=True,
                                encoding="utf-8")
        if result.returncode != 0:
            details.append("The specification items fail the verification:\n"
                           f"{(result.stderr or result.stdout).strip()}")
    export = _SKIP
    if _has_export_configuration(worktree):
        exported = _export(worktree)
        if exported is None:
            details.append("The specwareexport tool is not available.")
            export = _ERROR
        elif exported.clean:
            export = _OK
        else:
            details.append("The export does not reproduce the tree:\n"
                           f"{exported.describe()}")
            export = _ERROR
    else:
        status = _get_status(worktree)
        _restore(worktree)
        if status:
            details.append("The tree is not clean:\n" + "\n".join(status))
    return _TreeResult(fmt, export, details)


def _check_base(worktree: Path, base_ref: str,
                findings: _Findings) -> _TreeResult:
    """ Checks that the change set starts from a clean state.  A dirty base is
    a warning.  The first commit of the change set has to make the tree
    clean. """
    _git("checkout", "--detach", base_ref, cwd=worktree)
    result = _check_tree(worktree, [], False)
    if not result.clean:
        findings.warning(
            f"The base `{base_ref}` of the change set is not clean.  The first "
            "commit has to make the tree clean:", "\n".join(result.details))
    return result


def _check_head(worktree: Path, base_ref: str, head_ref: str,
                findings: _Findings) -> _TreeResult:
    """ Checks the tree after the last commit of the change set. """
    _git("checkout", "--detach", head_ref, cwd=worktree)
    result = _check_tree(worktree, _get_head_items(base_ref, head_ref), True)
    if not result.clean:
        findings.error(
            f"After the last commit `{head_ref}` of the change set, the tree "
            "is not clean:", "\n".join(result.details))
    return result


def _check_export(worktree: Path, category: list[str],
                  pending: _PendingExport | None, findings: _Findings,
                  url: str) -> tuple[str, _PendingExport | None]:
    """ Checks that the export reproduces the tree of the commit.  A
    specification commit may leave the generated files to the next commit,
    which has to be a source commit.  A merge may leave the items to the next
    commit, which has to be a specification commit. """
    result = _export(worktree)
    if result is None:
        findings.error("The specwareexport tool is not available.")
        return _ERROR, None
    status = _OK
    if pending is not None:
        if pending.merge:
            expected = CATEGORY_SPEC
            missing = "the items"
        else:
            expected = CATEGORY_SOURCE
            missing = "the generated files"
        if category != [expected]:
            findings.error(
                f"In {pending.url}, the export does not reproduce the tree, "
                f"and the next commit {url} is no `{expected}` commit "
                f"which provides {missing}:", pending.result.describe())
            status = _ERROR
        elif not result.clean:
            findings.error(
                f"In {url}, the export does not reproduce the tree, so the "
                f"commit does not provide {missing} of the preceding commit "
                f"{pending.url}:", result.describe())
            return _ERROR, None
    if result.clean:
        return status, None
    if category == [CATEGORY_SPEC] and (pending is None or not pending.merge):
        return status, _PendingExport(url, result)
    findings.error(f"In {url}, the export does not reproduce the tree:",
                   result.describe())
    return _ERROR, None


def _check_merge(worktree: Path, pending: _PendingExport | None,
                 findings: _Findings,
                 url: str) -> tuple[str, _PendingExport | None]:
    """ Checks that the export reproduces the tree of the merge.  If it does
    not, the next commit has to provide the items. """
    if pending is not None:
        findings.error(
            f"In {pending.url}, the export does not reproduce the tree, and "
            f"the next commit {url} is a merge:", pending.result.describe())
    if not _has_export_configuration(worktree):
        return (_SKIP if pending is None else _ERROR), None
    result = _export(worktree)
    if result is None:
        findings.error("The specwareexport tool is not available.")
        return _ERROR, None
    status = _OK if pending is None else _ERROR
    if result.clean:
        return status, None
    return status, _PendingExport(url, result, merge=True)


def _check_deleted(worktree: Path, commit: str, findings: _Findings,
                   url: str) -> str:
    """ Checks that no build item still lists a file the commit deleted. """
    deleted = _lines(
        _git("diff-tree", "--no-commit-id", "--name-only", "--no-renames",
             "--diff-filter=D", "-r", commit))
    if not deleted:
        return _SKIP
    found = find_references(deleted, cwd=worktree)
    if not found:
        return _OK
    detail = [
        f"{item} still lists {path}" for path in sorted(found)
        for item in sorted(found[path])
    ]
    findings.error(
        f"In {url}, the commit deletes files which a build item still lists, "
        "so the build specification refers to files which do not exist:",
        "\n".join(detail))
    return _ERROR


def _check_extractable(repository: Path, upstream_ref: str,
                       commits: list[tuple[str, str]],
                       categories: dict[str, list[str]],
                       findings: _Findings) -> None:
    """ Checks that the upstreamable commits apply to the upstream branch. """
    source = [
        commit for commit, _ in commits
        if categories[commit] == [CATEGORY_SOURCE]
    ]
    if not source:
        return
    logging.info("check that %d upstreamable commits apply to %s", len(source),
                 upstream_ref)
    with tempfile.TemporaryDirectory() as tmp_dir:
        worktree = Path(tmp_dir) / "extract"
        _git("worktree", "add", "--detach", str(worktree), upstream_ref,
             cwd=repository)
        try:
            for commit in source:
                if not _git_ok("cherry-pick", "--no-commit", commit,
                               cwd=worktree):
                    conflicts = _lines(
                        _git("diff", "--name-only", "--diff-filter=U",
                             cwd=worktree))
                    subprocess.run(["git", "cherry-pick", "--quit"],
                                   cwd=worktree,
                                   check=False,
                                   capture_output=True)
                    # The rtems.org baseline is frozen, so the result is a
                    # note for a later upstream submission.
                    findings.warning(
                        f"The commit `{commit[:10]}` does not apply to "
                        f"`{upstream_ref}`.  A submission to the rtems.org "
                        "repository would conflict in:",
                        "\n".join(conflicts))
                    return
                # A runner of the CI has no committer identity.  The commit
                # exists only in the temporary worktree.
                _git("-c",
                     "user.name=inspect_changes",
                     "-c",
                     "user.email=inspect_changes@invalid",
                     "commit",
                     "--no-edit",
                     "--no-verify",
                     "--allow-empty",
                     "-C",
                     commit,
                     cwd=worktree)
        finally:
            subprocess.run(
                ["git", "worktree", "remove", "--force",
                 str(worktree)],
                cwd=repository,
                check=False,
                capture_output=True)


def _get_categories(commit: str) -> list[str]:
    return sorted({get_category(f) for f in _get_files(commit)})


def _check_subject(subject: str, found: list[str], findings: _Findings,
                   url: str) -> None:
    for category in _SUBJECT_CATEGORIES:
        prefix = f"{category}: "
        if found == [category] and not subject.startswith(prefix):
            findings.error(f"In {url}, the subject of a `{category}` commit "
                           f"does not start with `{prefix}`.")
        elif found != [category] and subject.startswith(prefix):
            findings.error(
                f"In {url}, the subject starts with `{prefix}`, but the "
                f"change set belongs to: {', '.join(found)}.")


def _check_commit(
        worktree: Path, commit: str, subject: str, found: list[str],
        pending: _PendingExport | None, findings: _Findings,
        url: str) -> tuple[str, str, str, _PendingExport | None]:
    """ Checks a commit which is no merge.  The commit is checked out. """
    # pylint: disable=too-many-arguments
    # pylint: disable=too-many-positional-arguments
    files = _get_files(commit)
    if len(found) != 1 or CATEGORY_UNKNOWN in found:
        findings.error(
            f"In {url}, the change set belongs to more than one category: "
            f"{', '.join(found)}.  Split it into one commit per category.")
    _check_subject(subject, found, findings, url)
    existing = _get_existing_files(commit, files)
    items = [
        f for f in existing if f.startswith("spec/") and f.endswith(".yml")
    ]
    fmt = _check_spec_format(worktree, items, findings, f"In {url}")
    export = _SKIP
    if _has_export_configuration(worktree) and (
            pending is not None or any(
                f.startswith("spec/") or f.endswith(_C_SUFFIXES)
                for f in files)):
        export, pending = _check_export(worktree, found, pending, findings,
                                        url)
    deleted = _check_deleted(worktree, commit, findings, url)
    return fmt, export, deleted, pending


def _get_repository_path() -> Path:
    repository = Path(".").absolute()
    while not (repository / ".git").exists():
        if repository.parent == repository:
            raise RuntimeError("no Git repository found")
        repository = repository.parent
    return repository


def main(argv: list[str]) -> int:
    """ Inspects the change set of a pull request. """

    def _add_arguments(parser):
        parser.add_argument("--upstream-ref",
                            help="the reference of the rtems.org repository "
                            "mirror (default: rtems.org/main)",
                            default="rtems.org/main")
        parser.add_argument("--output",
                            help="write the report to this file instead of "
                            "the standard output")
        parser.add_argument("url",
                            metavar="URL",
                            nargs=1,
                            help="the repository URL")
        parser.add_argument("base_ref",
                            metavar="BASE_REF",
                            nargs=1,
                            help="the base Git reference")
        parser.add_argument("head_ref",
                            metavar="HEAD_REF",
                            nargs=1,
                            help="the head Git reference")

    args = get_arguments(argv[1:],
                         description=sys.modules[__name__].__doc__,
                         add_arguments=(_add_arguments, ))
    repository = _get_repository_path()
    base_ref = args.base_ref[0]
    head_ref = args.head_ref[0]
    url = args.url[0]
    findings = _Findings()
    has_upstream = _git_ok("rev-parse", "--verify", f"{args.upstream_ref}^{{commit}}")
    if has_upstream:
        _check_invariant(args.upstream_ref, findings)
    else:
        findings.warning(
            f"The upstream reference `{args.upstream_ref}` does not exist.  "
            "The category invariant and the extractability check are skipped.")
    exclude_ref = args.upstream_ref if has_upstream else None
    commits = _get_commits(base_ref, head_ref, exclude_ref)
    commit_rows = _get_rows(base_ref, head_ref, exclude_ref)
    logging.info("inspect %d commits in %s..%s", len(commit_rows), base_ref,
                 head_ref)
    rows = [["Subject", "Category", "Format", "Export", "Sources", "Status"]]
    categories = {commit: _get_categories(commit) for commit, _ in commits}
    with tempfile.TemporaryDirectory() as tmp_dir:
        worktree = Path(tmp_dir) / "inspect"
        _git("worktree", "add", "--detach", str(worktree), head_ref,
             cwd=repository)
        pending: _PendingExport | None = None
        pending_row: list[str] = []
        try:
            base = _check_base(worktree, base_ref, findings)
            rows.append([
                f"Base `{base_ref}`", "", base.fmt, base.export, _SKIP,
                _OK if base.clean else _WARNING
            ])
            base_row = rows[-1]
            dirty_base = not base.clean
            for commit, is_merge, subject in commit_rows:
                commit_url = f"{url}/commit/{commit}"
                errors_before = findings.error_count
                _git("checkout", "--detach", commit, cwd=worktree)
                if is_merge:
                    export, pending = _check_merge(worktree, pending,
                                                   findings, commit_url)
                    fmt = _SKIP
                    deleted = _SKIP
                    found = [_MERGE]
                else:
                    found = categories[commit]
                    fmt, export, deleted, pending = _check_commit(
                        worktree, commit, subject, found, pending, findings,
                        commit_url)
                if dirty_base:
                    dirty_base = False
                    tree = _check_tree(worktree, [], False)
                    if not tree.clean:
                        findings.error(
                            f"The base `{base_ref}` of the change set is not "
                            f"clean, and the first commit {commit_url} does "
                            "not make the tree clean:",
                            "\n".join(tree.details))
                        base_row[-1] = _ERROR
                status = (_OK if findings.error_count == errors_before else
                          _ERROR)
                rows.append([
                    f"[{subject}]({commit_url})", ", ".join(found), fmt,
                    export, deleted, status
                ])
                if pending is not None and pending.url == commit_url:
                    pending_row = rows[-1]
            if pending is not None:
                pending_row[-1] = _ERROR
                missing = "items" if pending.merge else "generated files"
                findings.error(
                    f"In {pending.url}, the export does not reproduce the "
                    f"tree, and no commit follows which provides the "
                    f"{missing}:", pending.result.describe())
            errors_before = findings.error_count
            head = _check_head(worktree, base_ref, head_ref, findings)
            rows.append([
                f"Head `{head_ref}`", "", head.fmt, head.export, _SKIP,
                _OK if findings.error_count == errors_before else _ERROR
            ])
        finally:
            subprocess.run(
                ["git", "worktree", "remove", "--force",
                 str(worktree)],
                cwd=repository,
                check=False,
                capture_output=True)
    if has_upstream:
        _check_extractable(repository, args.upstream_ref, commits, categories,
                           findings)
    content = CommonMarkContent(context="CC-BY-SA-4.0")
    content.add_simple_table(rows)
    content.add(findings.content)
    report = str(content)
    if args.output:
        Path(args.output).write_text(report, encoding="utf-8")
    else:
        sys.stdout.write(report)
    return 1 if findings.error_count else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
