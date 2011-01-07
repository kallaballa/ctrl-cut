# cd to epilogcups basedir
cd "`dirname $(readlink -f $0)`/.."

export DEVICE_URI=file://out.prn
./EpilogFilter 300 kintel Terminal 1 "" ../test/circle.ps 
