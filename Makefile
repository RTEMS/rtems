# SPDX-License-Identifier: BSD-2-Clause

# Copyright (C) 2026 embedded brains GmbH & Co. KG

RTEMS_VERSION ?= 7

# The architectures of the tools and the BSPs.  An empty TOOLS_ARCH selects
# all of them.
ALL_ARCHS = aarch64 arm i386 m68k microblaze mips moxie nios2 or1k powerpc \
	riscv sparc x86_64
TOOLS_ARCH ?=
ARCHS = $(or $(TOOLS_ARCH),$(ALL_ARCHS))
TOOLS_PREFIX ?= $(CURDIR)/tools/$(RTEMS_VERSION)
# Further options of build_tools.py.
TOOLS_OPTIONS ?=

export PKG_ARCH ?= $(or $(TOOLS_ARCH),sparc)
export PKG_BSP ?= gr740

# The change set of check-commits.  An empty BASE selects the merge base of
# HEAD_REF and its upstream branch.
BASE ?=
HEAD_REF ?= HEAD
REPOSITORY_URL ?= https://github.com/embedded-brains/rtems
INSPECT_OPTIONS ?=

WORK_TOOLS ?= $(CURDIR)/work-tools/$(RTEMS_VERSION)
WORK_ARCH ?= aarch64
WORK_BSP ?= zynqmp_apu
WORK_INI ?= work-config.ini
WORK_RTEMS ?= work-rtems
WORK_BUILD ?= work-build
WORK_TOOLS_MARKER ?= $(WORK_TOOLS)/bin/$(WORK_ARCH)-rtems$(RTEMS_VERSION)-gcc
WORK_MAKEFILE ?= Makefile.work

define WORK_CONFIG_INI =
[$(WORK_ARCH)/$(WORK_BSP)]
OPTIMIZATION_FLAGS = -O0 -g -fdata-sections -ffunction-sections
endef

TEST_REPORT ?= test-report.md

# A scaler overrides the scaler of the measured test durations in the test
# runner items.  Empty keeps the scaler of the items.
TIMEOUT_SCALER ?=
RUN_OPTIONS = $(if $(TIMEOUT_SCALER),--timeout-scaler $(TIMEOUT_SCALER))

export GIT_OPTIONS ?= --do-not-use-git

export VENV ?= .venv

comma = ,

VENV_MARKER = $(VENV)/venv-marker

.ONESHELL:

all: work-tools work-rtems $(WORK_MAKEFILE) work

# The package configuration of each BSP provides the package targets.
PKG_MAKEFILE = config-bsps/$(PKG_ARCH)/$(PKG_BSP)/package.mk

pkg: | prepare
	$(MAKE) -f $(PKG_MAKEFILE) pkg

# Update the target items from the test logs of the last package build.
pkg-update: | prepare
	$(MAKE) -f $(PKG_MAKEFILE) pkg-update
.PHONY: pkg-update

# View the specification with the enabled sets of the package.
specview: | prepare
	$(MAKE) -f $(PKG_MAKEFILE) specview
.PHONY: specview

pkg-clean:
	if test -d workspace/.git ; then cd workspace && git clean -xdf . && git checkout -- . ; fi

# The build directory is reused, so a build after a change compiles the
# changed sources only.  The aarch64 tools provide no ILP32 multilib, so
# the ILP32 BSPs cannot link.
bsps: | prepare
	. $(VENV)/bin/activate
	set -e
	./waf bsplist "--rtems-bsps=$(subst $(eval) ,$(comma),$(ARCHS:%=%/.*))" | \
	    grep -v '_ilp32' | \
	    python3 ./run_tests.py config --default OPTIMIZATION_FLAGS=-O2 \
	        --all-bsps $$(for arch in $(ARCHS); do python3 ./run_tests.py list $$arch; done | cut -f1) \
	        >config.ini
	./waf configure "--rtems-tools=$(TOOLS_PREFIX)"
	./waf
	./waf install
.PHONY: bsps

tests: | prepare
	. $(VENV)/bin/activate
	: >$(TEST_REPORT)
	status=0
	for bsp in $$(for arch in $(ARCHS); do python3 ./run_tests.py list $$arch; done | cut -f1); do
	  python3 ./run_tests.py run --reuse $(RUN_OPTIONS) "$$bsp" >>$(TEST_REPORT) || status=1
	done
	exit $$status
.PHONY: tests

# A special configuration builds one BSP with its options into a build
# directory of its own and runs its tests.
CONFIG ?= gr740-smp-debug

config-tests: | prepare
	. $(VENV)/bin/activate
	set -e
	bsp="$$(python3 ./run_tests.py configurations | awk -v name="$(CONFIG)" '$$1 == name { print $$2 }')"
	if test -z "$$bsp"; then echo "error: no special configuration $(CONFIG)" >&2; exit 2; fi
	python3 ./run_tests.py config --configuration "$(CONFIG)" >config-$(CONFIG).ini
	./waf configure "--rtems-tools=$(TOOLS_PREFIX)" "--rtems-config=config-$(CONFIG).ini" "--out=build-$(CONFIG)"
	./waf
	python3 ./run_tests.py run --reuse $(RUN_OPTIONS) --configuration "$(CONFIG)" \
	    --build-directory "build-$(CONFIG)" --output-directory "test-logs-$(CONFIG)" \
	    "$$bsp" >$(TEST_REPORT)
.PHONY: config-tests

tools: | prepare
	mkdir -p src
	. $(VENV)/bin/activate
	./build_tools.py --rtems-version=$(RTEMS_VERSION) $(TOOLS_OPTIONS) $(ARCHS)
.PHONY: tools

check-commits: | prepare
	. $(VENV)/bin/activate
	set -e
	base="$(BASE)"
	if test -z "$$base"; then base="$$(git merge-base "$(HEAD_REF)" "$(HEAD_REF)@{upstream}")"; fi
	python3 ./.github/inspect_changes.py $(INSPECT_OPTIONS) "$(REPOSITORY_URL)" "$$base" "$(HEAD_REF)"
.PHONY: check-commits

$(WORK_TOOLS_MARKER): | prepare
	mkdir -p src
	uv run ./build_tools.py --rtems-version=$(RTEMS_VERSION) --tools-directory=work-tools $(WORK_ARCH)
	$@ --version

work-tools: $(WORK_TOOLS_MARKER)

work-rtems: $(WORK_INI) $(WORK_TOOLS_MARKER) | prepare
	uv run ./waf configure "--rtems-tools=$(WORK_TOOLS)" "--prefix=$(WORK_RTEMS)" "--out=$(WORK_BUILD)" "--rtems-config=$(WORK_INI)"
	uv run ./waf
	uv run ./waf install

$(WORK_MAKEFILE): src/work-template/$(WORK_MAKEFILE)
	mkdir -p work
	cp -r src/work-template/* work
	mv work/$(WORK_MAKEFILE) $(WORK_MAKEFILE)

work: $(WORK_MAKEFILE)
	$(MAKE) -f $<
.PHONY: work

view: $(WORK_MAKEFILE)
	$(MAKE) -f $< view

run: $(WORK_MAKEFILE)
	$(MAKE) -f $< run

debug: $(WORK_MAKEFILE)
	$(MAKE) -f $< debug

gdb: $(WORK_MAKEFILE)
	$(MAKE) -f $< gdb

coverage: $(WORK_MAKEFILE)
	$(MAKE) -f $< coverage

clean:
	if test -f $(WORK_MAKEFILE); then $(MAKE) -f $(WORK_MAKEFILE) clean; fi

$(WORK_INI):
	echo "$(WORK_CONFIG_INI)" >$@

work-clean:
	rm -rf $(WORK_INI) $(WORK_RTEMS) $(WORK_BUILD)
.PHONY: work-clean

distclean: work-clean
	rm -rf config-cache config-tools tools $(WORK_TOOLS)
.PHONY: distclean

prepare: $(VENV_MARKER)

$(VENV_MARKER): uv.lock
	uv sync --all-groups
	touch $@

ifndef CI
uv.lock: pyproject.toml
	uv lock
	touch $@
endif
