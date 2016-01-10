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
param_name[0]="seed"
param_short[0]="s"
param_value[0]="1"
param_help[0]="Seed used by the random generator"

param_name[1]="towns"
param_short[1]="t"
param_value[1]="1"
param_help[1]="Number of towns to generate"

param_name[2]="town_size_min"
param_short[2]="tsm"
param_value[2]="10000"
param_help[2]="The minimum number of individuals in a town"

param_name[3]="town_size_max"
param_short[3]="tsM"
param_value[3]="1000000"
param_help[3]="The maximum number of individuals in a town"

param_name[4]="town_size_shape"
param_short[4]="tss"
param_value[4]="1.0"
param_help[4]="The shape parameter used by the town size Parato distribution"

param_name[5]="tile_x"
param_short[5]="tx"
param_value[5]="10"
param_help[5]="Number of tiles in the horizontal direction"

param_name[6]="tile_y"
param_short[6]="ty"
param_value[6]="10"
param_help[6]="Number of tiles in the vertical direction"

param_name[7]="tile_width"
param_short[7]="tw"
param_value[7]="3.5"
param_help[7]="Width of a tile in kilometers"

param_name[8]="popdens_mx"
param_short[8]="px"
param_value[8]="0"
param_help[8]="Population density trend's X coefficient (must be [-1,1])"

param_name[9]="popdens_my"
param_short[9]="py"
param_value[9]="0"
param_help[9]="Population density trend's Y coefficient (must be [-1,1])"

param_name[10]="mean_household_pop"
param_short[10]="mhp"
param_value[10]="3.2"
param_help[10]="Mean number of individuals per household"

param_name[11]="river_probability"
param_short[11]="rp"
param_value[11]="0.5"
param_help[11]="The probability that a town has a river (must be [0,1])"

param_name[12]="river_width"
param_short[12]="rw"
param_value[12]="400"
param_help[12]="How wide to make rivers, in meters (must be smaller than tile width)"

param_name[13]="disease_pockets"
param_short[13]="dp"
param_value[13]="4"
param_help[13]="Number of disease pockets to generate"

param_name[14]="pocket_kernel_type"
param_short[14]="pk"
param_value[14]="exponential"
param_help[14]="The type of kernel to use for disease pockets"

param_name[15]="pocket_scaling"
param_short[15]="ps"
param_value[15]="1.0"
param_help[15]="The scaling factor to use for disease pocket"

param_name[16]="mean_income_b00"
param_short[16]="mi00"
param_value[16]="0.75"
param_help[16]="Mean income trend's independent coefficient base value"

param_name[17]="mean_income_b01"
param_short[17]="mi01"
param_value[17]="0.00714"
param_help[17]="Mean income trend's X coefficient base value"

param_name[18]="mean_income_b10"
param_short[18]="mi10"
param_value[18]="0.00714"
param_help[18]="Mean income trend's Y coefficient base value"

param_name[19]="mean_income_b02"
param_short[19]="mi02"
param_value[19]="0"
param_help[19]="Mean income trend's X^2 coefficient base value"

param_name[20]="mean_income_b20"
param_short[20]="mi20"
param_value[20]="0"
param_help[20]="Mean income trend's Y^2 coefficient base value"

param_name[21]="mean_income_b11"
param_short[21]="mi11"
param_value[21]="0"
param_help[21]="Mean income trend's XY coefficient base value"

param_name[22]="sd_income_b00"
param_short[22]="si00"
param_value[22]="0.25"
param_help[22]="SD income trend's independent coefficient base value"

param_name[23]="sd_income_b01"
param_short[23]="si01"
param_value[23]="0"
param_help[23]="SD income trend's X coefficient base value"

param_name[24]="sd_income_b10"
param_short[24]="si10"
param_value[24]="0"
param_help[24]="SD income trend's Y coefficient base value"

param_name[25]="sd_income_b02"
param_short[25]="si02"
param_value[25]="0"
param_help[25]="SD income trend's X^2 coefficient base value"

param_name[26]="sd_income_b20"
param_short[26]="si20"
param_value[26]="0"
param_help[26]="SD income trend's Y^2 coefficient base value"

param_name[27]="sd_income_b11"
param_short[27]="si11"
param_value[27]="0"
param_help[27]="SD income trend's XY coefficient base value"

param_name[28]="mean_disease_b00"
param_short[28]="md00"
param_value[28]="0.5"
param_help[28]="Mean disease trend's independent coefficient base value"

param_name[29]="mean_disease_b01"
param_short[29]="md01"
param_value[29]="0"
param_help[29]="Mean disease trend's X coefficient base value"

param_name[30]="mean_disease_b10"
param_short[30]="md10"
param_value[30]="0"
param_help[30]="Mean disease trend's Y coefficient base value"

param_name[31]="mean_disease_b02"
param_short[31]="md02"
param_value[31]="0"
param_help[31]="Mean disease trend's X^2 coefficient base value"

param_name[32]="mean_disease_b20"
param_short[32]="md20"
param_value[32]="0"
param_help[32]="Mean disease trend's Y^2 coefficient base value"

param_name[33]="mean_disease_b11"
param_short[33]="md11"
param_value[33]="0"
param_help[33]="Mean disease trend's XY coefficient base value"

param_name[34]="sd_disease_b00"
param_short[34]="sd00"
param_value[34]="0.25"
param_help[34]="SD disease trend's independent coefficient base value"

param_name[35]="sd_disease_b01"
param_short[35]="sd01"
param_value[35]="0"
param_help[35]="SD disease trend's X coefficient base value"

param_name[36]="sd_disease_b10"
param_short[36]="sd10"
param_value[36]="0"
param_help[36]="SD disease trend's Y coefficient base value"

param_name[37]="sd_disease_b02"
param_short[37]="sd02"
param_value[37]="0"
param_help[37]="SD disease trend's X^2 coefficient base value"

param_name[38]="sd_disease_b20"
param_short[38]="sd20"
param_value[38]="0"
param_help[38]="SD disease trend's Y^2 coefficient base value"

param_name[39]="sd_disease_b11"
param_short[39]="sd11"
param_value[39]="0"
param_help[39]="SD disease trend's XY coefficient base value"

param_name[40]="dweight_population"
param_short[40]="wpn"
param_value[40]="1.0"
param_help[40]="Disease weight for household population"

param_name[41]="dweight_income"
param_short[41]="wi"
param_value[41]="1.0"
param_help[41]="Disease weight for household income"

param_name[42]="dweight_risk"
param_short[42]="wr"
param_value[42]="1.0"
param_help[42]="Disease weight for household risk"

param_name[43]="dweight_age"
param_short[43]="wa"
param_value[43]="1.0"
param_help[43]="Disease weight for household age"

param_name[44]="dweight_sex"
param_short[44]="ws"
param_value[44]="1.0"
param_help[44]="Disease weight for household sex"

param_name[45]="dweight_pocket"
param_short[45]="wpt"
param_value[45]="1.0"
param_help[45]="Disease weight for pocketing"

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
    declare -r latin_points=`../latin_hypercube --index --dims ${#value_array[@]} --points $number_of_points`
#echo $latin_points
  fi

  if [ $index -lt "${#name_array[@]}" ]; then
    # make the base directory
    mkdir -p "$directory/${name_array[$index]}"
    local array=(${value_array[$index]//:/ })
    local array_size=${#array[@]}
    local found=0

    # loop through every latin point
    for point in $latin_points; do
#echo "point: $point"
      if [ $index -eq 0 ]; then root_index=0; fi;
      local val_index=0
      for val in "${array[@]}"; do
#echo "INDEX IS $index AND VAL IS $val"
        # get the indeces of the points corresponding to the root and current branches of the tree
        declare -i loop_index=0
        declare -i test_index
        declare -i min_index
        declare -i max_index
        declare -i root_point_index=-1
        declare -i branch_point_index=-1

        # get the root point index then convert if the array size is less than the number of points
        root_point_index=$( echo $point | cut -d "," -f 1 )
        if [ "$number_of_points" -ne "$array_size" ]; then
          root_point_index=$( printf "%.*f" 0\
            $( echo "scale=8; $root_point_index/$number_of_points*$array_size" | bc )\
          )
        fi

        # get the branch point index then convert if the array size is less than the number of points
        branch_point_index=$( echo $point | cut -d "," -f $(( $index+1 )) )
        if [ "$number_of_points" -ne "$array_size" ]; then
          branch_point_index=$( printf "%.*f" 0\
            $( echo "scale=8; $branch_point_index/$number_of_points*$array_size" | bc )\
          )
        fi

        # only generate if we aren't using a latin hypercube, or the root/branch indeces match
#echo "looking for ($root_point_index,$branch_point_index) currently in ($root_index,$val_index)"
        if [ "$root_index" -eq "$root_point_index" ] && [ "$val_index" -eq "$branch_point_index" ]; then
          sub_dir="$directory/${name_array[$index]}/v$val"
#echo "making $sub_dir"
          mkdir -p $sub_dir
          # go to the next parameter and repeat recursively
#echo
#echo "STARTING $( echo "$index + 1" | bc )"
#echo
          create_latin_config_tree $sub_dir name_array value_array $( echo "$index + 1" | bc ) $number_of_points
          found=1
#echo
#echo "FINISHED $( echo "$index + 1" | bc )"
#echo
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
echo "You must nose choose whether to create the full hypercube or a random latin hypercube subset of
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
  for index in ${!param_name[*]}; do
    name=${param_name[$index]}
    if [ ${variable_params[$name]} ]; then
      value=${variable_params[$name]}
    else
      value=${param_value[$index]}
    fi
    sed -i "s/<$name>/$value/" $conf_file
  done
done

# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
echo "Finished creating hypercube configuration tree"
