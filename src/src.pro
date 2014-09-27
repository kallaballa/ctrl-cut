include(common.pri)
TEMPLATE = subdirs
SUBDIRS = ctrl-cut cups-filter lpd-epilog gui cut2epilog
!deploy {
  SUBDIRS += svg2pdf pclint
}
