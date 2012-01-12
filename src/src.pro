include(common.pri)
TEMPLATE = subdirs
SUBDIRS = ctrl-cut cups-filter lpd-epilog gui
!deploy {
  SUBDIRS += svg2pdf pclint
}
