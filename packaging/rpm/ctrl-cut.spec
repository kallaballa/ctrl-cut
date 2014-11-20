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
Version: 1.0
Release: 1
License: GPLv2+
Group: Hardware/Other
URL: https://github.com/Metalab/ctrl-cut
Packager: Amir Hassan <amir@viel-zu.org>
%if %{defined suse_version}
BuildRequires: update-desktop-files boost-devel ghostscript-library ghostscript-devel libqt4-devel libqt4-x11 libQtWebKit-devel libpng16-devel cairo-devel glib2-devel ImageMagick-devel libMagick++-devel libxml++-devel librsvg-devel libX11-devel cgal-devel
PreReq: /bin/chmod ncurses-utils
Requires: libboost_filesystem1_54_0 libboost_system1_54_0 ghostscript libqt4 libqt4-x11 libQtWebKit4 libpng16-16 libcairo2 libMagick++-6_Q16-5 libMagickCore-6_Q16-2 libxml++-2_6-2 librsvg-2-2 libX11-6 libCGAL10
%else
BuildRequires: gcc-c++ boost-devel ghostscript ghostscript-devel qt4-devel qt4-x11 qtwebkit-devel desktop-file-utils xdg-utils cairo-devel glib2-devel ImageMagick-devel ImageMagick-c++-devel libxml++-devel libpng-devel librsvg2-devel libX11-devel CGAL-devel
Requires: ghostscript boost-filesystem boost-system qt qt-x11 qtwebkit libpng cairo ImageMagick-c++ ImageMagick libxml++ librsvg2 libX11 CGAL
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
qmake-qt4 -recursive VERSION=1.0 CONFIG+=deploy CONFIG-=debug ctrl-cut.pro
%else
qmake -recursive VERSION=1.0 CONFIG+=deploy CONFIG-=debug ctrl-cut.pro
%endif
make

%install
export CC_BASE=`pwd`
export DESTDIR="%{buildroot}"
export PREFIX="%{_prefix}"
./cc install
%if %{defined suse_version}
  %suse_update_desktop_file -c %name Ctrl-Cut "Laser Cutter Control Panel" %name "" Graphics 2DGraphics Qt Engineering
%else
  cp etc/ctrl-cut.desktop $DESTDIR/$PREFIX/share/applications/
%endif

%post
 exit 0

%clean
# clean up the hard disc after build
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc README.md LICENSE

%{_libdir}/libctrl-cut.so.1
%{_bindir}/ctrl-cut
%{_prefix}/share/ctrl-cut
%{_prefix}/share/ctrl-cut/EpilogLegend36EXT.png
%{_prefix}/share/ctrl-cut/EpilogZing16.png
%{_prefix}/share/ctrl-cut/EpilogZing24.png
%{_prefix}/share/ctrl-cut/logo.svg
%{_prefix}/share/ctrl-cut/splash.png
%{_prefix}/share/applications/ctrl-cut.desktop

