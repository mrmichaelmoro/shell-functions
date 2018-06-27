#!/bin/bash


##########################################################################
# This script has been authored using a standard temmplate for rapid     #
# development. All variables defined above the banner below are required #
# for the script/functions to operate properly. Functions are stored 	 #
# centrally for the sake of security and rapid development. 		 #
##########################################################################

##TEMPLATE VERSION --v1.1

# Number of cmdline arguements passed. Default zero. Set to properly
# check number of args if needed. No arguments - set to ZERO!
NO_OF_ARGS=1

# Usage Text - set for usage function when proper params are not passed
USAGE_TEXT="$0 [script_to_import_into_template]"

# Enable logging - by defaults enabled (1). To disable logging, set to zero(0).
ENABLE_LOGS=1

# Number of process logs to keep for this script. by default will keep the last
# 4 versions.
LOGVERSIONS=4

# Secret Sauce Section
# No user-servicable parts inside
source ~/tools/functions.inc
#

# Standard startup checks - do not remove
initialize
usage_chk
time_capture_start

##########################################################################
# Start Your Script Between The Banners Including Variable Definitions   #
##########################################################################
#merge_start

SCRIPT_SRC=$1
SCRIPT_SRC_BKUP=$SCRIPT_SRC".bkp"
TEMPLATE='~/tools/script_template.sh'

check_file $SCRIPT_SRC
check_file $TEMPLATE
cp $SCRIPT_SRC $SCRIPT_SRC_BKUP
check_file $SCRIPT_SRC_BKUP

MERGELINE_START=`grep -n "merge_start" $TEMPLATE | cut -f1 -d":"`
MERGELINE_END=`grep -n "merge_end" $TEMPLATE | cut -f1 -d":"`
END_O_TEMPLATE=`grep -n "end_of_template" $TEMPLATE | cut -f1 -d":"`

MERGELINE_START=`expr $MERGELINE_START - 1`
MERGELINE_END=`expr $MERGELINE_END + 1`

head -$MERGELINE_START $TEMPLATE > $SCRIPT_SRC
echo >> $SCRIPT_SRC
cat $SCRIPT_SRC_BKUP | grep -v "#!" >> $SCRIPT_SRC
echo >> $SCRIPT_SRC
cat $TEMPLATE | tail -n +"$MERGELINE_END" | head -n "$(($END_O_TEMPLATE - $MERGELINE_END))" >> $SCRIPT_SRC

info_handler "Script merged with template."

#merge_end
##########################################################################
# Your Script Should Not Go Beyond This Banner                           #
##########################################################################

# Stamdard closing checks - do not remove
time_check
