# SPDX-License-Identifier: BSD-2-Clause

# Copyright (C) 2014, 2024 embedded brains GmbH & Co. KG
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

import json
import re
import matplotlib.pyplot as plt  # type: ignore
from matplotlib import ticker  # type: ignore


def _plot(data: dict) -> None:
    _, axes = plt.subplots()
    axes.set_title("Context Switch Timing Test")
    axes.set_xlabel("Function Nest Level")
    axes.set_ylabel("Context Switch Time [us]")
    x = list(range(0, len(data[0]["stats-by-function-nest-level"])))
    axes.xaxis.set_major_locator(ticker.FixedLocator(x))
    for samples in data:
        y = [
            values[2] / 1000.0
            for values in samples["stats-by-function-nest-level"]
        ]
        axes.plot(x, y, label=samples["environment"], marker='o')
    axes.legend(loc='best')
    plt.savefig("tmcontext01.png")
    plt.savefig("tmcontext01.pdf")
    plt.close()


_JSON_DATA = re.compile(
    r"\*\*\* BEGIN OF JSON DATA \*\*\*(.*)"
    r"\*\*\* END OF JSON DATA \*\*\*", re.DOTALL)

with open("tmcontext01.scn", "r", encoding="utf-8") as src:
    match = _JSON_DATA.search(src.read())
    data = json.loads(match.group(1))

_plot(data)
