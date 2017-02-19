#!/bin/bash

function histogram {
  echo $1 | sed -e "s#\(.*\)/\(.*\)\.flat\.\([^-]\+\)-\?\(.*\)\.\([0-9]\+\)#\1 \2 \3 \4 \5#"
  echo -n "0,"
  echo "`grep "^child count" $1.txt |
         sed -e "s/child count: [0-9]\+ diseased of \([0-9]\+\) total.*/\1/"` - \
        `cat $1.s*.individual.csv |
         grep "^[0-9]\+,[0-9]\+,[0-9]\+," |
         sed -e "s#^[0-9]\+,[0-9]\+,\([0-9]\+\),.*#\1#" |
         sort -g | uniq | wc -l`" | bc

  index=1
  lines=`cat $1.s*.individual.csv |
      grep "^[0-9]\+,[0-9]\+,[0-9]\+," | # only include data
      sed -e "s#^[0-9]\+,[0-9]\+,\([0-9]\+\),.*#\1#" | # only show individual index
      sort -g | # sort individual index numerically
      uniq -c | # count the number of times an index shows
      sed -e "s# \+\([0-9]\+\) .*#\1#" | # remove the individual index
      sort -g | # sort the counts numerically
      uniq -c | # count the number of times a count shows
      sed -e "s# \+\([0-9]\+\) \([0-9]\+\)#\2,\1#" # show "times,count"
  `
  for line in $lines; do
    # print value of 0 for any missing index
    current=`echo $line | sed -e "s/\([^,]\),.*/\1/"`
    while [ $current -ne $index ]; do
      echo $index,0
      ((index++))
    done
    echo $line
    ((index++))
  done
  echo
}

for summary in */*flat.*.txt; do
  name=`echo $summary | sed -e 's#\.txt$##'`
  histogram $name
done
