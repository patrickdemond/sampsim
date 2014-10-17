#!/bin/bash

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

echo -n -e "${black}black${NC}  "
echo -n -e "${dark_gray}dark_gray${NC}  "
echo -n -e "${blue}blue${NC}  "
echo -n -e "${light_blue}light_blue${NC}  "
echo -n -e "${green}green${NC}  "
echo -n -e "${light_green}light_green${NC}  "
echo -n -e "${cyan}cyan${NC}  "
echo -n -e "${light_cyan}light_cyan${NC}  "
echo -n -e "${red}red${NC}  "
echo
echo -n -e "${light_red}light_red${NC}  "
echo -n -e "${purple}purple${NC}  "
echo -n -e "${light_purple}light_purple${NC}  "
echo -n -e "${brown}brown${NC}  "
echo -n -e "${yellow}yellow${NC}  "
echo -n -e "${light_gray}light_gray${NC}  "
echo -n -e "${white}white${NC}  "
echo

directory="./test"
name_array[0]="foo"
value_array[0]="1:2:3"
name_array[1]="bar"
value_array[1]="10:20:30"
name_array[2]="asdf"
value_array[2]="10"
name_array[3]="qwer"
value_array[3]="a:b"

function blah
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

mkdir -p $directory
blah $directory 0
