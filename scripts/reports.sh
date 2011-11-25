DESTINATIONS=( "CONSOLE" "CSV" "HTML" )
DESTINATION_FILES=( "" "" "" )
ACTIONS=( "open" "begin" "print" "term" "close" )

# local functions
findAction() {
  action="$1"
  actcnt=${#ACTIONS[@]}
  actidx=-1

  for ((i=0; i<$actcnt; i++)); do
    if [ "$action" == "${ACTIONS[$i]}" ]; then
      actidx=$i
      break;
    fi
  done

  echo $actidx;
}

findDestination() {
  dest="$1"
  destcnt=${#DESTINATIONS[@]}
  destidx=-1

  for ((i=0; i<$destcnt; i++)); do
    if [ "$dest" == "${DESTINATIONS[$i]}" ]; then
      destidx=$i
      break;
    fi
  done

  echo $destidx;
}

findDestinationFile() {
 destidx=`findDestination "$1"`
 echo ${DESTINATION_FILES[$destidx]};
}

report_init() {
  dest="$1"
  destfile="$2"
  destidx=`findDestination "$dest"`
  [ $destidx -lt 0 ] && error "destination not found: $dest";
  DESTINATION_FILES[$destidx]=$destfile;
  [ "$destfile" != "-" -a ! -b "$destfile" -a ! -c "$destfile" ] && truncate --size 0 "$destfile"
}

report_open() {
  report_do "open" "$1"
}

report_begin() {
  report_do "begin" $@
}

report_print() {
  [ $COL_I -ge ${#COLUMNS[@]} ] && report_do "term";
  report_do "print" "$1" "$2"
  COL_I=$[ $COL_I + 1 ]
}

report_term() {
  report_do "term" $@
}

report_close() {
  report_do "close" $@
}

report_do() {
  action="$1"
  actidx="`findAction "$action"`"
  [ $actidx -lt 0 ] && error "Action not found: $action";
  shift;

  destcnt=${#DESTINATIONS[@]}

  for ((i=0; i<$destcnt; i++)); do
    if [ -n "${DESTINATION_FILES[$i]}" ]; then
        report_${action}_${DESTINATIONS[$i]} "$1" $2

    fi
  done
}

### START CONSOLE REPORTING

report_open_CONSOLE() {
 true
}

report_begin_CONSOLE() {
 true
}

report_print_CONSOLE()
{
  if [ $# -ge 2 ]; then
      $2 $1  >> "`findDestinationFile "CONSOLE"`"
  else
    echo -n $1  >> "`findDestinationFile "CONSOLE"`"
  fi

  s=$(printf "%$((${PADDING[${COL_I}]}-${#1}))s"); echo -n "${s// /.}"  >> "`findDestinationFile "CONSOLE"`"
}

report_term_CONSOLE() {
  COL_I=0;
  echo  >> "`findDestinationFile "CONSOLE"`"
}

report_close_CONSOLE() {
 true
}

### START CSV REPORTING

report_open_CSV() {
 true
}

report_begin_CSV() {
  [ -n "$1" ] && report_print_CSV "$1"
}

report_print_CSV() {
  echo -en "${1};" >> "`findDestinationFile "CSV"`"
}

report_term_CSV() {
  echo >> "`findDestinationFile "CSV"`";
}

report_close_CSV() {
 true
}

### START HTML REPORTING
HTML_TEST_DIR=
HTML_CASE=
HTML_TEST_TYPE=

report_open_HTML() {
  javascript="\
function timedRefresh(timeoutPeriod) { \
setTimeout(\"location.reload(true);\",timeoutPeriod); \
}"

  style="\
body { background-color:#fff;} \
a { font-size:10px; } \
table { width:100%; padding:0px; } \
td { margin:0px; padding:5px; border:2px solid #000; font-size:12px;} \
span { color:#000; }"

  intro="<html><head><script type=\"text/javascript\">$javascript</script><style type=\"text/css\">$style</style></head><body onload=\"timedRefresh(3000)\"><table padding=\"0\" margin=\"0\">"
  echo "$intro" >> "`findDestinationFile "HTML"`"
 
}

report_begin_HTML() {
  echo "<tr>"  >> "`findDestinationFile "HTML"`"
  if [ -n "$1" -a "$1" != "$HTML_TEST_DIR" ]; then
    echo "<td style=\"background-color: yellow\" nowrap width=0%><b>$1</b></td>" >> "`findDestinationFile "HTML"`"
    HTML_TEST_DIR="$1"
  else
    echo "<td width=0% style=\"border: 0;\"></td>" >> "`findDestinationFile "HTML"`"
  fi
}

function testimg() {
  origimgpath="$1"
  thumbimgpath="tmp/$1"

  if [ -f "$origimgpath" ]; then
    thumbdir="`dirname $thumbimgpath`"
    [ ! -d "$thumbdir" ] && mkdir -p  "$thumbdir" || rm -f "$thumbdir/*";
    convert -resize 300 -filter box -unsharp 0x5 -contrast-stretch 0 "$origimgpath" "$thumbimgpath" &> /dev/null
    echo "<div><a href="$origimgpath"><span style=\"font-size: 11px\">`basename $origimgpath`</span><br><img width=\"300\" src=\"$thumbimgpath\" alt=\"`dirname $origimgpath`\"></img></a></div>"
  fi
}


report_print_HTML() {
  [ $COL_I -eq 0 ] && HTML_CASE=$1;
  [ $COL_I -eq 1 ] && HTML_TEST_TYPE=$1;
  color=black
  [ -n "$2" ] && color="$2"

  echo "<td title="${COLUMNS[${COL_I}]}" style=\"color:$color;\">$1</td>" >> "`findDestinationFile "HTML"`"
}

report_term_HTML() {
  echo "</tr>" >> "`findDestinationFile "HTML"`";
  if [ -n "$HTML_TEST_DIR" -a -n "$HTML_CASE" -a -n "$HTML_TEST_TYPE" ]; then
    images=`find "$HTML_TEST_DIR/out/$HTML_CASE/" -name "$HTML_CASE*$HTML_TEST_TYPE*raw*.png" -or -name "$HTML_CASE*prn*.png"`
    echo "<tr><td style=\"text-align:center; font-size:16px; font-weight:bold; color:red; background-color:#eee; border:0; padding:0px\" colspan=\"$[ ${#COLUMNS[@]} + 1]\">" >> "`findDestinationFile "HTML"`";
    if [ -n "$images" ]; then
      echo "<table width=0% style=\"background-color:#eee; padding: 0px; margin: 0px;\"><tr>" >> "`findDestinationFile "HTML"`";
   
      if [ `echo "$images" | wc -l` -gt 0 ]; then
        echo "$images" | while read img; do
          echo "<td style=\"border:0; padding:5px\">" >> "`findDestinationFile "HTML"`";
  	  testimg "$img" >> "`findDestinationFile "HTML"`"
	  echo "</td>" >> "`findDestinationFile "HTML"`";
        done

        echo "<td width=100%>" >> "`findDestinationFile "HTML"`";
        echo "</tr></table></td></tr>" >> "`findDestinationFile "HTML"`";
      fi
    else
      echo "<br>NO IMAGES<br>&nbsp;</td></tr>" >> "`findDestinationFile "HTML"`";
    fi 
  fi
}

report_close_HTML() {
  echo "</table><div width=100% style=\" text-align: center; font-size: 18px; color: white; background-color: black; font-style:bolder;\">End of test</span></body></html>" >> "`findDestinationFile "HTML"`"
}
