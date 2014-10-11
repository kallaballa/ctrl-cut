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
BuildRequires: boost-devel cups-devel ghostscript-library ghostscript-devel libqt4-devel libqt4-x11 cairo-devel glib2-devel ImageMagick-devel libMagick++-devel libxml++-devel libpng16-devel librsvg-devel libX11-devel libQtWebKit-devel libpng16-compat-devel
PreReq: /bin/chmod cups cups-client ncurses-utils
Requires: libboost_filesystem1_53_0 libboost_system1_53_0 libopenssl1_0_0 glibc glibc libdrm2 libexpat1 libffi4 fontconfig libfreetype6 libgcc_s1 Mesa-libglapi0 libglib-2_0-0 Mesa-libGL1 libgmodule-2_0-0 libgobject-2_0-0 libgstapp-0_10-0 libgstreamer-0_10-0 libgstinterfaces-0_10-0 libgstinterfaces-0_10-0 libgstreamer-0_10-0 libgstinterfaces-0_10-0 libICE6 libjpeg8 liblzma5 glibc liborc-0_4-0 libpcre1 libpng16-16 glibc libqt4 libqt4-x11 libqt4 libqt4-x11 libQtWebKit4 glibc libSM6 libsqlite3-0 libopenssl1_0_0 libstdc++6 libuuid1 libwebp4 libX11-6 libX11-xcb1 libXau6 libxcb-dri2-0 libxcb-glx0 libxcb1 libXcursor1 libXdamage1 libXext6 libXfixes3 libXinerama1 libXi6 libxml2-2 libXrandr2 libXrender1 libxslt1 libXxf86vm1 libz1
%else
BuildRequires: gcc-c++ boost-devel cups-devel ghostscript ghostscript-devel qt4-devel qt4-x11 desktop-file-utils xdg-utils cairo-devel glib2-devel ImageMagick-devel libMagick++-devel libxml++-devel libpng16-devel librsvg-devel libX11-devel
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
qmake-qt4 -recursive VERSION=1 CONFIG+=deploy CONFIG-=debug ctrl-cut.pro
%else
qmake -recursive VERSION=1 CONFIG+=deploy CONFIG-=debug ctrl-cut.pro
%endif
make

%install
export CC_BASE=`pwd`
export DESTDIR="%{buildroot}"
export PREFIX="%{_prefix}"
./cc install

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
