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
  rm -f */*.csv */*.json */*.log */*.done */*.png */*.avi */*.txt
fi

# ignore file searches that come up empty
shopt -s nullglob

for income_type in flat bbb; do
  for population_type in vanilla popdens; do

    echo ""
    echo "Sampling $population_type.$income_type population"

    # loop over all samplers and process the population
    for sample_dir in *_sample; do
      # get the sample type by removing the _method at the end of the sample directory
      len=${#sample_dir}
      let len=len-7
      sample_type=${sample_dir:0:$len}

      files=($sample_dir/*.conf)
      for ((i=${#files[@]}-1; i>=0; i--)); do
        # get the sample type by removing the .conf at the end of the sample config file
        sample_config_file="${files[$i]}"
        slash=`expr index "$sample_config_file" /`
        dot=`expr index "$sample_config_file" .`
        let len=dot-slash-1
        sample_size=${sample_config_file:$slash:$len}
        sample_name=${sample_config_file:0:$slash}$population_type.$income_type.$sample_size

        if [ -f "$sample_name.done" ]; then
          echo "  ${BOLD}$sample_type${NORMAL} sample (sample size $sample_size) ${BLUE}[skipping]${NORMAL}"
        else
          which sqsub > /dev/null
          command="${!sample_dir} -F -d -s --seed $seed -c $sample_config_file $population_type.$income_type.json $sample_name"
          if [ $? -eq 0 ]; then
            sqsub -r 120m --mpp=4g -q serial -o $sample_name.log \
              $command && touch $sample_name.done
          else
            $command > $sample_name.log && touch $sample_name.done && \
              echo "  ${BOLD}$sample_type${NORMAL} sample (sample size $sample_size) ${GREEN}[done]${NORMAL}"
          fi
        fi
      done
    done

  done
done
