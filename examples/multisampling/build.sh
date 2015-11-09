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

# loop over all weight types
for weight_type in none population income risk age sex pocket; do
  echo ""
  echo "Processing $population_config_file"
  $generate -s -p -c vanilla.conf $weight_type \
    --dweight_population `if [[ "population" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
    --dweight_income `if [[ "income" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
    --dweight_risk `if [[ "risk" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
    --dweight_age `if [[ "age" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
    --dweight_sex `if [[ "sex" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi` \
    --dweight_pocket `if [[ "pocket" = "$weight_type" ]]; then echo 1.0; else echo 0.0; fi`

  # loop over all samplers and process the population
#  for sample_dir in *_sample
#  do
#    # get the sample type by removing the _method at the end of the sample directory
#    sample_type=${sample_dir:0: -7}
#
#    for sample_config_file in $sample_dir/*.conf
#    do
#      # get the sample type by removing the .conf at the end of the sample config file
#      slash=`expr index "$sample_config_file" /`
#      sample_name=${sample_config_file:0:$slash}$population_name.${sample_config_file:$slash}
#
#      echo ""
#      echo "Sampling $population_name using the $sample_type sampler (configuration ${sample_config_file})"
#      ${!sample_dir} -s -c $sample_config_file $population_name.json $sample_name
#    done
#  done
done
