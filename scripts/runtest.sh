#!/bin/bash

# Usage: runtest <testpath>/<testcase>
# Example: runtest test-data/corel/quad/default
runtest()
{
  casedir="$1"
  testdir="`dirname $casedir`"
  testname="`basename $testdir`"
  testcase="`basename $casedir`"
  testapp="`dirname $testdir`"

  ignorefile="$casedir/.ignores"
  optionsfile="$casedir/.options"
  commonoptsfile="$testapp/common.options"
  psfile="$testdir/$testname.ps"
  svgfile="$testdir/$testname.svg"
  prnfile="$testdir/$testcase.prn"
  cutsvgfile="$testdir/$testname.svg.cut"
  cutpsfile="$testdir/$testname.ps.cut"

  outdir="$testdir/$testcase"
  [ ! -d "$outdir" ]  && mkdir -p "$outdir"

  logfile="$outdir/$testcase.log"

  readIgnores "$ignorefile"

  if [ -f "$psfile" ]; then 
    logfile="$outdir/ps.raw.log"
    outfile="$outdir/ps.raw"
    prnv="$outdir/ps.prn-v.png"
    outv="$outdir/ps.raw-v.png"
    prnr="$outdir/ps.prn-r.png"
    outr="$outdir/ps.raw-r.png"

    report_begin "$testdir"
    report_print "$testcase" 2>> $logfile
    report_print "ps"  2>> $logfile

    # Generate a PCL/RTL file using our filter
    dotimeout  checkcat "run filter" "scripts/run-filter.sh $psfile $optionsfile $commonoptsfile"  > $outfile 2> $logfile
    errcode=$?
    if [ $errcode != 0 ]; then
	 report_print "N/A" red
         report_term
    else
         compareResults "$prnfile" "$outfile" "$prnv" "$outv" "$prnr" "$outr" "$logfile"
    fi
  fi

  if [ -f "$svgfile" ]; then
    logfile="$outdir/svg.raw.log"
    outfile="$outdir/svg.raw"
    prnv="$outdir/svg.prn-v.png"
    outv="$outdir/svg.raw-v.png"
    prnr="$outdir/svg.prn-r.png"
    outr="$outdir/svg.raw-r.png"

    report_begin "$testdir"
    report_print "$testcase" 2>> $logfile
    report_print "svg" 2>> $logfile
    # Generate a PCL/RTL file using our filter
    dotimeout  checkcat "run filter" "scripts/run-filter.sh $svgfile $optionsfile $commonoptsfile"  > $outfile 2>> $logfile
    errcode=$?

    if [ $errcode != 0 ]; then
      report_print "N/A" red
      report_term
    else
      compareResults "$prnfile" "$outfile" "$prnv" "$outv" "$prnr" "$outr" "$logfile"
    fi
  fi

  if [ -f "$cutpsfile" ]; then
    check "run cut2epilog on ps" "src/cut2epilog/cut2epilog $cutpsfile $outdir/cut.ps.raw" 2>> $logfile
    report_begin "$testdir"
    report_print "check ps equiv"
    if check "compare prn files" "cmp $outdir/cut.ps.raw $outdir/ps.raw" 2>> $logfile; then
      report_print "OK" green
    else
      report_print "Err" red
    fi
    report_term
  fi

  if [ -f "$cutsvgfile" ]; then
    check "run cut2epilog on svg" "src/cut2epilog/cut2epilog $cutsvgfile $outdir/cut.svg.raw" 2>> $logfile
    report_begin "$testdir"
    report_print "check svg equiv"
    if check "compare prn files" "cmp $outdir/cut.svg.raw $outdir/svg.raw" 2>> $logfile; then 
			report_print "OK" green
    else
      report_print "Err" red
		fi
    report_term 
  fi
}

function compareResults() {
  prnfile="$1"
  outfile="$2"
  prnv="$3"
  outv="$4"
  prnr="$5"
  outr="$6"
  logfile="$7"

  if [ -f $prnfile ]; then 
      has_prnfile=1
  else 
      has_prnfile=0
  fi

  # Binary compare with the validated output (e.g. from the Windows drivers)
  binary_ok=0
  if [ $has_prnfile == 1 ]; then
    dotimeout check "compare binary" "cmp $prnfile $outfile" 2>> $logfile
    if [ $? == 0 ]; then
      binary_ok=1
    fi
  fi

  if [ $binary_ok == 1 ]; then
    report_print "ok" green 2>> $logfile
  else
    if [ $has_prnfile == 0 ]; then
      report_print "N/A" red 2>> $logfile
    else
    report_print "no" yellow 2>> $logfile
    color=red
    # Convert cut vectors to bitmaps and compare them
    errorstr=""
    rtlcompare=`scripts/rtlcompare.sh -o $outdir $VERBOSE $prnfile $outfile "$prnv" "$outv" "$prnr" "$outr" 2>> $logfile`
    errcode="$?"
    if [ $errcode -ne 0 -o ! -f "$outv" ]; then
       errorstr="Err"
      rawtopbmfailed=1
    fi

    if [ $errcode -ne 0 ]; then
      errorstr="Err"
      color=red
    elif [ ! -f $prnv ]; then
      errorstr="N/A"
      color=green
    fi
    if [ -z $errorstr ]; then
      errorstr=`scripts/compare-bitmaps.sh $VERBOSE $prnv $outv 2>> $logfile`
      if [ $? == 0 ]; then
        pixelstr="OK"
        color=green
      else
        pixelstr=`echo $errorstr | awk '{ print $3 }'`
        color=red
      fi
    else 
      pixelstr=$errorstr
    fi
    report_print "$pixelstr" $color 2>> $logfile

    if [ ! -f $prnr ]; then
      pixelstr="N/A"
      color=green
    else
      errorstr=`scripts/compare-raster.sh $VERBOSE $prnr $outr 2>> $logfile`
      if [ $? == 0 ]; then
        pixelstr="OK"
        color=green
      else
        pixelstr="$errorstr"
        color=red
      fi
    fi
    report_print "$pixelstr" $color 2>> $logfile

    # Compare bboxes, number of polylines and total cut length
    color=red
    bbox_diff=`echo $rtlcompare | awk '{print $5}'`
    if [ "$bbox_diff" != "0" ]; then
        bboxstr="Err"
    else
        bboxstr="OK"
        color=green
    fi    
    report_print $bboxstr $color 2>> $logfile

    color=red
    polyline_diff=`echo $rtlcompare | awk '{ print $1 }'`
    if [ "$polyline_diff" != "0" ]; then
        plstr=$polyline_diff
    else
        plstr="OK"
        color=green
    fi
    report_print $plstr $color 2>> $logfile

    color=red
    length_diff=`echo $rtlcompare | awk '{print $2}'`
    if [ "$length_diff" != "0" ]; then
        lenstr="$length_diff"
    else
        lenstr="OK"
        color=green
    fi
    report_print $lenstr $color 2>> $logfile

    color=red
    move_diff=`echo $rtlcompare | awk '{print $3}'`
    if [ "$move_diff" != "0" ]; then
        lenstr="$move_diff"
    else
        lenstr="OK"
        color=green
    fi
    report_print $lenstr $color 2>> $logfile 
  fi

  fi

  report_term
}

