include(common.pri)
TEMPLATE = subdirs
SUBDIRS = cups-filter lpd-epilog
!deploy {
  SUBDIRS += pclint
}
# Not used at the moment:
# SUBDIRS += svg2pdf 
