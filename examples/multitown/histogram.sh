#!/bin/bash

function histogram {
  cat $1.s*.individual.csv |
    grep "^[0-9]\+,[0-9]\+,[0-9]\+," | # only include data
    sed -e "s#^[0-9]\+,[0-9]\+,\([0-9]\+\),.*#\1#" | # only show individual index
    sort -g | # sort individual index numerically
    uniq -c | # count the number of times an index shows
    sed -e "s# \+\([0-9]\+\) .*#\1#" | # remove the individual index
    sort -g | # sort the counts numerically
    uniq -c | # count the number of times a count shows
    sed -e "s# \+\([0-9]\+\) \([0-9]\+\)#\2,\1#" # show "times,count"
}

for sampler in *_sample; do
  for pop in vanilla popdens; do
    for type in flat bbb; do
      for size in 7 10 30; do
        echo $sampler $pop $type $size
        histogram $sampler/$pop.$type.$size
        echo
      done
    done
  done
done
