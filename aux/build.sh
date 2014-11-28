#!/bin/bash
# 
# A script used to run a large batch of configuration files
# 
# Program:  sampsim
# Module:   hypercube.sh
# Language: bash
# 
#########################################################################################

# colors
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
BLACK=$(tput setaf 0)
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
BLUE=$(tput setaf 4)
MAGENTA=$(tput setaf 5)
CYAN=$(tput setaf 6)
WHITE=$(tput setaf 7)
BOLD=$(tput bold)
UNDERLINE=$(tput smul)
STANDOUT=$(tput smso)
NORMAL=$(tput sgr0)

conf_files=( $( find . -type f | grep "\.conf$" ) )
num_conf_files=${#conf_files[*]}
int_greater_zero_pattern="^[1-9][0-9]*$" # any integer greater than 0

# functions
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-

# preamble
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
echo This script will build a total of $num_conf_files populations from a hypercube tree of configurations.
echo

# get whether to plot populations
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
plot=true
while true; do
  echo -n "Do you wish to create plots of all populations? (select ${BOLD}${YELLOW}y${NORMAL}es or ${BOLD}${YELLOW}n${NORMAL}o> "
  read -s -n 1 answer
  echo
  if [ "$answer" = "y" ]; then
    plot="-p"
    break;
  elif [ "$answer" = "n" ]; then
    plot=""
    break;
  else
    echo "${RED}Invalid input, please select ${BOLD}${YELLOW}y${NORMAL}${RED} or ${BOLD}${YELLOW}n${NORMAL}"
  fi
done

# get batch execution type
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
multiprocess=true
while true; do
  echo -n "Please select ${BOLD}${YELLOW}m${NORMAL}ulti-process or ${BOLD}${YELLOW}s${NORMAL}erial farming> "
  read -s -n 1 answer
  echo
  if [ "$answer" = "m" ]; then
    multiprocess=true
    break;
  elif [ "$answer" = "s" ]; then
    multiprocess=false
    break;
  else
    echo "${RED}Invalid input, please select ${BOLD}${YELLOW}m${NORMAL}${RED} or ${BOLD}${YELLOW}s${NORMAL}"
  fi
done

if [ true = "$multiprocess" ]; then
  # generate populations using multiple processes
  # -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  default_numjobs=$( grep -c ^processor /proc/cpuinfo )
  while true; do
    read -p "How many processes do you wish to use? (default: ${BLUE}$default_numjobs${NORMAL}) " numjobs
    if [ -z $numjobs ]; then numjobs=$default_numjobs; fi
    if [[ ! $numjobs =~ $int_greater_zero_pattern ]]; then
      echo "${RED}ERROR: you must choose an integer greater than 0${NORMAL}"
      exit
    else
      break;
    fi
  done

  job_commands=()
  for index in $( seq 1 $numjobs ); do
    ((index--))
    job_commands[$index]="date +'[process $index]> starting process at %X'"
  done

  job_index=0;
  for index in ${!conf_files[*]}; do
    job_index=$( expr $index % $numjobs )
    name=${conf_files[$index]:0:-5}
    cmd="nice -10 ./generate $plot -c $name.conf $name > $name.log && \
         echo \"[process $job_index]> configuration $name complete\""
    job_commands[$job_index]="${job_commands[$job_index]} && $cmd"
  done

  echo 
  for index in ${!job_commands[*]}; do
    eval ${job_commands[$index]} && date +"[process $index]> finished process at %X" &
  done
  wait
  echo "All jobs are complete."
else
  # generate populations using serial farming
  # -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  while true; do
    read -p "How many serial jobs do you wish to use? (default: ${BLUE}$num_conf_files${NORMAL}) " numjobs
    if [[ ! $numjobs =~ $int_greater_zero_pattern ]]; then
      echo "${RED}ERROR: you must choose an integer greater than 0${NORMAL}"
      exit
    else
      break;
    fi
  done
fi
