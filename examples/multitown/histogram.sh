#!/bin/bash

function histogram {
  echo -n "0,"
  echo "`grep "child count" $2.txt |
         sed -e "s/child count: [0-9]\+ diseased of \([0-9]\+\) total.*/\1/"` - \
        `cat $1/$2.$3.s*.individual.csv |
         grep "^[0-9]\+,[0-9]\+,[0-9]\+," |
         sed -e "s#^[0-9]\+,[0-9]\+,\([0-9]\+\),.*#\1#" |
         sort -g | uniq | wc -l`" | bc

  index=1
  for line in `
    cat $1/$2.$3.s*.individual.csv |
      grep "^[0-9]\+,[0-9]\+,[0-9]\+," | # only include data
      sed -e "s#^[0-9]\+,[0-9]\+,\([0-9]\+\),.*#\1#" | # only show individual index
      sort -g | # sort individual index numerically
      uniq -c | # count the number of times an index shows
      sed -e "s# \+\([0-9]\+\) .*#\1#" | # remove the individual index
      sort -g | # sort the counts numerically
      uniq -c | # count the number of times a count shows
      sed -e "s# \+\([0-9]\+\) \([0-9]\+\)#\2,\1#" # show "times,count"
  `; do
    # print value of 0 for any missing index
    current=`echo $line | sed -e "s/\([^,]\),.*/\1/"`
    while [ $current -ne $index ]; do
      echo $index,0
      ((index++))
    done
    echo $line
    ((index++))
  done
}

for sampler in *_sample; do
  for pop in vanilla popdens; do
    for size in 7 30; do
      echo $sampler $pop $size
      histogram $sampler $pop.flat $size
      echo
    done
  done
done
