include(common.pri)
TEMPLATE = subdirs
SUBDIRS = cups-filter lpd-epilog
!deploy {
  SUBDIRS += svg2pdf pclint
}
