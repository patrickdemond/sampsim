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
sbatch=$( command -v sbatch )
sqsub=$( command -v sqsub )

build_dir="../../../.."
arc_epi_sample="$build_dir/arc_epi_sample"
circle_gps_sample="$build_dir/circle_gps_sample"
random_sample="$build_dir/random_sample"
square_gps_sample="$build_dir/square_gps_sample"
strip_epi_sample="$build_dir/strip_epi_sample"

# determine whether to replace existing files
replace=0
while true; do
  echo -n "Do you wish to replace existing data files? (select ${BOLD}${YELLOW}y${NORMAL}es or ${BOLD}${YELLOW}n${NORMAL}o)> "
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

# delete all existing sample files if we are replacing them
if [ $replace -eq 1 ]; then
  rm -f ../links/*/*_sample/*
fi

# ignore file searches that come up empty
shopt -s nullglob

for population in ../links/*/*.conf; do

  index=`echo $population | sed -e 's#\.\./links/##' | sed -e 's#/.*##'`
  echo ""
  echo "Sampling population $index"

  # loop over all samplers and process the population
  for sample_dir in *_sample; do
    # get the sample type by removing the _method at the end of the sample directory
    len=${#sample_dir}
    let len=len-7
    sampler=${sample_dir:0:$len}

    # make sure this sampler's directory exists
    sample_pop_dir=${population/v[a-z0-9.]*\.conf/$sample_dir}
    if [ ! -d $sample_pop_dir ]; then mkdir $sample_pop_dir; fi

    files=($sample_dir/*.conf)
    for ((i=${#files[@]}-1; i>=0; i--)); do
      # get the sample type by removing the .conf at the end of the sample config file
      sample_config_file="${files[$i]}"
      settings=`echo $sample_config_file | sed -e "s#.*/\([^.]\+\).*#\1#"`
      size=`echo $sample_config_file | sed -e "s#.*/[^.]\+.\([0-9]\+\).conf#\1#"`
      name=`echo $sample_config_file | sed -e "s#\(.*\)/\(.*\)\.conf#\1/\2#"`
      done_file=${population/v[a-z0-9.]*\.conf/$name}.done
      log_file=${population/v[a-z0-9.]*\.conf/$name}.log

      if [ -f $done_file ]; then
        echo "  ${BOLD}$sampler${NORMAL} sample (sample size $size) ${BLUE}[skipping]${NORMAL}"
      else
        command="${!sample_dir} -S --seed $seed -c $sample_config_file ${population/conf/json.tar.gz} ${population/v[a-z0-9.]*\.conf/$name}"

        if [ ! -z $sbatch ]; then
          batch_file=${index}__${sampler}__${name}.sh
          time="04:00:00"
          if [ "circle_gps" == $sampler ] || [ "square_gps" == $sampler ]; then
            if [[ $sample_config_file == *"07"* ]]; then time="16:00:00"; else time="2-00:00"; fi
          fi
          printf "#!/bin/bash\n#SBATCH --time=$time\n#SBATCH --mem=8000M\n#SBATCH --output=$log_file\n$command" > $batch_file
          sbatch $batch_file && touch $done_file
        elif [ ! -z $sqsub ]; then
          time="4h"
          if [ "circle_gps" == $sampler ] || [ "square_gps" == $sampler ]; then
            if [[ $sample_config_file == *"07"* ]]; then time="16h"; else time="48h"; fi
          fi
          sqsub -r $time --mpp=8g -q serial -o $log_file $command && touch $done_file
        else
          $command > $log_file && touch $done_file && \
            echo "  ${BOLD}$sampler${NORMAL} sample (sample size $size) ${GREEN}[done]${NORMAL}"
        fi
      fi
    done
  done

done
