DESTINATIONS=( "CONSOLE" "CSV" "SVG" )
DESTINATION_FILES=( "" "" "" )
ACTIONS=( "open" "begin" "print" "term" "close" )
REPORT_TITLE=
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
  if [ "$destfile" != "-" ]; then
    echo > $destfile || error "destination file not writeable: $destfile";
  fi
  DESTINATION_FILES[$destidx]=$destfile;
}

report_open() {
  REPORT_TITLE=$1
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
    [ -n "${DESTINATION_FILES[$i]}" ] &&  report_${action}_${DESTINATIONS[$i]} "$1" $2
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
      $2 $1
  else
    echo -n $1
  fi
  s=$(printf "%$((${PADDING[${COL_I}]}-${#1}))s"); echo -n "${s// /.}"
}

report_term_CONSOLE() {
  COL_I=0;
  echo
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

