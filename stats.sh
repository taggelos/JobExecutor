#!/usr/bin/env bash

############
# Functions
############

#Coloured letters
red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`

#Display error message
function error(){
	echo "${red}Not an appropriate syntax!"
	echo "${green}Usage${reset}: $0 
[-t] for total number of keywords searched
[-m] for most frequently keyword found
[-l] for least frequently keyword found"
}

function read_files(){
	#Do the operations inside the log folder
	cd log

	#Check if there is log folder
	if (( $? != 0 )); then
		echo "${red}Log folder Not Found!${reset}"
		exit 1
	else
		count_searches=0
		most_found=0
		most_keyword=''
		least_found=0
		least_keyword=''
		#Store each word, so as not to hold duplicate searches
		declare -a words_found
		for filename in `ls -1 Worker_*.txt`
		do
			while read -r line || [[ -n "$line" ]]
			do
				#Have each part seperated by ':'
				IFS=':' read -ra parts <<< $line
				#Seperate command and keyword
				IFS=' ' read -ra cmd <<< "${parts[1]}"

				#Count search commands
				if [ $cmd == "search" ]; then
					#Do not count multiple searches of the same word
					if [[ ! " ${words_found[@]} " =~ " ${cmd[1]} " ]]; then
						words_found+=(${cmd[1]})
						count_searches=$((count_searches+1))
					fi					
				fi

				#Exclude the timestamp and the query from the parts
				count_paths=$((${#parts[@]}-2))

				#Calculate most found Keyword, if there are many same hold only the first one
				if (( $count_paths > most_found )); then
					most_found=$count_paths
					most_keyword=${cmd[1]}
				fi

				#Calculate least found Keyword, if there are many same hold only the first one
				if (( $least_found == 0 )); then
					least_found=$count_paths
				elif (( $count_paths < least_found )); then
					least_found=$count_paths
					least_keyword=${cmd[1]}
				fi
			done<$filename
		done
	fi
}

function print_results(){
	#The argument specified
	local arg=$1
	if [ "$arg" == "-t" ]; then
		echo "Total number of keywords searched: $count_searches"
	elif [ "$arg" == "-m" ]; then
		echo "$most_keyword [totalNumFilesFound: $most_found]"
	elif [ "$arg" == "-l" ]; then
		echo "$least_keyword [totalNumFilesFound: $least_found]"
	else
		echo "Total number of keywords searched: $count_searches"
		echo "Keyword most frequently found: $most_keyword [totalNumFilesFound: $most_found]"
		echo "Keyword least frequently found: $least_keyword [totalNumFilesFound: $least_found]"
	fi	
}
##################################
# Receive and check the arguments
##################################

#Number of arguments received
ARGC=$#

#If no arguments are provided, display all of the results
if (( $ARGC == 0 )); then
	read_files
	print_results
#Accept the argument, if it matches one of the specified characters
elif (( $ARGC == 1 )); then
	if [ "$1" == "-t" ] || [ "$1" == "-m" ] || [ "$1" == "-l" ]; then
		read_files 
		print_results $1
	else
		error
	fi
else
	error;
fi