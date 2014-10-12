include(common.pri)
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = ctrl-cut gui
!deploy {
  SUBDIRS += svg2pdf cups-filter cut2epilog render2cut lpd-epilog
}
