#!/bin/bash

cd $CC_BASE

. $CC_FUNCTIONS

xdg-icon-resource install --novendor --mode system --context apps --size 16 $CC_BASE/etc/desktop/ctrl-cut-16x16.png ctrl-cut
xdg-icon-resource install --novendor --mode system --context apps --size 22 $CC_BASE/etc/desktop/ctrl-cut-22x22.png ctrl-cut
xdg-icon-resource install --novendor --mode system --context apps --size 24 $CC_BASE/etc/desktop/ctrl-cut-24x24.png ctrl-cut
xdg-icon-resource install --novendor --mode system --context apps --size 32 $CC_BASE/etc/desktop/ctrl-cut-32x32.png ctrl-cut
xdg-icon-resource install --novendor --mode system --context apps --size 48 $CC_BASE/etc/desktop/ctrl-cut-48x48.png ctrl-cut
xdg-icon-resource install --novendor --mode system --context apps --size 128 $CC_BASE/etc/desktop/ctrl-cut-128x128.png ctrl-cut
desktop-file-install --dir=/usr/share/applications $CC_BASE/etc/desktop/ctrl-cut.desktop
xdg-desktop-icon install --novendor $CC_BASE/etc/desktop/ctrl-cut.desktop

