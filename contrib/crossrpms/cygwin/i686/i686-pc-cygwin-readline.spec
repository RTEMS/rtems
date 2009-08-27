#
# Please send bugfixes or comments to
# 	http://www.rtems.org/bugzilla
#


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

%define readline_version 5.2.14-12
%define readline_rpmvers %{expand:%(echo 5.2.14-12 | tr - _)} 

Name:         i686-pc-cygwin-readline
Release:      0.20090827.0%{?dist}
License:      GPL
Group:        Development/Tools

BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildArch:	noarch

Version:      	%readline_rpmvers
Summary:      	Cygwin readline Libraries

Source0:	ftp://cygwin.com/pub/cygwin/release/readline/readline-%{readline_version}.tar.bz2
Source1:	ftp://cygwin.com/pub/cygwin/release/readline/libreadline6/libreadline6-%{readline_version}.tar.bz2

Requires:	i686-pc-cygwin-libncurses-devel
Provides:       i686-pc-cygwin-readline-devel = %{version}-%{release}

%description
Cygwin readline libraries.

%prep
%setup -c -q

%build
  rm -rf i686-pc-cygwin

  # Setup sys-root (Usable for gcc >= 3.4)
  mkdir -p i686-pc-cygwin/sys-root
  ( cd i686-pc-cygwin/sys-root ; %{__tar} xvjf %{SOURCE0})
  ( cd i686-pc-cygwin/sys-root ; %{__tar} xvjf %{SOURCE1})

%install
  rm -rf $RPM_BUILD_ROOT
  mkdir -p $RPM_BUILD_ROOT%{_prefix}
  cp -a i686-pc-cygwin $RPM_BUILD_ROOT%{_prefix}

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
esac
done

sed -e 's,^[ ]*/usr/lib/rpm.*/brp-strip,./brp-strip,' \
< os_install_post~ > os_install_post 
%define __os_install_post . ./os_install_post


cat << EOF > %{_builddir}/%{name}-%{readline_rpmvers}/find-provides
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i686-pc-cygwin/(lib|include|sys-root)' \
  %{?_gcclibdir:| grep -v '^${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i686-pc-cygwin/'} | %__find_provides
EOF
chmod +x %{_builddir}/%{name}-%{readline_rpmvers}/find-provides
%define __find_provides %{_builddir}/%{name}-%{readline_rpmvers}/find-provides

cat << EOF > %{_builddir}/%{name}-%{readline_rpmvers}/find-requires
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i686-pc-cygwin/(lib|include|sys-root)' \
  %{?_gcclibdir:| grep -v '^${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i686-pc-cygwin/'} | %__find_requires
EOF
chmod +x %{_builddir}/%{name}-%{readline_rpmvers}/find-requires
%define __find_requires %{_builddir}/%{name}-%{readline_rpmvers}/find-requires

%ifnarch noarch
# Extract %%__debug_install_post into debug_install_post~
cat << \EOF > debug_install_post~
%__debug_install_post
EOF

# Generate customized debug_install_post script
cat debug_install_post~ | while read a x y; do
case $a in
# Prevent find-debuginfo.sh* from trying to handle foreign binaries
*/find-debuginfo.sh)
  b=$(basename $a)
  sed -e 's,find "$RPM_BUILD_ROOT" !,find "$RPM_BUILD_ROOT"%_bindir "$RPM_BUILD_ROOT"%_libexecdir !,' $a > $b
  chmod a+x $b
  ;;
esac
done

sed -e 's,^[ ]*/usr/lib/rpm/find-debuginfo.sh,./find-debuginfo.sh,' \
< debug_install_post~ > debug_install_post 
%define __debug_install_post . ./debug_install_post

%endif

%clean
  rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%dir %{_prefix}/i686-pc-cygwin
%{_prefix}/i686-pc-cygwin/sys-root


