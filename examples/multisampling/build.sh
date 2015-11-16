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

# delete all existing csv, json and png files if we are replacing them
if [ $replace -eq 1 ]; then
  rm `find -type f | grep "\.\(csv\|json\|png\)$"`
fi

# ignore file searches that come up empty
shopt -s nullglob

# loop over all weight types
for weight_type in vanilla population income risk age sex pocket; do
  echo ""
  echo -n "Generating $weight_type population "
  if [ -f "$weight_type.json" ]; then
    echo "[skipping]"
  else
    $generate -s -p -c vanilla.conf $weight_type \
      --dweight_population `if [[ "population" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      --dweight_income `if [[ "income" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      --dweight_risk `if [[ "risk" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      --dweight_age `if [[ "age" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      --dweight_sex `if [[ "sex" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      --dweight_pocket `if [[ "pocket" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` > $weight_type.log
    echo "[done]"
  fi

  # loop over all samplers and process the population
  for sample_dir in *_sample
  do
    # get the sample type by removing the _method at the end of the sample directory
    sample_type=${sample_dir:0: -7}

    for sample_config_file in $sample_dir/*.conf
    do
      # get the sample type by removing the .conf at the end of the sample config file
      slash=`expr index "$sample_config_file" /`
      sample_size=${sample_config_file:$slash:-5}
      sample_name=${sample_config_file:0:$slash}${weight_type}.${sample_size}

      echo -n "> Generating $sample_type sample (sample size ${sample_size}) "
      if [ -f "$sample_name.json" ]; then
        echo "[skipping]"
      else
        ${!sample_dir} -s -c $sample_config_file $weight_type.json $sample_name > $sample_name.log
        echo "[done]"
      fi
    done
  done
done
