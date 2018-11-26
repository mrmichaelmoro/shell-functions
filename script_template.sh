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
TOOLSHOME="$HOME/tools/"
if [ -f $TOOLSHOME/function.inc ]; then
	source $TOOLSHOME/functions.inc
else
	echo
	echo "[ FAIL ]: Could not locate/source '$TOOLSHOME/functions.inc'"
	echo
	exit 2
fi
#

# Standard startup checks - do not remove
initialize
usage_chk
time_capture_start

##########################################################################
# Start Your Script Between The Banners Including Variable Definitions   #
##########################################################################
#merge_start


#merge_end
##########################################################################
# Your Script Should Not Go Beyond This Banner                           #
##########################################################################

# Stamdard closing checks - do not remove
time_check



#end_of_template - do not remove this line
