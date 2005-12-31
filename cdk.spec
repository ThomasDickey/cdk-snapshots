# $Id: cdk.spec,v 1.15 2005/12/30 02:06:06 tom Exp $
# Note that this is NOT a relocatable package
%define ver      5.0
%define subver   20051230
%define rel      1
%define prefix   /usr

Summary: A Curses Development Library (ncurses-based widgets)
Name: cdk
Version: %ver
Release: %rel
Copyright: BSD
Group: Applications/Libraries
Source: ftp://invisible-island.com/cdk/cdk.tar.gz
BuildRoot: /tmp/cdk-%{ver}-root
Packager: Squidster <squidster@msa.dec.com>
Requires: ncurses


%description
A Curses Development Library (ncurses-based widgets)

%changelog

%prep
%setup -n %{name}-%{ver}-%{subver}

%build
CFLAGS="${RPM_OPT_FLAGS}"
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%prefix
make DOCUMENT_DIR=/usr/share/doc/cdk

%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT DOCUMENT_DIR=$RPM_BUILD_ROOT/usr/share/doc/cdk install

%clean
rm -rf $RPM_BUILD_ROOT

%post

%postun

%files
%defattr(-, root, root)
%doc BUGS EXPANDING NOTES TODO COPYING INSTALL README
%{prefix}/include/cdk/*.h
%{prefix}/lib/lib*.a
%{prefix}/man/man3/*

