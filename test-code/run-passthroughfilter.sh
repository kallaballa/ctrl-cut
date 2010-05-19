#!/bin/sh

if [ $# == 0 ]; then
  filter=./passthroughfilter
else
  filter=$1
fi

export CHARSET=utf-8
export CONTENT_TYPE=application/pdf
export CUPS_CACHEDIR=/private/var/spool/cups/cache
export CUPS_DATADIR=/usr/share/cups
export CUPS_FILETYPE=document
export CUPS_SERVERROOT=/private/etc/cups
export DEVICE_URI=file:/tmp/passthrough.ps
export FINAL_CONTENT_TYPE=printer/passthrough
export LANG=en_US.UTF-8
export PATH=/usr/libexec/cups/filter:/usr/bin:/usr/sbin:/bin:/usr/bin
export PPD=/private/etc/cups/ppd/passthrough.ppd
export PRINTER=passthrough
export SOFTWARE=CUPS/1.4.3
export USER=root

$filter 32 kintel Untitled 1 "AP_ColorMatchingMode=AP_ApplicationColorMatching AP_D_InputSlot= nocollate com.apple.print.DocumentTicket.PMSpoolFormat=application/pdf com.apple.print.JobInfo.PMApplicationName=TextEdit com.apple.print.JobInfo.PMJobName=Untitled com.apple.print.JobInfo.PMJobOwner=Marius\ Kintel com.apple.print.PageToPaperMappingMediaName=A4 com.apple.print.PageToPaperMappingType..n.=1 com.apple.print.PrinterInfo.PMColorDeviceID..n.=11625 com.apple.print.PrintSettings.PMColorMatchingMode..n.=0 com.apple.print.PrintSettings.PMColorSpaceModel..n.=1 com.apple.print.PrintSettings.PMColorSyncProfileID..n.=1580 com.apple.print.PrintSettings.PMCopies..n.=1 com.apple.print.PrintSettings.PMCopyCollate..b. com.apple.print.PrintSettings.PMDestinationType..n.=1 com.apple.print.PrintSettings.PMFirstPage..n.=1 com.apple.print.PrintSettings.PMLastPage..n.=2147483647 com.apple.print.PrintSettings.PMPageRange..a.0..n.=1 com.apple.print.PrintSettings.PMPageRange..a.1..n.=2147483647 DestinationPrinter"
