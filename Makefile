# SPDX-License-Identifier: BSD-2-Clause

# Copyright (C) 2026 embedded brains GmbH & Co. KG

TOOLS_ARCH ?= sparc

PKG_BSP ?= gr740

GIT_OPTIONS ?= --do-not-use-git

VENV ?= .venv

VENV_MARKER = $(VENV)/venv-marker

.ONESHELL:

all: tools pkg

pkg: | prepare
	. $(VENV)/bin/activate
	specbuild $(GIT_OPTIONS) spec config-bsps/spec config-bsps/$(TOOLS_ARCH)/$(PKG_BSP)

pkg-clean:
	if test -d workspace/.git ; then cd workspace && git clean -xdf . && git checkout -- . ; fi

.PHONY: bsps

bsps: | prepare
	. $(VENV)/bin/activate
	echo '[DEFAULT]' >config.ini
	echo 'OPTIMIZATION_FLAGS = -O2' >>config.ini
	./waf bsplist "--rtems-bsps=$(TOOLS_ARCH)/.*" | sed 's,\(.*\),[\1],' >>config.ini
	./waf configure "--rtems-tools=$(PWD)/tools/6"
	./waf
	./waf install

tools: | prepare
	mkdir -p src
	. $(VENV)/bin/activate
	./build_tools.py $(TOOLS_ARCH)

prepare: $(VENV_MARKER)

$(VENV_MARKER): uv.lock
	uv sync --all-groups
	touch $@

ifndef CI
uv.lock: pyproject.toml
	uv lock
	touch $@
endif
