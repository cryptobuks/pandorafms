#!/bin/bash
# **********************************************************************
# Pandora FMS Generic Host Agent
# GNU/Linux version 
# (c) 2003-2006 Sancho Lerena <slerena@gmail.com>
# with the help of many people. Please see http://pandora.sourceforge.net
# Este codigo esta licenciado bajo la licencia GPL 2.0.
# This code is licensed under GPL 2.0 license.
# **********************************************************************
AGENT_VERSION=1.2.1
AGENT_BUILD=070103

if [ -z "$1" ]
then
	echo " "
	echo "Fatal error: I need an argument to Pandora FMS Agent config directory"
 	echo " "
 	echo " example: pandora_agent /etc/pandora  "
 	echo " "
 	exit -1
else
 	PANDORA_HOME=$1
fi

if [ ! -f $PANDORA_HOME/pandora_agent.conf ]
then
	echo " "
	echo "FATAL ERROR: Cannot load $PANDORA_HOME/pandora_agent.conf"
	echo " "
	exit -1
fi

# Init internal variables
CONTADOR=0
EXECUTE=1
MODULE_END=0
TIMESTAMP=`date +"%Y/%m/%d %H:%M:%S"`
IFS=$'\n'

# Default values
CHECKSUM_MODE=1
DEBUG_MODE=0
SERVER_PORT=22
INTERVAL=300
if [ -z "`echo $LANG | grep '\.'`" ]
then
	ENCODING="iso-8859-1"
else
	ENCODING=`echo $LANG | cut -f 2 -d "."`
fi
NOMBRE_HOST=`/bin/hostname`
OS_NAME=`uname -s`
PANDORA_LOGFILE=/var/log/pandora_agent.log
TEMP=/tmp

# Read config file
for a in `cat $PANDORA_HOME/pandora_agent.conf | grep -v -e "^#" | grep -v -e "^module" `
do
        a=`echo $a | tr -s " " " "`
        # Get general configuration parameters from config file
        if [ ! -z "`echo $a | grep -e '^logfile'`" ]
        then
                PANDORA_LOGFILE=`echo $a | awk '{ print $2 }' `
                echo "$TIMESTAMP - [SETUP] - Pandora Logfile is $PANDORA_LOGFILE" >> $PANDORA_LOGFILE
        fi
        if [ ! -z "`echo $a | grep -e '^server_ip'`" ]
        then
                SERVER_IP=`echo $a | awk '{ print $2 }' `
                echo "$TIMESTAMP - [SETUP] - Server IP Address is $SERVER_IP" >> $PANDORA_LOGFILE
        fi
        if [ ! -z "`echo $a | grep -e '^server_path'`" ]
        then
                SERVER_PATH=`echo $a | awk '{ print $2 }' `
                echo "$TIMESTAMP - [SETUP] - Server Path is $SERVER_PATH" >> $PANDORA_LOGFILE
        fi
        if [ ! -z "`echo $a | grep -e '^temporal'`" ]
        then
                TEMP=`echo $a | awk '{ print $2 }' `
                echo "$TIMESTAMP - [SETUP] - Temporal Path is $TEMP" >> $PANDORA_LOGFILE
        fi
        if [ ! -z "`echo $a | grep -e '^interval'`" ]
        then
                INTERVAL=`echo $a | awk '{ print $2 }' `
                echo "$TIMESTAMP - [SETUP] - Interval is $INTERVAL seconds" >> $PANDORA_LOGFILE
        fi
 	if [ ! -z "`echo $a | grep -e '^agent_name'`" ]
        then
               NOMBRE_HOST=`echo $a | awk '{ print $2 }' `
         	echo "$TIMESTAMP - [SETUP] - Agent name is $NOMBRE_HOST " >> $PANDORA_LOGFILE
        fi
 	if [ ! -z "`echo $a | grep -e '^debug'`" ]
        then
               DEBUG_MODE=`echo $a | awk '{ print $2 }' `
         	echo "$TIMESTAMP - [SETUP] - Debug mode is $DEBUG_MODE " >> $PANDORA_LOGFILE
        fi
 	if [ ! -z "`echo $a | grep -e '^checksum'`" ]
        then
               CHECKSUM_MODE=`echo $a | awk '{ print $2 }' `
         	echo "$TIMESTAMP - [SETUP] - Checksum is $CHECKSUM_MODE " >> $PANDORA_LOGFILE
        fi
	# Contribution of daggett
	if [ ! -z "`echo $a | grep -e '^server_port'`" ]
        then
                SERVER_PORT=`echo $a | awk '{ print $2 }' `
                echo "$TIMESTAMP - [SETUP] - Server Port is $SERVER_PORT" >> $PANDORA_LOGFILE
        fi
	# Contribution of daggett
	if [ ! -z "`echo $a | grep -e '^encoding'`" ]
        then
                ENCODING=`echo $a | awk '{ print $2 }' `
                echo "$TIMESTAMP - [SETUP] - Encoding is $ENCODING" >> $PANDORA_LOGFILE
        fi
done


# Get Linux Distro type and version
if [ -f "/etc/SuSE-release" ]
then
	OS_VERSION=`cat /etc/SuSE-release | grep VERSION | cut -f 3 -d " "`
	LINUX_DISTRO=SUSE
else
	if [ -f "/etc/lsb-release" ]
	then
		OS_VERSION=`cat /etc/lsb-release | grep DISTRIB_RELEASE | cut -f 2 -d "="`
		LINUX_DISTRO=UBUNTU
		OS_VERSION="UBUNTU $OS_VERSION"
	else
		if [ -f "/etc/debian_version" ]
		then
			OS_VERSION=`cat /etc/debian_version`
			OS_VERSION="DEBIAN $OS_VERSION"
			LINUX_DISTRO=DEBIAN
		else
			if [ -f "/etc/fedora-release" ]
			then
				OS_VERSION=`cat /etc/fedora-release | cut -f 4 -d " "`
				OS_VERSION="FEDORA $OS_VERSION"
				LINUX_DISTRO=FEDORA
			else
				LINUX_DISTRO=GENERIC
				OS_VERSION=`uname -r`
			fi
		fi
	fi
fi

# Script banner at start
echo "Pandora FMS Agent $AGENT_VERSION (c) Sancho Lerena, and others 2007"
echo "This program is licensed under GPL Terms. http://pandora.sf.net"
echo "Running in $NOMBRE_HOST at $TIMESTAMP \n"
echo " "

if [ "$DEBUG_MODE" == "1" ]
then
	echo "Warning: Running in DEBUG mode"
fi

# MAIN Program loop begin

while [ "1" == "1" ]
do
	# Deleted debug / error info on each run to avoid giant logs
	rm -Rf $PANDORA_LOGFILE.err 2> /dev/null

 	# Date and time, SERIAL is number of seconds since 1/1/1970, for every packet.
 	TIMESTAMP=`date +"%Y/%m/%d %H:%M:%S"`
 	SERIAL=`date +"%s"`
 
 	# File names
 	DATA=$TEMP/$NOMBRE_HOST.$SERIAL.data
 	DATA2=$TEMP/$NOMBRE_HOST.$SERIAL.data_temp
 	CHECKSUM=$TEMP/$NOMBRE_HOST.$SERIAL.checksum
 	PANDORA_FILES="$TEMP/$NOMBRE_HOST.$SERIAL.*"
 
 	# Makes data packet
	echo "<?xml version=\"1.0\" encoding=\"$ENCODING\"?> " > $DATA
	echo "<agent_data os_name='$OS_NAME' os_version='$OS_VERSION' interval='$INTERVAL' version='$AGENT_VERSION' timestamp='$TIMESTAMP' agent_name='$NOMBRE_HOST'>" >> $DATA
	
 	for a in `cat $PANDORA_HOME/pandora_agent.conf | grep -v -e "^#" | grep -e "^module" ` 
 	do
  		a=`echo $a | tr -s " " " "`
 
         	if [ ! -z "`echo $a | grep -e '^module_exec'`" ]
         	then
			if [ $EXECUTE -eq 0 ]
			then
	            		execution=`echo $a | cut -c 13- `
            			res=`eval $execution`
            			if [ -z "$flux_string" ]
            			then
	             			res=`eval expr $res 2> $PANDORA_LOGFILE.err`
     				fi
            			echo "<data><![CDATA[$res]]></data>" >> $DATA2
			fi
         	fi
 
         	if [ ! -z "`echo $a | grep -e '^module_name'`" ]
         	then
            		name=`echo $a | cut -c 13- `
     			echo "<name><![CDATA[$name]]></name>" >> $DATA2
         	fi
		
		if [ ! -z "`echo $a | grep -e '^module_begin'`" ]
		then
			echo "<module>" >> $DATA2
			EXECUTE=0
		fi
		
		if [ ! -z "`echo $a | grep -e '^module_max' `" ]
		then
			max=`echo $a | awk '{ print $2 }' `
			echo "<max><![CDATA[$max]]></max>" >> $DATA2
		fi

		if [ ! -z "`echo $a | grep -e '^module_min'`" ]
		then
			min=`echo $a | awk '{ print $2 }' `
			echo "<min><![CDATA[$min]]></min>" >> $DATA2
		fi
		
		if [ ! -z "`echo $a | grep -e '^module_description'`" ]
		then
			desc=`echo $a | cut -c 20- `
			echo "<description><![CDATA[$desc]]></description>" >> $DATA2
		fi
  
         	if [ ! -z "`echo $a | grep -e '^module_end'`" ]
         	then
         	   	echo "</module>" >> $DATA2
			MODULE_END=1
		else
			MODULE_END=0
         	fi
 
         	if [ ! -z "`echo $a | grep -e '^module_type'`" ]
         	then
            		mtype=`echo $a | awk '{ print $2 }' `
            		if [ ! -z "`echo $mtype | grep 'generic_data_string'`" ]
     			then
   				flux_string=1
     			else
                 		flux_string=0
                 		unset flux_string
            		fi
            		echo "<type><![CDATA[$mtype]]></type>" >> $DATA2
         	fi
		
  		if [ ! -z "`echo $a | grep '^module_interval'`" ]
  		then
              		# Determine if execution is to be done
              		MODULEINTERVAL=`echo $a | awk '{ print $2 }'`
              		EXECUTE=`expr \( $CONTADOR + 1 \) % $MODULEINTERVAL`
  		fi

		# If module ends, and execute for this module is enabled
		# then write 

		if [ $MODULE_END -eq 1 ]
		then
			if [ $EXECUTE -eq 0 ]
			then
				cat $DATA2 >> $DATA
			fi
			rm -Rf $DATA2 > /dev/null 2> /dev/null
		fi
	done
	
	# Count number of agent runs
	CONTADOR=`expr $CONTADOR + 1`
	# Keep a limit of 100 for overflow reasons
	if [ $CONTADOR -eq 100 ]
	then
		CONTADOR=0
	fi

	# Call for user-defined script for data adquisition
	if [ -f "$PANDORA_HOME/pandora_user.conf" ]
	then
		/bin/bash $PANDORA_HOME/pandora_user.conf >> $DATA
	fi
	
	# Checking MD5
	if [ "$CHECKSUM_MODE" == "1" ]
	then
		# Calculate Checksum and prepare MD5 file
		CHECKSUM_DATA=`md5sum $DATA`
		echo $CHECKSUM_DATA > $CHECKSUM 
	else
		CHECKSUM_DATA="No valid checksum"
		echo $CHECKSUM_DATA > $CHECKSUM
	fi

	# Finish data packet
	echo "</agent_data>" >> $DATA
	echo "" >> $DATA

	# Replace & chars in XML (should not to be any) to avoid syntax problems
	sed "s/&/&amp;/g" $DATA > $TEMP/finalxml.tmp
	rm -f $DATA
	mv $TEMP/finalxml.tmp $DATA

	if [ "$DEBUG_MODE" == "1" ]
	then
		echo "$TIMESTAMP - Finish writing XML $DATA" >> $PANDORA_LOGFILE
		echo "Data file is at $DATA - Exiting now"
		echo " "
		exit 0
	fi
	
	# Send packets to server and detele it
 	scp -P $SERVER_PORT $PANDORA_FILES pandora@$SERVER_IP:$SERVER_PATH > /dev/null 2>  $PANDORA_LOGFILE.err

	# Delete data
	rm -f $PANDORA_FILES> /dev/null 2> $PANDORA_LOGFILE.err
	
	# Go to bed :-)
	sleep $INTERVAL

done 
# This runs forever! 
