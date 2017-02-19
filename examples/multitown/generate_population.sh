#!/bin/bash
# 
# This script creates a number of sample populations and samples
# 
####################################################

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

# Paths to executables is assuming the build is parallel to source directory but
# with source/ replaced with build/
build_dir="../.."

generate="$build_dir/generate"
arc_epi_sample="$build_dir/arc_epi_sample"
circle_gps_sample="$build_dir/circle_gps_sample"
random_sample="$build_dir/random_sample"
square_gps_sample="$build_dir/square_gps_sample"
strip_epi_sample="$build_dir/strip_epi_sample"

# determine whether to replace existing files
replace=0
while true; do
  echo -n "Do you wish to replace existing data (.json) files? (select ${BOLD}${YELLOW}y${NORMAL}es or ${BOLD}${YELLOW}n${NORMAL}o)> "
  read -s -n 1 answer
  echo
  if [ "$answer" = "y" ]; then
    replace=1
    break;
  elif [ "$answer" = "n" ]; then
    replace=0
    break;
  else
    echo "${RED}Invalid input, please select ${BOLD}${YELLOW}y${NORMAL}${RED} or ${BOLD}${YELLOW}n${NORMAL}"
  fi
done

# determine the seed
read -p "What seed would you like to use? (any number or string is permitted)> " seed

# delete all existing csv, json, log, done and png files if we are replacing them
if [ $replace -eq 1 ]; then
  rm -f *.csv *.json *.log *.txt
fi

# ignore file searches that come up empty
shopt -s nullglob

# loop over all income types
echo ""
echo "Generating populations"
for income_type in flat bbb; do
  for population_type in vanilla popdens; do

    mean_income_b00="0.75"
    mean_income_b01="0.00714"
    mean_income_b10="0.00714"
    if [[ "bbb" = "$income_type" ]]; then
      mean_income_b00="0.75,0.5"
      mean_income_b01="0.00714,0.1"
      mean_income_b10="0.00714,0.1"
    fi

    if [ -f "$population_type.$income_type.json" ]; then
      echo "  ${BOLD}$population_type.$income_type${NORMAL} population ${BLUE}[skipping]${NORMAL}"
    else
      command="$generate -f -s --seed $seed -c $population_type.conf $population_type.$income_type --mean_income_b00 $mean_income_b00 --mean_income_b01 $mean_income_b01 --mean_income_b10 $mean_income_b10"
      which sqsub > /dev/null
      if [ $? -eq 0 ]; then
        sqsub -r 10m --mpp=4g -q serial -o $population_type.$income_type.log $command
      else
        $command > $population_type.$income_type.log && \
          echo "  ${BOLD}$population_type.$income_type${NORMAL} population ${GREEN}[done]${NORMAL}"
      fi
    fi

  done
done
