#!/bin/bash
# 
# Parses the simulation and outputs the mean and stdev prevalence results for each population and sample type.
# Run this script after building the multitown simulation.

for summary in `find populations/ -type f | grep "[^/]\+_sample/nr.*\.07\.txt$"`; do
  # print the title
  echo -n $summary | sed -e 's#\(.*\)_sample/nr-\?\([^.]\+\)\?\.[0-9][0-9]\.txt#"\1 \2"#'
  echo ',,,"mx=0;my=0",,,,,,,,,,,,,,,"mx=1;my=1"'
  echo ',,,"unweighted",,,,,,"weighted",,,,,,,,,"unweighted",,,,,,"weighted"'
  echo ',true,,7,,,30,,,7,,,30,,,,true,,7,,,30,,,7,,,30,'
  echo ',mean,,mean,stdev,,mean,stdev,,mean,stdev,,meand,stdev,,,mean,,mean,stdev,,mean,stdev,,mean,stdev,,meand,stdev'

  for resample in "nr" "r"; do
    if [ "nr" == $resample ]; then
      # print the population's true mean
      grep "^child count:" $summary | sed -e "s#.*(prevalence \(.*\))#\1#" | tr '\n' ','
      echo -n "no-resample,"
    else
      echo -n ',,resample,'
    fi

    for prevalence in "prevalence" "weighted prevalence"; do
      for size in "07" "30"; do
        sum=${summary/nr/$resample}
        sum=${sum/07/$size}
        if [ -e $sum ]; then
          grep "^sampled child count:" $sum |
            sed -e "s#.*($prevalence \([^ ]\+\) (\([^)]\+\))).*#\1,\2#" |
           tr '\n' ','
        else
          echo -n 'na,na,'
        fi
        echo -n ','
      done
    done
  done
done
