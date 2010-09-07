#
# Please send bugfixes or comments to
# 	http://www.rtems.org/bugzilla
#

%define _prefix                 /opt/rtems-4.10
%define _exec_prefix            %{_prefix}
%define _bindir                 %{_exec_prefix}/bin
%define _sbindir                %{_exec_prefix}/sbin
%define _libexecdir             %{_exec_prefix}/libexec
%define _datarootdir            %{_prefix}/share
%define _datadir                %{_datarootdir}
%define _sysconfdir             %{_prefix}/etc
%define _sharedstatedir         %{_prefix}/com
%define _localstatedir          %{_prefix}/var
%define _includedir             %{_prefix}/include
%define _libdir                 %{_exec_prefix}/%{_lib}
%define _mandir                 %{_datarootdir}/man
%define _infodir                %{_datarootdir}/info
%define _localedir              %{_datarootdir}/locale

%ifos cygwin cygwin32 mingw mingw32
%define _exeext .exe
%define debug_package           %{nil}
%define _libdir                 %{_exec_prefix}/lib
%else
%define _exeext %{nil}
%endif

%ifos cygwin cygwin32
%define optflags -O3 -pipe -march=i486 -funroll-loops
%endif

%ifos mingw mingw32
%if %{defined _mingw32_cflags}
%define optflags %{_mingw32_cflags}
%else
%define optflags -O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4 -mms-bitfields
%endif
%endif

%if "%{_build}" != "%{_host}"
%define _host_rpmprefix %{_host}-
%else
%define _host_rpmprefix %{nil}
%endif

%define rpmvers 1.11.1
%define srcvers	1.11.1
%define amvers  1.11

%define name			rtems-4.10-automake
%define requirements		rtems-4.10-autoconf >= 2.61

Name:		%{name}
URL:		http://sources.redhat.com/automake
License:	GPL
Group:		Development/Tools
Version:	%{rpmvers}
Release:	2%{?dist}
Summary:	Tool for automatically generating GNU style Makefile.in's

BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildArch:	noarch
BuildRequires:  %{requirements} perl
Requires:     	%{requirements}
Requires(post):	/sbin/install-info
Requires(preun):/sbin/install-info

Source0: ftp://ftp.gnu.org/gnu/automake/automake-%{srcvers}.tar.bz2


%description
Automake is a tool for automatically generating "Makefile.in"s from
files called "Makefile.am". "Makefile.am" is basically a series of
"make" macro definitions (with rules being thrown in occasionally).
The generated "Makefile.in"s are compatible to the GNU Makefile
standards.

%prep
%setup -q -n automake-%{srcvers}
%{?PATCH0:%patch0 -p1}

# Work around rpm inserting bogus perl-module deps
cat << \EOF > %{name}-prov
#!/bin/sh
%{__perl_provides} $* |\
    sed -e '/^perl(Automake/d'
EOF
%define __perl_provides %{_builddir}/automake-%{srcvers}/%{name}-prov
chmod +x %{__perl_provides}

cat << \EOF > %{name}-requ
#!/bin/sh
%{__perl_requires} $* |\
    sed -e '/^perl(Automake/d'
EOF
%define __perl_requires %{_builddir}/automake-%{srcvers}/%{name}-requ
chmod +x %{__perl_requires}


%build
PATH=%{_bindir}:$PATH
case %_host in
*-mingw32)
# MinGW ships obsolete perl-5.6.1, which doesn't support threads
  echo am_cv_prog_PERL_ithreads=no > config.cache
  ;;
esac

# Don't use %%configure, it replaces config.sub/config.guess with the 
# outdated versions bundled with rpm.
./configure -C --prefix=%{_prefix} --infodir=%{_infodir} --mandir=%{_mandir} \
  --bindir=%{_bindir} --datadir=%{_datadir} \
  --docdir=%{_datadir}/automake-%{amvers}/doc
make

%install
rm -rf "$RPM_BUILD_ROOT"
make DESTDIR=${RPM_BUILD_ROOT} install

mkdir -p $RPM_BUILD_ROOT%{_datadir}/aclocal
echo "/usr/share/aclocal" > $RPM_BUILD_ROOT%{_datadir}/aclocal/dirlist

rm -f $RPM_BUILD_ROOT%{_infodir}/dir
touch $RPM_BUILD_ROOT%{_infodir}/dir

# Extract %%__os_install_post into os_install_post~
cat << \EOF > os_install_post~
%__os_install_post
EOF

# Generate customized brp-*scripts
cat os_install_post~ | while read a x y; do
case $a in
# Prevent brp-strip* from trying to handle foreign binaries
*/brp-strip*)
  b=$(basename $a)
  sed -e 's,find $RPM_BUILD_ROOT,find $RPM_BUILD_ROOT%_bindir $RPM_BUILD_ROOT%_libexecdir,' $a > $b
  chmod a+x $b
  ;;
# Fix up brp-compress to handle %%_prefix != /usr
*/brp-compress*)
  b=$(basename $a)
  sed -e 's,\./usr/,.%{_prefix}/,g' < $a > $b
  chmod a+x $b
  ;;
esac
done

sed -e 's,^[ ]*/usr/lib/rpm.*/brp-strip,./brp-strip,' \
  -e 's,^[ ]*/usr/lib/rpm.*/brp-compress,./brp-compress,' \
< os_install_post~ > os_install_post 
%define __os_install_post . ./os_install_post

%clean
  rm -rf $RPM_BUILD_ROOT

%post 
/sbin/install-info  --info-dir=%{_infodir} %{_infodir}/automake.info.gz ||:

%preun
if [ $1 -eq 0 ]; then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/automake.info.gz ||:
fi

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog NEWS README THANKS
%dir %{_bindir}
%{_bindir}/aclocal*
%{_bindir}/automake*
%dir %{_infodir}
%ghost %{_infodir}/dir
%{_infodir}/automake.info*.gz
%dir %{_mandir}
%dir %{_mandir}/man1
%{_mandir}/man1/*
%dir %{_datadir}
%{_datadir}/aclocal
%{_datadir}/aclocal-%{amvers}
%{_datadir}/automake-%{amvers}

