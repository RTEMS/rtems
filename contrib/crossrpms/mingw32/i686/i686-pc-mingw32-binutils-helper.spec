Name:           i686-pc-mingw32-binutils-helper
Version:        0.20090717.0
Release:        1%{?dist}
Summary:        RTEMS binutils helper

Group:          Development
License:        GPLv3+
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildArch:	noarch
BuildRequires:  mingw32-binutils
Requires:       mingw32-binutils
Provides:	i686-pc-mingw32-binutils

%description
%{summary}

%prep
%setup -q -c -T -n %{name}-%{version}

%build

%files

%changelog
