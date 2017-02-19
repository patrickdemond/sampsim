#!/bin/bash
# 
# Parses the simulation and outputs the mean and stdev prevalence results for each population and sample type.
# Run this script after building the multitown simulation.

for summary in *_sample/vanilla.flat.nr*07*.txt; do
  # print the title
  echo $summary | sed -e 's#\(.*\)_sample/.*\.nr-\?\([^-]\+\)\?\.[0-9][0-9]\.txt#"\1 \2"#'

  for income in "flat" "bbb"; do
    sum=${summary/flat/$income}
    for resample in "nr" "r"; do
      sum=${sum/nr/$resample}
      for pop in "vanilla" "popdens"; do
        sum=${sum/vanilla/$pop}

        # print the population's true mean
        if [ "nr" == $resample ]; then
          filename=${pop}.${income}.txt
          if [ -e $filename ]; then
            grep "^child count:" $filename | sed -e "s#.*(prevalence \(.*\))#\1#" | tr '\n' ','
          else
            echo -n 'na,'
          fi
        else
          echo -n ','
        fi
        echo -n ','

        for prevalence in "prevalence" "weighted prevalence"; do
          for size in "07" "30"; do
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
          echo -n ','
        done
      done
      echo
    done
  done
  echo

done
