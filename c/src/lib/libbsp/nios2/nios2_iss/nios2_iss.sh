#!/bin/sh
IOD=jtag_uart_0
PTF=nios2_iss.ptf
EXE="$1"
# e.g. hello.nxe
nios2-iss -c --stdin=${IOD} --stdout=${IOD} --stderr=${IOD} -f "${EXE}" -p "${PTF}"
