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

%define freebsd_version 8.1
%define freebsd_rpmvers %{expand:%(echo 8.1 | tr - _)}

%define _use_internal_dependency_generator 0
%define __debug_install_post %{nil}

Name:		i586-pc-freebsd8.1
Release:	0.20100727.0%{?dist}
License:	FreeBSD
Group:		Development/Tools

BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildArch:	noarch

Version:      	%freebsd_rpmvers
Summary:      	i586-pc-freebsd8.1 Libraries

Source0:  ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.aa
Source1:  ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ab
Source2:  ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ac
Source3:  ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ad
Source4:  ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ae
Source5:  ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.af
Source6:  ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ag
Source7:  ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ah
Source8:  ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ai
Source9:  ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.aj
Source10: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ak
Source11: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.al
Source12: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.am
Source13: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.an
Source14: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ao
Source15: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ap
Source16: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.aq
Source17: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ar
Source18: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.as
Source19: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.at
Source20: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.au
Source21: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.av
Source22: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.aw
Source23: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ax
Source24: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ay
Source25: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.az
Source26: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.ba
Source27: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.bb
Source28: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.bc
Source29: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.bd
Source30: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.be
Source31: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.bf
Source32: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.bg
Source33: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.bh
Source34: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.bi
Source35: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.bj
Source36: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.bk
Source37: ftp://ftp.freebsd.org/pub/FreeBSD/releases/i386/8.1-RELEASE/base/base.bl

%description
i586-pc-freebsd8.1 libraries.

%prep
%setup -q -c -T -n %{name}-%{version}

  cat \
 %SOURCE0 %SOURCE1 %SOURCE2 %SOURCE3 %SOURCE4 \
 %SOURCE5 %SOURCE6 %SOURCE7 %SOURCE8 %SOURCE9 \
 %SOURCE10 %SOURCE11 %SOURCE12 %SOURCE13 %SOURCE14 \
 %SOURCE15 %SOURCE16 %SOURCE17 %SOURCE18 %SOURCE19 \
 %SOURCE20 %SOURCE21 %SOURCE22 %SOURCE23 %SOURCE24 \
 %SOURCE25 %SOURCE26 %SOURCE27 %SOURCE28 %SOURCE29 \
 %SOURCE30 %SOURCE31 %SOURCE32 %SOURCE33 %SOURCE34 \
 %SOURCE35 %SOURCE36 %SOURCE37 \
 | %{__tar} --unlink -pxzf - ./lib ./usr/lib ./usr/include %{?el5:||:}

%build
  # Setup sys-root (Usable for gcc >= 3.4)
  mkdir -p i586-pc-freebsd8.1/sys-root
  %{__tar} cf - lib usr/lib usr/include | ( cd i586-pc-freebsd8.1/sys-root ; %{__tar} xf -)

  pushd i586-pc-freebsd8.1/sys-root/usr/lib > /dev/null
  # missing files
  rm -f lib*_p.*
  # not needed
  rm -rf aout compat

  # Fix up symlinks
  find -type l -exec ls -l {} \; | \
    while read a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aA ; do \
    case $aA in
    /lib* ) # link
       rm $a8;
       b=`basename $aA`
       ln -s ../../lib/$b $a8
       ;;
    esac
  done
  popd > /dev/null

%install
  rm -rf $RPM_BUILD_ROOT
  mkdir -p $RPM_BUILD_ROOT%{_prefix}
  cp -a i586-pc-freebsd8.1 $RPM_BUILD_ROOT%{_prefix}

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


cat << EOF > %{_builddir}/%{name}-%{freebsd_rpmvers}/find-provides
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i586-pc-freebsd8.1/(lib|include|sys-root)' \
  %{?_gcclibdir:| grep -v '^${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i586-pc-freebsd8.1/'} | %__find_provides
EOF
chmod +x %{_builddir}/%{name}-%{freebsd_rpmvers}/find-provides
%define __find_provides %{_builddir}/%{name}-%{freebsd_rpmvers}/find-provides

cat << EOF > %{_builddir}/%{name}-%{freebsd_rpmvers}/find-requires
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i586-pc-freebsd8.1/(lib|include|sys-root)' \
  %{?_gcclibdir:| grep -v '^${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i586-pc-freebsd8.1/'} | %__find_requires
EOF
chmod +x %{_builddir}/%{name}-%{freebsd_rpmvers}/find-requires
%define __find_requires %{_builddir}/%{name}-%{freebsd_rpmvers}/find-requires

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

%package sys-root
Group: Development/Tools
Summary: i586-pc-freebsd8.1 target files for gcc >= 3.4

%description sys-root
i586-pc-freebsd8.1 target files for gcc >= 3.4

%files sys-root
%defattr(-,root,root,-)
%dir %{_prefix}/i586-pc-freebsd8.1
%{_prefix}/i586-pc-freebsd8.1/sys-root
