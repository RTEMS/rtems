#
# spec file for automake 
#

%define rpmvers 1.4_p5
%define srcvers	1.4-p5

%define _defaultbuildroot	/var/tmp/%{name}-%{srcvers}-root
%define _prefix			/opt/rtems
%define _name			automake

%if "%{_prefix}" != "/usr"
%define name			%{_name}-rtems
%define requirements		autoconf-rtems
%define _infodir		%{_prefix}/info
%define _mandir			%{_prefix}/man
%else
%define name			%{_name}
%define requirements		autoconf
%endif

Vendor:       http://www.rtems.com
Name:         %{name}
Packager:     Ralf Corsepius <ralf@links2linux.de>

Copyright:    GPL
Group:        rtems
Provides:     automake
Autoreqprov:  on
Version:      %{rpmvers}
Release:      0
Summary:      Tool for automatically generating GNU style Makefile.in's
BuildArch:    noarch
BuildRoot:    %{_defaultbuildroot}
BuildPreReq:  autoconf perl help2man
Requires:     %{requirements}
PreReq:	      /sbin/install-info

Source: automake-%{srcvers}.tar.bz2

%description
Automake is a tool for automatically generating "Makefile.in"s from
files called "Makefile.am". "Makefile.am" is basically a series of
"make" macro definitions (with rules being thrown in occasionally).
The generated "Makefile.in"s are compatible to the GNU Makefile
standards.

%prep
%setup -q -n %{_name}-%{srcvers}

%build
./configure --prefix=%{_prefix} --infodir=%{_infodir} --mandir=%{_mandir}
make

%install
%makeinstall

install -m 755 -d $RPM_BUILD_ROOT/%{_mandir}/man1
for i in $RPM_BUILD_ROOT%{_bindir}/*; do
  perllibdir=$RPM_BUILD_ROOT/%{_datadir}/automake \
  help2man $i > `basename $i`.1
  install -m 644 `basename $i`.1 $RPM_BUILD_ROOT/%{_mandir}/man1
done

gzip -9qf $RPM_BUILD_ROOT%{_infodir}/*.info* 2>/dev/null
gzip -9qf $RPM_BUILD_ROOT%{_mandir}/man?/* 2>/dev/null

%clean
[ x"$RPM_BUILD_ROOT" = x"%{_defaultbuildroot}" ] ; \
   rm -rf "$RPM_BUILD_ROOT"

%post 
install-info  --info-dir=%{_infodir} %{_infodir}/automake.info.gz

%preun
if [ $1 = 0 ]; then
  install-info --delete --info-dir=%{_infodir} %{_infodir}/automake.info.gz
fi

%files
%doc AUTHORS COPYING ChangeLog NEWS README THANKS
%{_bindir}/aclocal
%{_bindir}/automake
%doc %{_infodir}/automake.info*.gz
%doc %{_mandir}/man?/*
%{_datadir}/aclocal
%{_datadir}/automake
