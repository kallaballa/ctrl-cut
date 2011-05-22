#
# spec file for package ctrl-cut (Version 0.2)
#
# Copyright 2009 SUSE LINUX Products GmbH, Nuernberg, Germany.
# Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# needsrootforbuild

Summary: Laser Cutter Software (Epilog Legend 36EXT)
Name: ctrl-cut
Version: 0.2
Release: 1
License: GPLv2+
Group: Hardware/Other
URL: https://github.com/Metalab/ctrl-cut
Packager: Amir Hassan <amir@viel-zu.org>
%if %{defined suse_version}
BuildRequires: boost-devel cups-devel ghostscript-library ghostscript-devel libqt4-devel libqt4-x11
PreReq: /bin/chmod cups cups-client ncurses-utils
Requires: cups ghostscript-library
%else
BuildRequires: gcc-c++ boost-devel cups-devel ghostscript ghostscript-devel qt4-devel qt4-x11
Requires: cups ghostscript ncurses
%endif
Source:       %{name}-%{version}.tar.bz2
BuildRoot:    %{_tmppath}/%{name}-%{version}-build

%description
Ctrl-Cut is an initiative to produce Open Source toolchain for talking to laser cutters without being restricted to proprietery drivers and the chosen operating system of the laser cutter vendor.

Initially, this is targetted at Epilog laser cutters.

# extract source tar ball
%prep
%setup -q

%build
%if %{defined fedora_version}
qmake-qt4 -recursive VERSION=0.2 CONFIG+=deploy CONFIG-=debug ctrl-cut.pro
%else
qmake -recursive VERSION=0.2 CONFIG+=deploy CONFIG-=debug ctrl-cut.pro
%endif
make %{?jobs:-j%jobs}

%install
export CC_BASE=`pwd`
export CUPS_SERVER_BIN=$RPM_BUILD_ROOT/usr/lib/cups/
export CUPS_SERVER_DATA=$RPM_BUILD_ROOT/usr/share/cups/

mkdir -p $CUPS_SERVER_BIN/filter
mkdir -p $CUPS_SERVER_BIN/backend
mkdir -p $CUPS_SERVER_DATA/model
./cc install

%post
 test -x /etc/init.d/cups && \
         /etc/init.d/cups status >/dev/null && \
         /etc/init.d/cups reload >/dev/null
 # exit 0 needed here to ignore test return code
 exit 0

%clean
# clean up the hard disc after build
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc README.md LICENSE
%dir /usr/lib/cups/backend/
%dir /usr/lib/cups/filter/
%dir /usr/share/cups/model
%dir /usr/share/cups/model/Epilog

/usr/lib/cups/filter/ctrl-cut
/usr/lib/cups/backend/lpd-epilog
/usr/share/cups/model/Epilog/
/usr/share/cups/model/Epilog/Legend36EXT.ppd
