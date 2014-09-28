include(common.pri)
TEMPLATE = subdirs
SUBDIRS = ctrl-cut cups-filter lpd-epilog gui cut2epilog render2cut
!deploy {
  SUBDIRS += svg2pdf
}
