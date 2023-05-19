#!/usr/bin/env python

# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (C) 2019 embedded brains GmbH & Co. KG
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

import os
import subprocess
import unittest

try:
    os.symlink("../../wscript", "wscript")
except FileExistsError:
    pass
try:
    os.symlink("fake-rtems5-gcc", "bin/fake-rtems5-g++")
except FileExistsError:
    pass
exec(open("../../wscript", "r").read())


class TestWscript(unittest.TestCase):
    def test_is_enabled(self):
        self.assertTrue(_is_enabled([], None))
        self.assertTrue(_is_enabled([], []))
        self.assertFalse(_is_enabled([], ["A"]))
        self.assertTrue(_is_enabled(["A"], "A"))
        self.assertFalse(_is_enabled(["B"], "A"))
        self.assertTrue(_is_enabled(["A"], ["A"]))
        self.assertFalse(_is_enabled(["B"], ["A"]))
        self.assertTrue(_is_enabled(["A"], ["A", "B"]))
        self.assertTrue(_is_enabled(["B"], ["A", "B"]))
        self.assertFalse(_is_enabled(["C"], ["A", "B"]))
        self.assertFalse(_is_enabled(["A"], {"not": "A"}))
        self.assertTrue(_is_enabled(["B"], {"not": "A"}))
        self.assertFalse(_is_enabled(["A"], {"and": ["A", "B"]}))
        self.assertTrue(_is_enabled(["A", "B"], {"and": ["A", "B"]}))
        self.assertTrue(_is_enabled(["A", "B", "C"], {"and": ["A", "B"]}))
        self.assertTrue(
            _is_enabled(["A", "B"], {"and": ["A", "B", {"not": "C"}]})
        )
        self.assertFalse(
            _is_enabled(["A", "B", "C"], {"and": ["A", "B", {"not": "C"}]})
        )
        self.assertFalse(_is_enabled(["A"], {"and": "A", "x": "y"}))
        self.assertFalse(_is_enabled(["A"], {"x": "A"}))
        self.assertTrue(_is_enabled([], {"not": {"and": ["A", {"not": "A"}]}}))


class TestWaf(unittest.TestCase):
    @staticmethod
    def waf(args, returncode):
        cmd = f"../../waf {args}"
        print(cmd)
        cp = subprocess.run(
            cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )
        if returncode != None and cp.returncode != returncode:
            raise AssertionError(f"command failed: {cmd}")
        return cp

    def setUp(self):
        self.waf("distclean", None)
        self.tools = f"--rtems-tools={os.getcwd()}"
        self.specs = "--rtems-specs=spec"

    def test_specs_no_such_directory(self):
        cp = self.waf("configure --rtems-specs=nix", 1)
        self.assertIn(b"Cannot list build specification directory:", cp.stderr)

    def test_specs_not_a_directory(self):
        cp = self.waf("configure --rtems-specs=test.py", 1)
        self.assertIn(b"Cannot list build specification directory:", cp.stderr)

    def test_configure_no_bsp(self):
        cp = self.waf(
            f"configure {self.tools} {self.specs} --rtems-config=fake-none.ini",
            1,
        )
        self.assertIn(b"No such base BSP: 'fake/none'", cp.stderr)

    def test_configure_inherit_loop(self):
        cp = self.waf(
            f"configure {self.tools} {self.specs} --rtems-config=fake-inherit-loop.ini",
            1,
        )
        self.assertIn(
            b"Recursion in BSP options inheritance: fake/one -> fake/two -> fake/one",
            cp.stderr,
        )

    def test_configure_inherit_none(self):
        cp = self.waf(
            f"configure {self.tools} {self.specs} --rtems-config=fake-inherit-none.ini",
            1,
        )
        self.assertIn(
            b"BSP variant 'fake/none' cannot inherit options from not existing variant 'fake/none2'",
            cp.stderr,
        )

    def test_configure_inherit_good(self):
        self.waf(
            f"configure {self.tools} {self.specs} --rtems-config=fake-inherit-good.ini",
            0,
        )

    def test_configure_good(self):
        cp = self.waf(
            f"configure {self.tools} {self.specs} --rtems-config=fake-good.ini",
            0,
        )
        content = """$
a$z
DEF
0${NIX}1
"""
        with open("build/fake/good/config-file.txt", "r") as f:
            self.assertEqual(content, f.read())

    def test_bad_config_file(self):
        cp = self.waf(
            f"configure {self.tools} {self.specs}2 --rtems-config=fake-good.ini",
            1,
        )
        self.assertIn(
            b"""In item 'config-file' substitution in '${OPEN
' failed: Invalid placeholder in string: line 1, col 1
""",
            cp.stderr,
        )

    def test_ini_bad_integer(self):
        cp = self.waf(
            f"configure {self.tools} {self.specs} --rtems-config=bad-integer.ini",
            1,
        )
        self.assertIn(
            b"Value 'bad' for option 'INTEGER' is an invalid integer", cp.stderr
        )

    def test_ini_too_small(self):
        cp = self.waf(
            f"configure {self.tools} {self.specs} --rtems-config=too-small.ini",
            1,
        )
        self.assertIn(
            b"Value '-1' for option 'INTEGER' is not in closed interval [1, 16]",
            cp.stderr,
        )

    def test_ini_too_big(self):
        cp = self.waf(
            f"configure {self.tools} {self.specs} --rtems-config=too-big.ini",
            1,
        )
        self.assertIn(
            b"Value '100' for option 'INTEGER' is not in closed interval [1, 16]",
            cp.stderr,
        )

    def test_ini_not_power_of_two(self):
        cp = self.waf(
            f"configure {self.tools} {self.specs} --rtems-config=not-power-of-two.ini",
            1,
        )
        self.assertIn(
            b"Value '3' for option 'INTEGER' is not a power of two", cp.stderr
        )

    def test_good_integer(self):
        self.waf(
            f"configure {self.tools} {self.specs} --rtems-config=good-integer.ini",
            0,
        )


if __name__ == "__main__":
    unittest.main()
