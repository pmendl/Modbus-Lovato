#!/bin/bash

### BEGIN INIT INFO
# Provides:        gpio_18_27_userspace
# Required-Start:  $local_fs $syslog
# Required-Stop:   $local_fs $syslog
# Default-Start:   2 3 4 5
# Default-Stop: 
# Short-Description: Allow GPIO ports 18 and 27 unprivileged writting
### END INIT INFO

# INSTALLING:
# Copy into /etc/init.d/, then install: $ insserv gpio_18_27_userspace.sh

PATH=/sbin:/bin:/usr/sbin:/usr/bin

. /lib/lsb/init-functions

function setPort () {
    if [ ! -d /sys/class/gpio/gpio$1 ]; then
	if [ ! -w /sys/class/gpio/export ]; then
	    log_failure_msg "Can not write GPIO ports export"
	    return 1
        fi

        echo $1 >/sys/class/gpio/export
        if [ ! -d /sys/class/gpio/gpio$1 ]; then
	    log_failure_msg "Export of GPIO port $1 failed"
	    return 1
	fi

    fi

    if [ ! -w /sys/class/gpio/gpio$1/direction ]; then
	log_failure_msg "Can not write GPIO port $1 direction"
	return 1
    fi

    echo out >/sys/class/gpio/gpio$1/direction
    if ! chmod a+w /sys/class/gpio/gpio$1/value; then
	log_failure_msg "Can not chmod GPIO port $1 value"
	return 1
    fi
};

RESULT=0
case $1 in
	start|restart|force-reload)
		log_daemon_msg "Setting GPIO ports as userspace output"
		log_progress_msg "18"
		if ! setPort 18; then
		    RESULT=1
		fi

		log_progress_msg "27"
		if ! setPort 27; then
		    RESULT=1
		fi

		log_end_msg $RESULT
  		;;
	stop)
  		;;
	status)
		exit 0
		;;
	*)
		echo "Usage: $0 {start|stop|restart|try-restart|force-reload|status}"
		exit 2
		;;
esac
