#!/bin/bash
# 
# This script creates a number of sample populations and samples
# 
####################################################

# Paths to executables is assuming the build is parallel to source directory but
# with source/ replaced with build/
build_dir="../.."

generate="$build_dir/generate"
arc_epi_sample="$build_dir/arc_epi_sample"
circle_gps_sample="$build_dir/circle_gps_sample"
random_sample="$build_dir/random_sample"
square_gps_sample="$build_dir/square_gps_sample"
strip_epi_sample="$build_dir/strip_epi_sample"

# first delete all existing csv, json and png files
rm `find -type f | grep "\.\(csv\|json\|png\)$"`

# ignore file searches that come up empty
shopt -s nullglob

# loop over all conf files
for population_config_file in *.conf
do
  # get the population name by removing the .conf at the end of the population config file
  population_name=${population_config_file:0: -5}

  echo ""
  echo "Processing $population_config_file"
  $generate -c $population_config_file -p $population_name
done

# cheat by making a link to the balanced flat file that the script can work with
ln -sf balanced.household.csv balanced.json.household.csv

# loop over all samplers and process the balanced population
for sample_dir in *_sample
do
  # get the sample type by removing the _method at the end of the sample directory
  sample_type=${sample_dir:0: -7} 

  for sample_config_file in $sample_dir/*.conf
  do
    # get the sample type by removing the .conf at the end of the sample config file
    sample_name=${sample_config_file:0: -5}

    # get the sample iteration by getting the last two characters at the end of the smaple name
    sample_iteration=${sample_name: -2}

    echo ""
    echo "Sampling using $sample_type method, iteration #$sample_iteration"
    ${!sample_dir} -c $sample_config_file -p balanced.json $sample_name
  done
done
