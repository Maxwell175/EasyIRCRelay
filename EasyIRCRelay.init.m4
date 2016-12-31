#!/bin/sh
### BEGIN INIT INFO
# Provides:          EasyIRCRelay
# Required-Start:    $local_fs $network $named $time $syslog
# Required-Stop:     $local_fs $network $named $time $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Description:       EasyIRCRelay is a simple IRC bot that relays messages between channels on different IRC servers.
### END INIT INFO

SCRIPT="_BINPREFIX_/EasyIRCRelay _ETCPREFIX_/EasyIRCRelay/config.json"
RUNAS=_RUNASUSERNAME_

PIDFILE=/var/run/EasyIRCRelay.pid
LOGFILE=/var/log/EasyIRCRelay.log

start() {
  if [ -f "$PIDFILE" ] && kill -0 $(cat "$PIDFILE"); then
    echo 'Service already running' >&2
    return 1
  fi
  echo -n 'Starting service EasyIRCRelay... ' >&2
  local CMD="$SCRIPT &> \"$LOGFILE\" & echo \$!"
  su -c "$CMD" $RUNAS > "$PIDFILE"
  echo 'Done' >&2
}

stop() {
  if [ ! -f "$PIDFILE" ] || ! kill -0 $(cat "$PIDFILE"); then
    echo 'Service not running' >&2
    return 1
  fi
  echo -n 'Stopping service EasyIRCRelay... ' >&2
  kill -15 $(cat "$PIDFILE") && rm -f "$PIDFILE"
  echo 'Done' >&2
}

uninstall() {
  echo -n "Are you really sure you want to uninstall the EasyIRCRelay service? That cannot be undone. [yes|No] "
  local SURE
  read SURE
  if [ "$SURE" = "yes" ]; then
    stop
    rm -f "$PIDFILE"
    echo "Notice: The log file will not be removed: '$LOGFILE'" >&2
    update-rc.d -f EasyIRCRelay remove
    
    echo -n "Would you like me to also remove the program executable? [yes|No] "
    read SURE
    if [ "$SURE" = "yes" ]; then
      rm -fv "_BINPREFIX_/EasyIRCRelay"
    else
      echo "The program executable will continue to reside at \"_BINPREFIX_/EasyIRCRelay\"."
    fi
    
    echo -n "Should I delete the configuration directory? [yes/No] "
    read SURE
    if [ "$SURE" = "yes" ]; then
      rm -frv "_ETCPREFIX_/EasyIRCRelay"
    else
      echo "The configuration will continue to be located at _ETCPREFIX_/EasyIRCRelay"
    fi
    
    echo "The removal has been completed. I will now remove myself (this script) as well."
    rm -fv "$0"
  fi
}

reload() {
  if [ ! -f "$PIDFILE" ] || ! kill -0 $(cat "$PIDFILE"); then
    echo 'Service not running' >&2
    return 1
  fi
  echo -n 'Reloading service EasyIRCRelay... ' >&2
  kill -1 $(cat "$PIDFILE")
  echo 'Done' >&2
}


checkroot() {
  ROOT_UID="0"
  
  #Check if run as root
  if [ $(id -u) -ne 0 ] ; then
  	echo "You must run this command as root or using sudo."
  	exit 1
  fi
}

case "$1" in
  start)
    checkroot
    start
    ;;
  stop)
    checkroot
    stop
    ;;
  uninstall)
    checkroot
    uninstall
    ;;
  restart)
    checkroot
    stop
    start
    ;;
  reload)
    checkroot
    reload
    ;;
  *)
    echo "Usage: $0 {start|stop|reload|restart|uninstall}"
esac
