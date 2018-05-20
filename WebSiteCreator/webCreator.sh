#!/usr/bin/env bash

############
# Functions
############

#Coloured letters
red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`

#Display Usage message
function usage(){
	echo "${green}Usage${reset}:
$0 <ROOT DIRECTORY> <TEXT FILE> <NUMBER OF WEBSITES> <NUMBER OF LINKS>"
}

#Create directories and files
function create(){
	#The arguments specified
	local dirname=$1
	local filename=$2
	local numdirs=$3
	local numfiles=$4

	numlines=$(wc -l "$filename" | cut -d " " -f 1)
	if [[ "$numlines" -lt "10000" ]]; then
		echo "The number of lines must be greater than or equal to 10000. Please try with another file!"
		exit 2
	fi

	#Empty the directory if it is full
	if ! [ -z "$(ls -A $dirname)" ]; then
		echo "# Warning: directory is full, purging ..."
		rm -rf "$dirname"/*
	fi

	#Store the max value of the random number "k" that will be generated
	maxk=$((numlines - 2002))

	#Store the number of internal links (f) that will be generated
	numintlinks=$(( ( numfiles / 2 ) + 1 ))

	#Store the number of external links (q) that will be generated
	numextlinks=$(( ( numdirs / 2 ) + 1 ))

	declare -a arrsites
	for (( i = 0; i < numdirs; i++ )); do
		mkdir "$dirname/site$i"
		for (( j = 0; j < numfiles; j++ )); do
			website="site$i/page${i}_$RANDOM.html"			
			#Unique name for each individual website
			while [[ -e "$dirname/$website" ]]; do
				echo "File Exists. Generating another one with different number."
				website="site$i/page${i}_$RANDOM"
			done
			arrsites+=("$website")
			#Create the website
			touch "$dirname/$website"
		done
	done

	#Sites with incoming links
	declare -a checksites
	for (( i = 0; i < numdirs; i++ )); do
		#Create the site directory
		echo "# Creating web site $i ..."
		for (( j = 0; j < numfiles; j++ )); do
		x=$(( i * numfiles + j ))

		#Store the random number "k" that will be generated
		k=$(( ( RANDOM % maxk ) + 2 ))
		#Store the random number "m" that will be generated
		m=$(( ( RANDOM % 999 ) + 1001 ))
		#Last line we will take from the file
		f=$(( m / (numintlinks + numextlinks ) ))

		echo "#  Creating page ${arrsites[x]} with $m lines starting at line $k ..."

		#Put the header of html
		echo -e "<!DOCTYPE html>\n<html>\n\t<body>" > "$dirname${arrsites[x]}"

		#numintlinks
		for (( l = 0; l < numintlinks; l++ )); do
			sed -n "$k","$(( f + k ))"p "$filename" >> "$dirname${arrsites[x]}"
			y=$(( i * numfiles + $RANDOM % numfiles ))
			echo "#   Adding internal link to /${arrsites[y]}"
			echo -e "<a href=\"/${arrsites[y]}\">/${arrsites[y]}_text</a>" >> "$dirname${arrsites[x]}"
			checksites[y]=${arrsites[y]}
			k=$(( f + k + 1))
		done

		#numextlinks
		for (( l = 0; l < numextlinks; l++ )); do
			sed -n "$k","$(( f + k ))"p "$filename" >> "$dirname${arrsites[x]}"
			w=$(( $RANDOM % numdirs ))
			while [[ $w == $i ]]; do
				w=$(($RANDOM % numdirs))
			done
			y=$(( w * numfiles + $RANDOM % numfiles ))
			echo "#   Adding external link to /${arrsites[y]}"
			echo -e "<a href=\"/${arrsites[y]}\">/${arrsites[y]}_text</a>" >> "$dirname${arrsites[x]}"
			checksites[y]=${arrsites[y]}
			k=$(( f + k + 1))
		done

		echo -e "\n\t</body>\n</html>" >> "$dirname${arrsites[x]}"
		done
	done

	if [[ ${#checksites[@]} == ${#arrsites[@]} ]]; then
		echo "# All pages have at least one incoming link"
	fi

	echo "# Done."
}


##################################
# Receive and check the arguments
##################################

#Number of arguments received
ARGC=$#

#Regular expression for integer
intreg='^[0-9]+$'

#Error Flag to check if there was an error
errflag=0

#If there are not 4 arguments
if (( $ARGC != 4 )); then
	echo "${red}Four arguments are neeeded!${reset}"
	exit 1
fi

#Check 1st argument
if ! [[ -d "$1" ]]; then
	echo "${red}The first argument is not a directory!"
	errflag=1
fi

#Check 2nd argument
if ! [[ -f "$2" ]]; then
	echo "${red}The second argument is not a file!"
	errflag=1
fi

#Check 3rd argument
if ! [[ "$3" =~ $intreg  && "$3" > "1" ]] ; then
	echo "${red}The third argument is not an integer! (or bigger than 1)"
	errflag=1
fi

#Check 4th argument
if ! [[ "$4" =~ $intreg ]] ; then
	echo "${red}The fourth argument is not an integer!"
	errflag=1
fi

#Check if there was an error
if [ "$errflag" == "1" ]; then
	usage
	exit 1
else 
	echo "Lets Start!"
	create "$1" "$2" "$3" "$4"
fi
