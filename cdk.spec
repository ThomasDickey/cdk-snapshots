# Note that this is NOT a relocatable package
%define ver      4.9.10
%define rel      1
%define prefix   /usr

Summary: A Curses Development Library (ncurses-based widgets)
Name: cdk
Version: %ver
Release: %rel
Copyright: BSD
Group: Applications/Libraries
Source: ftp://dickey.his.com/cdk/cdk.tar.gz
BuildRoot: /tmp/cdk-%{ver}-root
Packager: Squidster <squidster@msa.dec.com>
Requires: ncurses


%description
A Curses Development Library (ncurses-based widgets)

%changelog

%prep
%setup

%build
CFLAGS="${RPM_OPT_FLAGS}"
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%prefix
make

%install
rm -rf $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{prefix} install

cd $RPM_BUILD_ROOT%{prefix}/
chown -R 0.0 *

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


