#!/bin/bash


PREPARE=; ENTER=; CLEAN=; VERBOSE=; X=; Y=;
RUN="/bin/bash"
CONFIG=
DISPLAY=

function verbose  { [ $VERBOSE ] && echo $@; $@ 1> /dev/null; }
function warn { [ $VERBOSE ] && echo "warn: $1" >&2; }
function error { echo "error: $1" >&2; exit 1; }
function usage {
  [ "$1" ] && error "$1"
  cat <<EOF
Usage: chrtsetup.sh [OPTIONS] [CHRTCONF]
  -v, --verbose            print actions
  -p, --prepare            perform mount sequence only
  -c, --clean              perform umount sequence only
  -e, --enter              enter chroot only
  -x DISPLAY               configures the xsession defined by DISPLAY
                           to allow all connections from localhost 
                           (xhost +localhost) and sets the DISPLAY
                           variable inside the chroot accordingly to
                           support connecting X11 clients from inside the
                           chroot to your X11 server.
  -y DISPLAY               does the same like -x but additionally sets the 
                           window manager name of the xsession to a 
                           non-reparenting window manager (wmname LG3D) to
                           avoid problems with some jdk versions running
                           with awesome wm. (shouldn't interfere with 
                           conventional use-cases :)
  -r, --run=COMMAND        run specified COMMAND after entering the chroot

CHRTCONF is the name of the chroot configuration stored in /etc/chrtsetup/.
If neither -p, -c nor -e is set -pce is assumed.

Examples:
  chrtsetup ubuntu                                      # executed setup defined in /etc/chrtsetup/ubuntu, enter chroot and clean up after
  chrtsetup -c ubuntu                                   # undo mounts performed to setup the chroot
  chrtsetup -pe ubuntu                                  # mount, enter but do not clean up after
  chrtsetup -y localhost:0.0 --run="xterm" ubuntu       # configure the xsession and start xterm inside the chroot
EOF

  exit 2
}

function chrtify { cmd=$1 doChroot=$2
  if [ "$doChroot" == "true" ]; then 
    echo "chroot -- $CHROOT $cmd"
  else
    echo $cmd
  fi
}

function doUmount { from=$1 to=$2 fstype=$3 opts=$4 doChroot=$5
  umount=`chrtify 'umount' $doChroot`
  verbose $umount -lt $fstype $to || error "umount failed: $@"
}

function doMount { from=$1 to=$2 fstype=$3 opts=$4 doChroot=$5
  mount=`chrtify 'mount' $doChroot`
  verbose $mount -t $fstype -o $opts $from $to || error "mount failed: $@"
}

function isMounted { from=$1 to=$2 fstype=$3 opts=$4 doChroot=$5
  grepMounts=`chrtify "grep -qF /etc/mtab -e" $doChroot`
  $grepMounts "$from $to $fstype"
} 

function clean {
  echo "$MOUNTS" | tac | while read mnt; do
    isMounted $mnt && doUmount $mnt \
      || warn "$mnt not mounted" >&2
  done
}

function prepare {
  echo "$MOUNTS" | while read mnt; do
    isMounted $mnt || doMount $mnt \
      && warn "$mnt already mounted" >&2
  done
}

function setupX {
    RUN="DISPLAY=$1 $RUN"
    export DISPLAY=$1 
    xhost +localhost
    [ $Y ] && /usr/local/bin/wmname LG3D
}

### main

[ $# -eq 0 ] && usage

eval set -- "`getopt -o vpecr:x:y: --long verbose,prepare,enter,clean,run:,x:,y: -n 'chrtsetup' -- \"$@\"`"
while true ; do
        case "$1" in
                -v|--verbose) VERBOSE=0 ; shift ;;
                -p|--prepare) PREPARE=0 ; shift ;;
                -e|--enter) ENTER=0 ; shift ;;
                -c|--clean) CLEAN=0 ; shift ;;
                -r|--run) RUN="$2" ; shift 2;;
                -x|--x) X=0; DISPLAY="$2" ; shift 2;;
                -y|--y) Y=0; DISPLAY="$2" ; shift 2;;
                --) shift ; break ;;
        esac
done
CONFIG=$1

[ -z $CONFIG ] && usage "CONFIG is mandatory"
[ ! "$CLEAN" -a ! "$PREPARE" -a  ! "$ENTER" ] && CLEAN=set PREPARE=set ENTER=set

source "$EC_ETC/chroot/$CONFIG"
[ "$X" -o "$Y" ] && setupX $DISPLAY
[ $PREPARE ] && prepare
[ $ENTER ]  && chroot "$CHROOT" bash -c "$RUN"
[ $CLEAN ]  && clean
