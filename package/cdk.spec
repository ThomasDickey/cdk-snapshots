# $Id: cdk.spec,v 1.42 2016/12/04 11:57:51 tom Exp $
Summary:	Curses Development Kit
%define AppProgram cdk
%define AppVersion 5.0
%define AppRelease 20161204
Name:  %{AppProgram}
Version:  %{AppVersion}
Release:  %{AppRelease}
License:  BSD (4-clause)
Group:  Development/Libraries
URL:  http://invisible-island.net/%{name}/
Source0:  %{name}-%{version}-%{release}.tgz
BuildRoot:  %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
# BuildRequires:	ncurses-devel

%description
Cdk stands for "Curses Development Kit".  It contains a large number of ready
to use widgets which facilitate the speedy development of full screen curses
programs.

%package devel
Summary:	Curses Development Kit
Group:		Development/Libraries
Requires:	%{name} = %{version}-%{release}

%description devel
Development headers for cdk (Curses Development Kit)

%prep
%define debug_package %{nil}
%setup -q -n %{name}-%{version}-%{release}

%build
%configure
make all
find . -name '*.o' -exec rm -f {} \;

%configure --with-shared --with-versioned-syms
#make %{?_smp_mflags} cdkshlib
make all

%install
rm -rf $RPM_BUILD_ROOT
make install.libs install \
	DESTDIR=$RPM_BUILD_ROOT \
	DOCUMENT_DIR=$RPM_BUILD_ROOT%{_defaultdocdir}/%{name}
ls -l $RPM_BUILD_ROOT%{_libdir}
chmod +x $RPM_BUILD_ROOT%{_libdir}/*.so # fixes rpmlint unstripped-binary-or-object
install -m 644 libcdk.a $RPM_BUILD_ROOT%{_libdir}

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%doc CHANGES COPYING INSTALL NOTES README VERSION
%{_libdir}/*.so.*
%exclude %{_libdir}/*.a
%exclude %{_mandir}/man3/*
%{_defaultdocdir}/%{name}/*

%files devel
%defattr(-,root,root,-)
%doc EXPANDING TODO examples demos
%{_libdir}/*.a
%{_libdir}/*.so
%{_bindir}/cdk5-config
%{_mandir}/man3/*
%{_includedir}/%{name}.h
%{_includedir}/%{name}

%changelog

* Wed Nov 05 2014 Thomas E. Dickey
- move manpages to -devel package

* Sun Nov 02 2014 Thomas E. Dickey
- change shared library configuration to use --with-shared option

* Tue Mar 20 2012 Thomas E. Dickey
- install cdk.h in normal location

* Fri May 13 2011 Thomas E. Dickey
- parameterize/adapt to building from unpatched sources as part of test builds

* Sun Jan 4 2009 Marek Mahut <mmahut@fedoraproject.org> - 5.0.20081105-1
- Initial build
