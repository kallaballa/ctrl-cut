include(common.pri)
TEMPLATE = subdirs
SUBDIRS = cups-filter lpd-epilog gui
!deploy {
  SUBDIRS += svg2pdf pclint test
}
