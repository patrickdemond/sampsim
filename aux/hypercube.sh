#!/bin/bash
# 
# A script used to prepare a large batch of runs using a hypercube of settings.
# 
# Program:  sampsim
# Module:   hypercube.sh
# Language: bash
# TODO:     Latin hypercube isn't using a seed
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

# parameter values
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
param_name=()
param_short=()
param_value=()
param_help=()

param_name+=("seed")
param_short+=("s")
param_value+=("1")
param_help+=("Seed used by the random generator")

param_name+=("target_prevalence")
param_short+=("tp")
param_value+=("0.5")
param_help+=("The population's target mean disease prevalence")

param_name+=("towns")
param_short+=("t")
param_value+=("250")
param_help+=("Number of towns to generate")

param_name+=("town_size_min")
param_short+=("tsm")
param_value+=("250")
param_help+=("The minimum number of individuals in a town")

param_name+=("town_size_max")
param_short+=("tsM")
param_value+=("250000")
param_help+=("The maximum number of individuals in a town")

param_name+=("town_size_shape")
param_short+=("tss")
param_value+=("0.819")
param_help+=("The shape parameter used by the town size Parato distribution")

param_name+=("tile_x")
param_short+=("tx")
param_value+=("10")
param_help+=("Number of tiles in the horizontal direction")

param_name+=("tile_y")
param_short+=("ty")
param_value+=("10")
param_help+=("Number of tiles in the vertical direction")

param_name+=("tile_width")
param_short+=("tw")
param_value+=("1.0")
param_help+=("Width of a tile in kilometers")

param_name+=("popdens_mx")
param_short+=("px")
param_value+=("0")
param_help+=("Population density trend's X coefficient (must be [-1,1])")

param_name+=("popdens_my")
param_short+=("py")
param_value+=("0")
param_help+=("Population density trend's Y coefficient (must be [-1,1])")

param_name+=("mean_household_pop")
param_short+=("mhp")
param_value+=("4")
param_help+=("Mean number of individuals per household")

param_name+=("river_probability")
param_short+=("rp")
param_value+=("0")
param_help+=("The probability that a town has a river (must be [0,1])")

param_name+=("river_width")
param_short+=("rw")
param_value+=("0")
param_help+=("How wide to make rivers, in meters (must be smaller than tile width)")

param_name+=("disease_pockets")
param_short+=("dp")
param_value+=("0")
param_help+=("Number of disease pockets to generate")

param_name+=("pocket_kernel_type")
param_short+=("pk")
param_value+=("exponential")
param_help+=("The type of kernel to use for disease pockets")

param_name+=("pocket_scaling")
param_short+=("ps")
param_value+=("1.0")
param_help+=("The scaling factor to use for disease pocket")

param_name+=("mean_income_b00")
param_short+=("mi00")
param_value+=("0.75,0.5")
param_help+=("Mean income trend's independent coefficient base value")

param_name+=("mean_income_b01")
param_short+=("mi01")
param_value+=("0.00714,0.1")
param_help+=("Mean income trend's X coefficient base value")

param_name+=("mean_income_b10")
param_short+=("mi10")
param_value+=("0.00714,0.1")
param_help+=("Mean income trend's Y coefficient base value")

param_name+=("mean_income_b02")
param_short+=("mi02")
param_value+=("0")
param_help+=("Mean income trend's X^2 coefficient base value")

param_name+=("mean_income_b20")
param_short+=("mi20")
param_value+=("0")
param_help+=("Mean income trend's Y^2 coefficient base value")

param_name+=("mean_income_b11")
param_short+=("mi11")
param_value+=("0")
param_help+=("Mean income trend's XY coefficient base value")

param_name+=("sd_income_b00")
param_short+=("si00")
param_value+=("0.25")
param_help+=("SD income trend's independent coefficient base value")

param_name+=("sd_income_b01")
param_short+=("si01")
param_value+=("0")
param_help+=("SD income trend's X coefficient base value")

param_name+=("sd_income_b10")
param_short+=("si10")
param_value+=("0")
param_help+=("SD income trend's Y coefficient base value")

param_name+=("sd_income_b02")
param_short+=("si02")
param_value+=("0")
param_help+=("SD income trend's X^2 coefficient base value")

param_name+=("sd_income_b20")
param_short+=("si20")
param_value+=("0")
param_help+=("SD income trend's Y^2 coefficient base value")

param_name+=("sd_income_b11")
param_short+=("si11")
param_value+=("0")
param_help+=("SD income trend's XY coefficient base value")

param_name+=("mean_disease_b00")
param_short+=("md00")
param_value+=("0")
param_help+=("Mean disease trend's independent coefficient base value")

param_name+=("mean_disease_b01")
param_short+=("md01")
param_value+=("0")
param_help+=("Mean disease trend's X coefficient base value")

param_name+=("mean_disease_b10")
param_short+=("md10")
param_value+=("0")
param_help+=("Mean disease trend's Y coefficient base value")

param_name+=("mean_disease_b02")
param_short+=("md02")
param_value+=("0")
param_help+=("Mean disease trend's X^2 coefficient base value")

param_name+=("mean_disease_b20")
param_short+=("md20")
param_value+=("0")
param_help+=("Mean disease trend's Y^2 coefficient base value")

param_name+=("mean_disease_b11")
param_short+=("md11")
param_value+=("0")
param_help+=("Mean disease trend's XY coefficient base value")

param_name+=("sd_disease_b00")
param_short+=("sd00")
param_value+=("0")
param_help+=("SD disease trend's independent coefficient base value")

param_name+=("sd_disease_b01")
param_short+=("sd01")
param_value+=("0")
param_help+=("SD disease trend's X coefficient base value")

param_name+=("sd_disease_b10")
param_short+=("sd10")
param_value+=("0")
param_help+=("SD disease trend's Y coefficient base value")

param_name+=("sd_disease_b02")
param_short+=("sd02")
param_value+=("0")
param_help+=("SD disease trend's X^2 coefficient base value")

param_name+=("sd_disease_b20")
param_short+=("sd20")
param_value+=("0")
param_help+=("SD disease trend's Y^2 coefficient base value")

param_name+=("sd_disease_b11")
param_short+=("sd11")
param_value+=("0")
param_help+=("SD disease trend's XY coefficient base value")

param_name+=("dweight_population")
param_short+=("wpn")
param_value+=("0.0")
param_help+=("Disease weight for household population")

param_name+=("dweight_income")
param_short+=("wi")
param_value+=("1.0")
param_help+=("Disease weight for household income")

param_name+=("dweight_risk")
param_short+=("wr")
param_value+=("0.0")
param_help+=("Disease weight for household risk")

param_name+=("dweight_age")
param_short+=("wa")
param_value+=("0.0")
param_help+=("Disease weight for household age")

param_name+=("dweight_sex")
param_short+=("ws")
param_value+=("0.0")
param_help+=("Disease weight for household sex")

param_name+=("dweight_pocket")
param_short+=("wpt")
param_value+=("0.0")
param_help+=("Disease weight for pocketing")

# functions
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-

# Used to display a progress meter
function progress_meter
{
  local width=50
  local message=$1
  local progress=$2
  local total=$3
  local percent=$( echo "scale=2; $progress / $total" | bc )
  local ticks=$( echo "$percent * $width" | bc | awk '{printf "%.0f", $0}' )
  local spaces=$( echo "$width - $ticks" | bc )

  if [ $progress -eq 1 ]; then
    last_progress_meter_ticks=-1
  fi
  
  if [ $ticks -ne $last_progress_meter_ticks ]; then
    local meter="";
    if [ $ticks -ne 0 ]; then
      meter+=`printf "%0.s#" $(seq 1 $ticks)`
    fi
    if [ $spaces -ne 0 ]; then
      meter+=`printf "%0.s " $(seq 1 $spaces)`
    fi
    let COL=$(tput cols)-${#message}+${#MAGENTA}+${#NORMAL}
    printf "\r%s%${COL}s" "$message" "[${MAGENTA}${meter}${NORMAL}]"
  fi
  last_progress_meter_ticks=$ticks
} 


# Used to join the elements of an array into a string.
# arg1: delimiter
# arg2: array
function join
{
  local IFS="$1"; shift; echo "$*";
}

# Recursive function to create nested directories
# arg1: directory (start with base directory, the recursion takes care of the rest)
# arg2: name_array (the name of the name array)
# arg3: value_array (the name of the value array)
# arg4: index (start with 0, the recursion takes care of the rest)
# arg5: total (the number of possible configuration files to be generated)
function create_config_tree
{
  local directory=$1
  local arr=$2"[*]"
  local name_array=(${!arr})
  local arr=$3"[*]"
  local value_array=(${!arr})
  local index=$4
  local total=$5

  # initialize when in the root of the tree
  if [ $index -eq 0 ]; then
    progress=0
  fi

  if [ $index -lt "${#name_array[@]}" ]; then
    # make the base directory
    mkdir -p "$directory/${name_array[$index]}"
    local array=(${value_array[$index]//:/ })
    for val in "${array[@]}"; do
      # determine and display progress
      let "progress++"
      progress_meter "Creating configuration tree" $progress $total
      sub_dir="$directory/${name_array[$index]}/v$val"
      mkdir -p $sub_dir
      # go to the next parameter and repeat recursively
      create_config_tree $sub_dir name_array value_array $( echo "$index + 1" | bc ) $total
    done
  fi
}

# Recursive function to create nested directories
# arg1: directory (start with base directory, the recursion takes care of the rest)
# arg2: name_array (the name of the name array)
# arg3: value_array (the name of the value array)
# arg4: index (start with 0, the recursion takes care of the rest)
# arg5: number_of_points (only used in first iteration and when latin is 1)
function create_latin_config_tree
{
  local directory=$1
  local arr=$2"[*]"
  local name_array=(${!arr})
  local arr=$3"[*]"
  local value_array=(${!arr})
  local index=$4
  local number_of_points=$5

  # initialize when in the root of the tree
  if [ $index -eq 0 ]; then
    # determine the maximum number of values in each index of the value array
    maximums="";
    for value in "${value_array[@]}"; do
      local array=(${value//:/ })
      local array_size=${#array[@]}
      if [ "" != "$maximums" ]; then
        maximums="$maximums,"
      fi
      maximums="$maximums$array_size"
    done

    declare -r latin_points=`\
      ../latin_hypercube --index0 \
                         --dims ${#value_array[@]} \
                         --points $number_of_points \
                         --maximums $maximums`
    root_array=(${value_array[0]//:/ })
    root_array_size=${#root_array[@]}
  fi

  if [ $index -lt "${#name_array[@]}" ]; then
    # make the base directory
    mkdir -p "$directory/${name_array[$index]}"
    local array=(${value_array[$index]//:/ })
    local array_size=${#array[@]}
    local found=0

    # loop through every latin point
    for point in $latin_points; do
      # if we at the root then start counting the root index make note of the root point
      if [ $index -eq 0 ]; then
        root_index=0
        root_point=$point
      else # otherwise only continue if the non-root point matches the root point
        if [ "$point" != "$root_point" ]; then continue; fi
      fi;
      local val_index=0
      for val in "${array[@]}"; do
        # get the indeces of the points corresponding to the root and current branches of the tree
        declare -i loop_index=0
        declare -i test_index
        declare -i min_index
        declare -i max_index
        declare -i root_point_index=-1
        declare -i branch_point_index=-1

        # get the root and branch point indeces
        root_point_index=$( echo $point | cut -d "," -f 1 )
        branch_point_index=$( echo $point | cut -d "," -f $(( $index+1 )) )

        # only generate if we aren't using a latin hypercube, or the root/branch indeces match
        if [ "$root_index" -eq "$root_point_index" ] && [ "$val_index" -eq "$branch_point_index" ]; then
          sub_dir="$directory/${name_array[$index]}/v$val"
          mkdir -p $sub_dir
          # go to the next parameter and repeat recursively
          create_latin_config_tree $sub_dir name_array value_array $( echo "$index + 1" | bc ) $number_of_points
          found=1
          if [ $found -eq 1 ]; then break; fi;
        fi

        # increment to the next point, but only when in the tree's root
        [ $index -eq 0 ] && ((root_index++))
        ((val_index++))
      done
      if [ $index -gt 0 ] && [ $found -eq 1 ]; then break; fi;
    done
  fi
}

# preamble
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
echo This script will create a hypercube of configurations into a directory tree.
echo
echo \
"For every parameter you may select either the default value, a custom value or a
range of values.  A hypercube of configurations will be created for every value
of every parameter which has a range of values.  Or, if you choose the random latin
hypercube sampling option, then a subset of configurations will be created such that
they fulfil a random latin hypercube."

# get target directory
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
directory="./hypercube"
echo "What directory do you wish to create the hypercube in? (default: ${BLUE}$directory${NORMAL})
${RED}warning: this will overwrite any existing directory${NORMAL}"
read -e -r -p "> " answer

# make empty responses the default
if [ "$answer" ]; then
  directory=$answer
fi

# append a ./ if there isn't one already and the path isn't absolute (starting with /)
if [[ "${directory:0:1}" != "/" || ${directory:0:2} != "./" ]]; then
  directory="./$directory"
fi

# fix spaces and backslashes
directory=$( echo $directory | sed "s/ /\\\ /g" | sed "s/\\\\\+/\\\\/g" )
if [ -d "$directory" ]; then
  rm -rf $directory
fi

# create directory and put the generate and build script in it
mkdir -p $directory
cp ../generate $directory
cp build.sh $directory
chmod 755 $directory/build.sh
      
number_pattern="^-?((([0-9]|[1-9][0-9]*)(\.[0-9]*)?)|(\.[0-9]+))$" # any number
non_zero_number_pattern="^-?((([1-9][0-9]*)(\.[0-9]*)?)|(0?\.[0-9]*[1-9]+[0-9]*))$" # any non-zero number

# latin or full hypercube?
latin=1
echo "You must now choose whether to create the full hypercube or a random latin hypercube subset of
configurations.  The full hypercube option will generate every possible variation of all parameter sets
provided, which, depending on the paramters you select may grow restrictively large.  The random latin
hypercube option will create a reduced subset such that only one parameter value will be used for each
hyperplane in the hypercube."
while true; do
  echo -n "Do you wish to only generate a random latin hypercube subset of configurations? (select ${BOLD}${YELLOW}y${NORMAL}es or ${BOLD}${YELLOW}n${NORMAL}o)> "
  read -s -n 1 answer
  echo
  if [ "$answer" = "y" ]; then
    latin=1
    break;
  elif [ "$answer" = "n" ]; then
    latin=0
    break;
  else
    echo "${RED}Invalid input, please select ${BOLD}${YELLOW}y${NORMAL}${RED} or ${BOLD}${YELLOW}n${NORMAL}"
  fi
done

# determine whether to use regular or short parameter names
short=1
echo "Depending on the number of variable parameters in the hypercube the full path and name of
generated configuration files may become excessively long.  To mitigate this effect it is possible
to use short forms of parameter names in configuration file paths.  This option is recommended for
hypercubes containing 4 or more variable parameters."
while true; do
  echo -n "Do you wish to use short parameter names? (select ${BOLD}${YELLOW}y${NORMAL}es or ${BOLD}${YELLOW}n${NORMAL}o)> "
  read -s -n 1 answer
  echo
  if [ "$answer" = "y" ]; then
    short=1
    break;
  elif [ "$answer" = "n" ]; then
    short=0
    break;
  else
    echo "${RED}Invalid input, please select ${BOLD}${YELLOW}y${NORMAL}${RED} or ${BOLD}${YELLOW}n${NORMAL}"
  fi
done

# get parameter values
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
skip=0
for index in ${!param_name[*]}; do
  name=${param_name[$index]}
  default=${param_value[$index]}
  help=${param_help[$index]}

  echo
  echo "Which parameter do you wish to use for ${BOLD}${CYAN}${name}${NORMAL}? \
(default: ${BLUE}${default}${NORMAL})"
  echo "Description: ${MAGENTA}${help}${NORMAL}"

  while true; do
    echo -n "Please select \
${BOLD}${YELLOW}d${NORMAL}efault, \
${BOLD}${YELLOW}c${NORMAL}ustom, \
${BOLD}${YELLOW}r${NORMAL}ange, \
${BOLD}${YELLOW}a${NORMAL}rray or \
${BOLD}${YELLOW}f${NORMAL}inish with default values> "
    read -s -n 1 answer
    echo
    if [ "$answer" = "d" ] || [ -z "$answer" ]; then
      # do nothing
      break;
    elif [ "$answer" = "c" ]; then
      # get the custom value
      read -p "Provide a custom value: " param_value[$index]

      break;
    elif [ "$answer" = "r" ]; then
      # get the lower value in the range
      while true; do
        read -p "Provide the lowest value in the range: " lower
        if [[ ! $lower =~ $number_pattern ]]; then
          echo "${RED}ERROR: 'lower' must be a number${NORMAL}"
        else
          break
        fi
      done

      # get the upper value in the range
      while true; do
        read -p "Provide the highest value in the range: " upper
        if [[ ! $upper =~ $number_pattern ]]; then
          echo "${RED}ERROR: 'upper' must be a number${NORMAL}"
        elif [ $( echo "$lower == $upper" | bc ) -eq 1 ]; then
          echo "${RED}ERROR: 'upper' cannot be the same value as 'lower'${NORMAL}"
        else
          break
        fi
      done

      # get the step value of the range
      while true; do
        read -p "Provide the step between values in the range: " step
        if [[ ! $step =~ $non_zero_pattern ]]; then
          echo "${RED}ERROR: 'step' must be a non-zero number${NORMAL}"
        elif [ $( echo "($lower < $upper && $step > 0) || ($lower > $upper && $step < 0)" | bc ) -eq 0 ]; then
          echo "${RED}ERROR: 'step' is in the wrong direction${NORMAL}"
        else
          break
        fi
      done

      value=""
      current=$lower
      while [ $( echo "$current <= $upper" | bc ) -eq 1 ]; do
        value="$value:${current}"
        current=$( echo "$current + $step" | bc )
        # add the leading 0 if the number starts with .
        if [ '.' = "${current:0:1}" ]; then
          current="0$current"
        fi
      done
      param_value[$index]=${value:1} # remove the first character

      break;
    elif [ "$answer" = "a" ]; then

      while true; do
        read -a array -p "Provide a list of values delimited by a space: "

        if [ ${#array[@]} -eq 0 ]; then
          echo "${RED}ERROR: The array cannot be empty${NORMAL}"
        else
          # validate all inputs as numbers
          test=1
          for idx in ${!array[*]}; do
            if [[ ! ${array[$idx]} =~ $number_pattern ]]; then
              echo "${RED}ERROR: Array can only contain numbers${NORMAL}"
              test=0
              break
            fi
          done
          if [ $test -eq 1 ]; then
            break
          fi
        fi
      done

      # the array is now valid, put into a string deliminating by :
      param_value[$index]=$(join : ${array[@]})

      break;
    elif [ "$answer" = "f" ]; then
      # get the custom value
      echo "Proceeding with default values for remaining parameters"
      skip=1
      break;
    else
      echo "${RED}Invalid input, please select d, c, r, a or f${NORMAL}"
    fi
  done

  if [ $skip -eq 1 ]; then
    break
  fi
done
echo

# create directory structure
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-

# make an array of all parameters with multiple values and count the total size of the hypercube
idx=0
size=0
mult=1
number_of_points=0 # only used if using a latin hypercube
name_array=()
value_array=()
for index in ${!param_name[*]}; do
  if [[ ${param_value[$index]} == *:* ]]; then
    if [ $short -eq 1 ]; then
      name_array+=(${param_short[$index]})
    else
      name_array+=(${param_name[$index]})
    fi
    value_array+=(${param_value[$index]})
    array=(${value_array[$idx]//:/ })
    array_size=${#array[@]}
    if [ $array_size -gt $number_of_points ]; then
      number_of_points=$array_size
    fi
    mult=$(( $mult * $array_size ))
    size=$(( $size + $mult ))
    ((idx++))
  fi
done

if [ $size -eq 0 ]; then
  echo "${RED}ERROR: You have to specify at least one parameter to have multiple values!${NORMAL}"
  exit
fi

if [ $latin -eq 1 ]; then
  create_latin_config_tree $directory name_array value_array 0 $number_of_points
else
  create_config_tree $directory name_array value_array 0 $size
fi
echo

# now create the config files
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-

# convert all end-level directories into files
depth=$( find $directory -printf '%d\n' | sort -n | tail -n 1 )
((depth++))
search=$( seq -s "/*" $depth | sed 's/[0-9]//g' )
search="$directory$search"
dir_list=$( find $search )
total=$( echo ${dir_list[@]} | wc -w )
declare -i progress=0
for dir in $dir_list; do
  # determine and display progress
  let "progress++"
  progress_meter "Writing configuration files" $progress $total

  # replace directory with file
  conf_file="$dir.conf"
  rm -rf "$dir"
  cp hypercube_template.conf $conf_file

  # get the variable parameters for this config file from the directory
  declare -A variable_params
  sub_dir=${dir#$directory}
  parts=(${sub_dir//\// })
  for (( index=0; index<${#parts[@]}; index+=2 )); do
    name=${parts[$index]}
    value=${parts[$index+1]:1} # remove the "v"
    variable_params[$name]=$value
  done

  # now go through all parameters and write them to the config file
  if [ $short -eq 1 ]; then
    name_array=("${param_short[@]}")
  else
    name_array=("${param_name[@]}")
  fi
  for index in ${!param_name[*]}; do
    short_name=${param_short[$index]}
    long_name=${param_name[$index]}
    if [ $short -eq 1 ] && [ ${variable_params[$short_name]} ]; then
      value=${variable_params[$short_name]}
    elif [ $short -ne 1 ] && [ ${variable_params[$long_name]} ]; then
      value=${variable_params[$long_name]}
    else
      value=${param_value[$index]}
    fi
    sed -i "s/<$long_name>/$value/" $conf_file
  done
done

# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
echo "Finished creating hypercube configuration tree"
