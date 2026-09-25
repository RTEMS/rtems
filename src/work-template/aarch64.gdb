# SPDX-License-Identifier: BSD-2-Clause

# Copyright (C) 2026 embedded brains GmbH & Co. KG

target remote :1234
set print pretty on
set pagination off
set confirm off
load
define reset
monitor system_reset
load
end
