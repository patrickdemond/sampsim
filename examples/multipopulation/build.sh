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
  rm `find -type f | grep "\.csv$"`
  rm `find -type f | grep "\.json$"`
  rm `find -type f | grep "\.log$"`
  rm `find -type f | grep "\.done$"`
  rm `find -type f | grep "\.png$"`
  rm `find -type f | grep "\.avi$"`
  rm `find -type f | grep "\.txt$"`
fi

# ignore file searches that come up empty
shopt -s nullglob

# loop over all weight types
echo ""
echo "Generating populations"
for weight_type in vanilla population income risk age sex pocket; do
  if [ 100 -ge `ls ${weight_type}.p*.json | wc -l` ]; then
    echo "  ${BOLD}$weight_type${NORMAL} population ${BLUE}[skipping]${NORMAL}" &
  else
    $generate -s --populations 100 --seed $seed -c vanilla.conf $weight_type \
      --dweight_population `if [[ "population" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      --dweight_income `if [[ "income" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      --dweight_risk `if [[ "risk" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      --dweight_age `if [[ "age" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      --dweight_sex `if [[ "sex" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      --dweight_pocket `if [[ "pocket" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
      > $weight_type.log && \
      echo "  ${BOLD}$weight_type${NORMAL} population ${GREEN}[done]${NORMAL}" &
  fi
done

for job in `jobs -p`; do
  wait $job
done

for weight_type in vanilla population income risk age sex pocket; do
  echo ""
  echo "Sampling $weight_type population"

  # loop over all samplers and process the population
  for sample_dir in *_sample; do
    # get the sample type by removing the _method at the end of the sample directory
    sample_type=${sample_dir:0: -7}

    files=($sample_dir/*.conf)
    for ((i=${#files[@]}-1; i>=0; i--)); do
      # loop over all populations
      for population_file in ${weight_type}.p*.json; do
        # get the population_name by removing the .json at the end of the population file
        population_name=${population_file:0: -5}

        # get the sample type by removing the .conf at the end of the sample config file
        sample_config_file="${files[$i]}"
        slash=`expr index "$sample_config_file" /`
        sample_size=${sample_config_file:$slash:-5}
        sample_name=${sample_config_file:0:$slash}${population_name}.${sample_size}

        if [ -f "$sample_name.done" ]; then
          echo "  ${BOLD}$sample_type${NORMAL} sample (sample size ${sample_size}) ${BLUE}[skipping]${NORMAL}"
        else
          ${!sample_dir} -f -d -s --seed $seed -c $sample_config_file $population_name.json $sample_name \
            > $sample_name.log && \
            touch $sample_name.done && \
            echo "  ${BOLD}$sample_type${NORMAL} sample (sample size ${sample_size}) ${GREEN}[done]${NORMAL}"
       fi
      done
    done
  done

  for job in `jobs -p`; do
    wait $job
  done
done
