#! /usr/bin/env python
# encoding: utf-8

"""
def build(bld):
	bld.load('long_gcc')
"""

import os, tempfile
from waflib import Task

def exec_command(self, cmd, **kw):
	# workaround for command line length limit:
	# http://support.microsoft.com/kb/830473
	tmp = None
	try:
		if not isinstance(cmd, str) and len(str(cmd)) > 8192:
			(fd, tmp) = tempfile.mkstemp(dir=self.generator.bld.bldnode.abspath())
			flat = ['"%s"' % x.replace('\\', '\\\\').replace('"', '\\"') for x in cmd[1:]]
			try:
				os.write(fd, ' '.join(flat).encode())
			finally:
				if tmp:
					os.close(fd)
			# Line may be very long:
			# Logs.debug('runner:' + ' '.join(flat))
			cmd = [cmd[0], '@' + tmp]
		ret = super(self.__class__, self).exec_command(cmd, **kw)
	finally:
		if tmp:
			os.remove(tmp)
	return ret

def wrap_class(class_name):
	cls = Task.classes.get(class_name)
	if not cls:
		return None
	derived_class = type(class_name, (cls,), {})
	derived_class.exec_command = exec_command
	if hasattr(cls, 'hcode'):
		derived_class.hcode = cls.hcode
	return derived_class

for k in 'c cxx cprogram cxxprogram cshlib cxxshlib cstlib cxxstlib'.split():
	wrap_class(k)
