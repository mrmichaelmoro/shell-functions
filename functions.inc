# No user-servicable parts inside
#

# Formatting area - pretty output stuff
bold()          { ansi 1 "$@"; }
italic()        { ansi 3 "$@"; }
underline()     { ansi 4 "$@"; }
blink()		{ ansi 5 "$@"; }
highlight()	{ ansi 7 "$@"; }
strikethrough() { ansi 9 "$@"; }
red()           { ansi 91 "$@"; }
yellow()	{ ansi 93 "$@"; }
green()         { ansi 92 "$@"; }
cyan()		{ ansi 96 "$@"; }
ansi()          { echo -e "\e[${1}m${*:2}\e[0m"; }
lowercase()	{ echo "$@" | tr '[:upper:]' '[:lower:]'; }
uppercase()	{ echo "$@" | tr '[:lower:]' '[:upper:]'; }

### START Standard Functions
# check_parameters() - checks to see if the calling function has been provided a
# required argument.
check_parameters() {
	if [ "$1" == "" ]; then
		error_handler "Parameter not specified." abend
	fi
}

# usage() - displays the usage error message defined in USAGE_TEXT by usage_chk
usage() { echo; echo "$(bold [USAGE]:) $(red $USAGE_TEXT)"; echo; exit 3;}

# usage_chk() - check to see if the number of expected arguments are passed to the
# script and if not, calls usage to display error and exit.
usage_chk() { if [ $NO_OF_PASSED_ARGS -lt $NO_OF_ARGS ]; then usage; fi }

# time_capture_start() - sets the start time for tracking run/elapsed  time of the
# overall script. Needs to be called only once per script. By default called once 
# in start up section. 
time_capture_start() { STARTTIME=$(date +%s); }

# iteration_time_capture_start() - sets the start time for tracking run/elapsed time
# of the individual iteration. This is useful for tracking individual tasks in a 
# loop. Must be called/referenced at the start of each iteration one wishes to time 
# within the custom script section.
iteration_time_capture_start() { ITERATION_STARTTIME=$(date +%s); }

# time_capture_end() - sets the end time for tracking run/elapsed time Called by 
# time_check function to determine and display the elapsed time of an iteration or 
# overall script. Can be used independantly but no warranty is provied.
time_capture_end() { ENDTIME=$(date +%s); } 

# info_handler() - prints out any message to standard out with INFO formatting in ansi green.
info_handler() { INFO_MSG=$1; echo -ne "\n$(green [INFO]): $INFO_MSG\n"; }

# warn_handler() - prints out any message to standard out with WARN formating in ansi yellow.
warn_handler() { WARN_MSG=$1; echo -ne "\n$(yellow [WARN]): $WARN_MSG\n"; } 

# error_handler() - prints out any message to standard out with ERROR formating in ansi red.
# will take a second parameter of abend to terminate the script upon error.
# i.e. error_handler "This is a mess." (this will just error to stdout)
# i.e. error_handler "This is a mess." abend (this will error to stdout and exit)
error_handler() { 
	ERROR_MSG=$1
	if [ -n $2 ]; then ACTION=$2; fi
	echo -ne "\n$(red [ERROR]): $ERROR_MSG\n"
	if [ "$ACTION" == "abend" ]; then { exit 1; } fi
} # end error_handler

#debug_handler() - prints out any message to standard out with DEBUG formating in ansi orange if
# global variable DEBUG is set to 1
debug_handler() { 
	if [ $DEBUG -ne 0 ]; then
		DEBUG_MSG=$1
		echo -ne "\n$(cyan [DEBUG]): $DEBUG_MSG\n"
	fi
}

# get_file_line_count() - returns the number of lines in the file passed as a parameter.
# i.e. get_file_line_count strings.txt (will return the number of lines in strings.txt)
get_file_line_count() {
	FILE=$1
	check_parameters $FILE
	FILELINECOUNT=`wc -l $FILE | cut -f1 -d" "`
	debug_handler "File $(bold \'$FILE\') has $(bold $FILELINECOUNT) lines."
	return $FILELINECOUNT
} # end get_file_line_count

# check_file() - checks if the file passed as a parameter to the function exists. If the
# file does not exist, it calls error_handler for the error msg with the abend parameter.
# Also counts the number of lines in the file using get_file_line_count() and assigns value 
# to $FILELINECOUNT for use in loop counters.
check_file() {
	FILE=$1
	CHECK_FILE_OPTIONS=$2
	check_parameters $FILE
	if [ ! -f $FILE ]; then
		error_handler "File $(bold \'$FILE\') does not exist." $CHECK_FILE_OPTIONS
	fi
	get_file_line_count $FILE
} # end check_file

# bkup_file() - backs up the file passed as parameter with a .bak extension
bkup_file() {
	FILE=$1
	check_parameters $FILE
	BKUPFILE="$FILE.bak"

	if [ ! -f $FILE ]; then
		error_handler "File $(bold \'$FILE\') does not exist." abend
	else
		cp $FILE $BKUPFILE
		info_handler "File $(bold $FILE) has been backed up as $(bold ${BKUPFILE})."
	fi
} # end bkup_file
	

# time_check() - returns the end of the iteration or overall script when called.
# will return iteration if iteration_time_capture_start was initiated. Will return
# total script time if iteration_time_capture_start is not initiated. By default
# time_check() is called in the closing section of the script template to return
# total script elapsed time based on time_capture_start called in the start section.
time_check()
{
	if [ ! -z $ITERATION_STARTTIME ]; then
		ORIGINAL_STARTTIME=$STARTTIME
		STARTTIME=$ITERATION_STARTTIME
	fi

	time_capture_end
	ELAPSED_SECS=`expr $ENDTIME - $STARTTIME`
	if [ $ELAPSED_SECS -gt 59 ]; then
		HOURS=`expr $ELAPSED_SECS / 60 / 60`
		MINS=`expr $ELAPSED_SECS / 60`
		SECS=`expr $ELAPSED_SECS % 60`
		
		if [ $MINS -gt 59 ]; then
			MINSMOD=`expr $MINS / 60`
			MINSDELTA=`expr $MINSMOD \* 60`
			MINS=`expr $MINS - $MINSDELTA`
		fi
		
		HOURS=$(bold  $HOURS )
		MINS=$(bold  $MINS )
		SECS=$(bold  $SECS )
		
		TIME_STRING="$HOURS $(red hours), $MINS $(red minutes), $(red and) $SECS $(red seconds)"
	else
		TIME_STRING="$(bold $ELAPSED_SECS) $(red seconds)"
	fi

	if [ ! -z $ITERATION_STARTTIME ]; then
		unset ITERATION_STARTTIME
		STARTTIME=$ORIGINAL_STARTTIME
	fi

	echo
	echo "$(bold [ELAPSED TIME]:) $TIME_STRING"
	echo
} # end time_check

# action_req() - a function provided for continuation of any process or script. Expected 
# key press is provided via parameter for multiple use throughout script. Function will return
# KEYPRESS_MATCH set to 0 for match, 1 for not matched. User must handle action in their script.
action_req()
{
	KEYPRESS=$1
	check_parameters $KEYPRESS
	read -n1 -r -p "$(bold [ACTION REQ]:) Press $KEYPRESS to continue..." key

	if [ "$key" != "$KEYPRESS" ]; then
		echo
		info_handler "Exiting upon user request."
		exit 1
	fi
} #end action_req

# log4me() - provides logging and log management functions of any script so manual redirection
# is not required on each execution. By default, logging is enabled in the script template by
# setting ENABLE_LOGS=1
log4me() {
	#enable logging and clean up logs
	if [ $ENABLE_LOGS -eq 1 ] ;then
		exec &> >(tee -a "$PROCESS_LOG")
	
		info_handler "Logging is enabled."
		
		LOGEXISTS=`ls | grep -c "$LOG_FORMAT."`
		
		if [ $LOGEXISTS -eq 0 ]; then
			return
		fi

		PREVLOGFILES=`ls -m $LOG_FORMAT.*| sed -e's/,//g'`
		for LOG in $PREVLOGFILES
		do                        
			let NO_O_LOGS++
		done

		if [ $NO_O_LOGS -gt $LOGVERSIONS ]; then
			FILES_TO_DELETE=`expr $NO_O_LOGS - $LOGVERSIONS`
			LOG_COUNTER=1
			info_handler "Cleaning up old logs."

			for LOG in $PREVLOGFILES
			do
				if [ $LOG_COUNTER -le $FILES_TO_DELETE ]; then
					rm $LOG
					let LOG_COUNTER++
				else
					break
				fi
			done
		fi
	else
		warn_handler "Logging is disabled."
	fi #end file cleanup
} #end log4me

# tty_chk() - this function check if the script is running via an interactive
# terminal or schedule so one can control verbose output. This is automatically
# called from the initialization function - initialize() and sets variable
# MYTERMINAL - 0 is interactive terminal, 1 is not.
tty_chk() {
	tty -s
	TERMSTATUS=$?

	if [ $TERMSTATUS -ne 0 ]; then
		MYTERMINAL=1
	else
		MYTERMINAL=0
	fi
} # end tty_chk

# initialize() - this function does several functions including the following:
# * define non-servicable variables/paramters
# * process reusable parameters across scripts (deamonize, debug, etc) 
initialize() {

	case "$1" in
		    -d|--daemon) $0 < /dev/null &> $LOG & disown
				 exit 0 ;;
	esac

	# check parameters
	usage_chk
	tty_chk

	# enable/disable/handle logging
	log4me

	# initiate timer for script elapsed time
	time_capture_start
} #end initialize

# pageout() is a simple function that will send a string text to any mobile phone
# number for notifications. Useful to tack on to long running jobs for notifications
# to pager personnel.
pageout() {

	DEVICE=$1
	MESSAGE=$2

	SUBJECT_STR=$SUBJECT_STR" #DoNotReply"
	if [ ! -z $DEBUG ] && [ $DEBUG -eq 0 ]; then
		echo "$MESSAGE" | mail -s "$SUBJECT_STR" $DEVICE 
	fi

} #end pageout

# loop_counter() simplifies counting current progress of line reads against total 
# number of lines captured in $FILELINECOUNT variable. This was created for several
# scripts in which data was read in via files and process/maniuplated somehow.
loop_counter() {
	let LOOPYCOUNTER++
	echo "$(red [)$(bold $LOOPYCOUNTER/$FILELINECOUNT)$(red ])"
}

# initialize_file() - creates a null file for output. If file exists, prompts 
# for action unless force is passed as an option.
# initialize_file filename.out force
initialize_file() {
	FILE=$1
	check_parameters $FILE
	IFOPT=$2
	
	if [ -f $FILE ] && [ "$IFOPT" != "force" ]; then
		warn_handler "File '$(bold $FILE)' exists. Do you wish to overwrite?"
		action_req "Y"	
		>$FILE
	else
		>$FILE
	fi
}
	
## Variable section
NO_OF_PASSED_ARGS=$#
RUNNING_SCRIPT=`basename $0`
LOG_FORMAT="$RUNNING_SCRIPT.log"
PROCESS_LOG="$LOG_FORMAT.$(date +%m%d%Y_%H%M%S)"
SUBJECT_STR="Job Status"
DEBUG=0
