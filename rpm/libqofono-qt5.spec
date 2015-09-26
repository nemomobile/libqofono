Name:       libqofono-qt5

Summary:    A library of Qt 5 bindings for ofono
Version:    0.85
Release:    1
Group:      System/Libraries
License:    LGPLv2.1
URL:        https://github.com/nemomobile/libqofono
Source0:    %{name}-%{version}.tar.bz2
Requires:   qt5-qtcore
Requires:   mobile-broadband-provider-info
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(Qt5XmlPatterns)

%description
This package contains Qt bindings for ofono cellular service
interfaces.

%package declarative
Summary:    Declarative plugin for libqofono
Group:      Development/Tools
Requires:   %{name} = %{version}-%{release}
Requires:   %{name} = %{version}

%description declarative
This package contains declarative plugin for libofono.

%package devel
Summary:    Development files for ofono Qt bindings
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   %{name} = %{version}

%description devel
This package contains the development header files for the ofono Qt bindings.

%package tests
Summary:    qml test app for the ofono Qt bindings
Group:      Development/Tools
Requires:   %{name} = %{version}-%{release}
Requires:   blts-tools
Requires:   phonesim
Requires:   mce-tools

%description tests
This package contains qml test for ofono Qt bindings.

%prep
%setup -q -n %{name}-%{version}

%build
export QT_SELECT=5
%qmake5
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
export QT_SELECT=5
%qmake5_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/%{name}.so.*

%files declarative
%defattr(-,root,root,-)
%{_libdir}/qt5/qml/MeeGo/QOfono/*

%files devel
%defattr(-,root,root,-)
%{_libdir}/%{name}.prl
%{_libdir}/%{name}.so
%{_libdir}/pkgconfig/qofono-qt5.pc
%{_includedir}/qofono-qt5/*.h
%{_includedir}/qofono-qt5/dbus/ofono*.xml
%{_datadir}/qt5/mkspecs/features/qofono-qt5.prf

%files tests
%defattr(-,root,root,-)
%{_libdir}/%{name}/tests/*
/opt/tests/%{name}/*
