# SPDX-License-Identifier: BSD-2-Clause

# Copyright (C) 2026 embedded brains GmbH & Co. KG

pkg:
	. $(VENV)/bin/activate && specbuild $(GIT_OPTIONS) spec config-bsps/spec config-bsps/$(PKG_ARCH)/$(PKG_BSP)

pkg-update:
	. $(VENV)/bin/activate && specupdatetimeouts --spec-directory config-bsps/spec \
	  workspace/doc/sparc/gr765/smp/test-logs/*-simulator.json \
	  workspace/doc/sparc/gr765/uni/test-logs/*-simulator.json
	. $(VENV)/bin/activate && specupdateperf --spec-directory config-bsps/spec \
	  --spec-directory spec --lazy /target/sparc/gr765/sis/perf-smp \
	  workspace/doc/sparc/gr765/smp/test-logs/*-simulator.json
	. $(VENV)/bin/activate && specupdateperf --spec-directory config-bsps/spec \
	  --spec-directory spec --lazy /target/sparc/gr765/sis/perf-default \
	  workspace/doc/sparc/gr765/uni/test-logs/*-simulator.json

specview:
	. $(VENV)/bin/activate && specwareview --validated=no \
	  --enabled=sparc/gr765,sparc,bsps/sparc/leon3,target/simulator,RTEMS_QUAL
	. $(VENV)/bin/activate && specwareview --validated=no \
	  --enabled=sparc/gr765,sparc,bsps/sparc/leon3,target/simulator,RTEMS_QUAL,RTEMS_SMP
