#!/bin/bash
# 
# A script used to prepare a large batch of runs using a hypercube of settings.
# 
# Program:  sampsim
# Module:   hypercube.sh
# Language: bash
# 
#########################################################################################

# directories
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
CFG_DIR=cfg
LOG_DIR=log
OUT_DIR=out

# ansi colors
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
black='\e[0;30m'
dark_gray='\e[1;30m'
blue='\e[0;34m'
light_blue='\e[1;34m'
green='\e[0;32m'
light_green='\e[1;32m'
cyan='\e[0;36m'
light_cyan='\e[1;36m'
red='\e[0;31m'
light_red='\e[1;31m'
purple='\e[0;35m'
light_purple='\e[1;35m'
brown='\e[0;33m'
yellow='\e[1;33m'
light_gray='\e[0;37m'
white='\e[1;37m'
NC='\e[0m' # No Color

# parameter values
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
param_name[0]="seed"
param_value[0]="1"
param_help[0]="Seed used by the random generator"

param_name[1]="towns"
param_value[1]="1"
param_help[1]="Number of towns to generate"

param_name[2]="town_size_min"
param_value[2]="10000"
param_help[2]="The minimum number of individuals in a town"

param_name[3]="town_size_max"
param_value[3]="1000000"
param_help[3]="The maximum number of individuals in a town"

param_name[4]="town_size_shape"
param_value[4]="1.0"
param_help[4]="The shape parameter used by the town size Parato distribution"

param_name[5]="tile_x"
param_value[5]="10"
param_help[5]="Number of tiles in the horizontal direction"

param_name[6]="tile_y"
param_value[6]="10"
param_help[6]="Number of tiles in the vertical direction"

param_name[7]="tile_width"
param_value[7]="3.5"
param_help[7]="Width of a tile in kilometers"

param_name[8]="popdens_mx"
param_value[8]="0"
param_help[8]="Population density trend's X coefficient (must be [-1,1])"

param_name[9]="popdens_my"
param_value[9]="0"
param_help[9]="Population density trend's Y coefficient (must be [-1,1])"

param_name[10]="mean_household_pop"
param_value[10]="3.2"
param_help[10]="Mean number of individuals per household"

param_name[11]="river_probability"
param_value[11]="0.5"
param_help[11]="The probability that a town has a river (must be [0,1])"

param_name[12]="river_width"
param_value[12]="400"
param_help[12]="How wide to make rivers, in meters (must be smaller than tile width)"

param_name[13]="disease_pockets"
param_value[13]="4"
param_help[13]="Number of disease pockets to generate"

param_name[14]="pocket_kernel_type"
param_value[14]="exponential"
param_help[14]="Number of disease pockets to generate"

param_name[15]="pocket_scaling"
param_value[15]="1.0"
param_help[15]="Number of disease pockets to generate"

param_name[16]="mean_income_b00"
param_value[16]="0.75"
param_help[16]="Mean income trend's independent coefficient base value"

param_name[17]="mean_income_b01"
param_value[17]="0.00714"
param_help[17]="Mean income trend's X coefficient base value"

param_name[18]="mean_income_b10"
param_value[18]="0.00714"
param_help[18]="Mean income trend's Y coefficient base value"

param_name[19]="mean_income_b02"
param_value[19]="0"
param_help[19]="Mean income trend's X^2 coefficient base value"

param_name[20]="mean_income_b20"
param_value[20]="0"
param_help[20]="Mean income trend's Y^2 coefficient base value"

param_name[21]="mean_income_b11"
param_value[21]="0"
param_help[21]="Mean income trend's XY coefficient base value"

param_name[22]="sd_income_b00"
param_value[22]="0.25"
param_help[22]="SD income trend's independent coefficient base value"

param_name[23]="sd_income_b01"
param_value[23]="0"
param_help[23]="SD income trend's X coefficient base value"

param_name[24]="sd_income_b10"
param_value[24]="0"
param_help[24]="SD income trend's Y coefficient base value"

param_name[25]="sd_income_b02"
param_value[25]="0"
param_help[25]="SD income trend's X^2 coefficient base value"

param_name[26]="sd_income_b20"
param_value[26]="0"
param_help[26]="SD income trend's Y^2 coefficient base value"

param_name[27]="sd_income_b11"
param_value[27]="0"
param_help[27]="SD income trend's XY coefficient base value"

param_name[28]="mean_disease_b00"
param_value[28]="0.5"
param_help[28]="Mean disease trend's independent coefficient base value"

param_name[29]="mean_disease_b01"
param_value[29]="0"
param_help[29]="Mean disease trend's X coefficient base value"

param_name[30]="mean_disease_b10"
param_value[30]="0"
param_help[30]="Mean disease trend's Y coefficient base value"

param_name[31]="mean_disease_b02"
param_value[31]="0"
param_help[31]="Mean disease trend's X^2 coefficient base value"

param_name[32]="mean_disease_b20"
param_value[32]="0"
param_help[32]="Mean disease trend's Y^2 coefficient base value"

param_name[33]="mean_disease_b11"
param_value[33]="0"
param_help[33]="Mean disease trend's XY coefficient base value"

param_name[34]="sd_disease_b00"
param_value[34]="0.25"
param_help[34]="SD disease trend's independent coefficient base value"

param_name[35]="sd_disease_b01"
param_value[35]="0"
param_help[35]="SD disease trend's X coefficient base value"

param_name[36]="sd_disease_b10"
param_value[36]="0"
param_help[36]="SD disease trend's Y coefficient base value"

param_name[37]="sd_disease_b02"
param_value[37]="0"
param_help[37]="SD disease trend's X^2 coefficient base value"

param_name[38]="sd_disease_b20"
param_value[38]="0"
param_help[38]="SD disease trend's Y^2 coefficient base value"

param_name[39]="sd_disease_b11"
param_value[39]="0"
param_help[39]="SD disease trend's XY coefficient base value"

param_name[40]="dweight_population"
param_value[40]="1.0"
param_help[40]="Disease weight for household population"

param_name[41]="dweight_income"
param_value[41]="1.0"
param_help[41]="Disease weight for household income"

param_name[42]="dweight_risk"
param_value[42]="1.0"
param_help[42]="Disease weight for household risk"

param_name[43]="dweight_age"
param_value[43]="1.0"
param_help[43]="Disease weight for household age"

param_name[44]="dweight_sex"
param_value[44]="1.0"
param_help[44]="Disease weight for household sex"

param_name[45]="dweight_pocket"
param_value[45]="1.0"
param_help[45]="Disease weight for pocketing"

# functions
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-

# Used to join the elements of an array into a string.
# arg1: delimiter
# arg2: array
function join
{
   local IFS="$1"; shift; echo "$*";
}

# Recursive function to create nested directories
# arg1: directory (start with base directory, the recursion takes care of the rest)
# arg2: index (start with 0, the recursion takes care of the rest)
function create_config_tree
{
  if [ $( echo "$2 < ${#name_array[@]}" | bc ) -ne 0 ]; then
    if [[ ${value_array[$2]} == *:* ]]; then
      # make the base directory
      mkdir "$1/${name_array[$2]}"
      array=(${value_array[$2]//:/ })
      for val in "${array[@]}"; do
        sub_dir="$1/${name_array[$2]}/${val}"
        # make the value directories
        mkdir $sub_dir
        # go to the next parameter and repeat recursively
        blah $sub_dir $( echo "$2 + 1" | bc )
      done
    else
      blah $sub_dir $( echo "$2 + 1" | bc )
    fi  
  fi  
}

# preamble
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
echo This script will build a list of configurations.
echo
echo \
"For every parameter you may select either the default value, a custom value or a
range of values.  A hypercube of configurations will be created for every value
of every parameter which has a range of values."

# get target directory
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
echo -e "What directory do you wish to create the hypercube in? (default: ${blue}./${NC})
${red}warning: this will overwrite any existing directory${NC}"
read -e -r -p "> " directory
if [ "${directory:0:1}" != "/" ]; then
  directory="./${directory}"
fi
# fix spaces and backslashes
directory=$( echo ${directory} | sed "s/ /\\\ /g" | sed "s/\\\\\+/\\\\/g" ) 
if [ -d "$directory" ]; then
  rm -rf ${directory}
fi
mkdir -p ${directory}

# get parameter values
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
for index in ${!param_name[*]}; do
  name=${param_name[$index]}
  default=${param_value[$index]}
  help=${param_help[$index]}

  echo
  echo -e "Which parameter do you wish to use for ${light_blue}${name}${NC}? \
(default: ${blue}${default}${NC})"
  echo -e "Description: ${dark_gray}${help}${NC}"

  while true; do
    echo -n -e "Please select \
${white}d${NC}efault, ${white}c${NC}ustom, ${white}r${NC}ange or ${white}a${NC}rray> "
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
      read -p "Provide the lowest value in the range: " lower
      read -p "Provide the highest value in the range: " upper
      read -p "Provide the step between values in the range: " step

      value=
      current=${lower}
      while [ $( echo "${current} <= ${upper}" | bc ) -eq 1 ]; do
        value="${value}:${current}"
        current=$( echo "${current} + ${step}" | bc )
      done
      param_value[$index]=${value:1} # remove the first character

      break;
    elif [ "$answer" = "a" ]; then

      read -a array -p "Provide a list of values delimited by a space: "
      param_value[$index]="$(join : ${array[@]})"

      break;
    else
      echo -e "${red}Invalid input, please select d, c or r${NC}"
    fi
  done
done

# create directory structure
# -+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
create_config_tree $directory 0
