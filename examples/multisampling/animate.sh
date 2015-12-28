#!/bin/bash
# 
# This script creates animations displaying each sample
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
  echo -n "Do you wish to replace existing data (.avi) files? (select ${BOLD}${YELLOW}y${NORMAL}es or ${BOLD}${YELLOW}n${NORMAL}o)> "
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

# delete all existing csv, json, log, done and png files if we are replacing them
if [ $replace -eq 1 ]; then
  rm `find -type f | grep "\.\(avi\)$"`
fi

# ignore file searches that come up empty
shopt -s nullglob

# loop over all weight types
echo ""
echo "Generating animations"
for file in */*.s001.png; do
  name=`expr match "$file" '\([a-z_]\+/[a-z_.0-9]\+\)\.s[0-9]'`
  if [ -f "$name.avi" ]; then
    echo "Skipping ${BOLD}$name${NORMAL}"
  else
    echo "Animating ${BOLD}$name${NORMAL}"
    mencoder "mf://${name}*.png" -mf fps=4:type=png -ovc copy -oac copy -o ${name}.avi
  fi
done
