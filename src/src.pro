include(common.pri)
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = ctrl-cut gui
!deploy {
  SUBDIRS += svg2pdf cut2epilog cut2brm render2cut import2cut lpd-epilog
}
