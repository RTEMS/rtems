# SPDX-License-Identifier: BSD-2-Clause

# Copyright (C) 2026 embedded brains GmbH & Co. KG

pkg:
	. $(VENV)/bin/activate && specbuild $(GIT_OPTIONS) spec config-bsps/spec config-bsps/$(PKG_ARCH)/$(PKG_BSP)

pkg-update:
	. $(VENV)/bin/activate && specupdatetimeouts --spec-directory config-bsps/spec \
	  workspace/doc/sparc/gr740/test-logs/*-simulator.json
	. $(VENV)/bin/activate && specupdateperf --spec-directory config-bsps/spec \
	  --spec-directory spec --lazy /target/sparc/gr740/sis/perf-smp \
	  workspace/doc/sparc/gr740/test-logs/*-simulator.json

specview:
	. $(VENV)/bin/activate && specwareview --validated=no \
	  --enabled=sparc/gr740,sparc,bsps/sparc/leon3,target/simulator,RTEMS_QUAL,RTEMS_SMP
