#!/bin/bash
# 
# Parses the simulation and outputs the mean and stdev prevalence results for each population and sample type.
# Run this script after building the multitown simulation.

if [ $# -ne 2 ]; then
  echo "Needs two input parameters:"
  echo "  1) the RR value (1, 1.5, 2, 3, etc)"
  echo "  2) whether to collect data from non-town-resampling (n) or town-resampling (r)"
fi

RR=$1
TR=$2
first=1

for summary in $( ls links/ | sort -n | sed -e "s#.*#find links/&/ -type f | grep '[^/]\\\\+_sample/${TR}.*07\\\\.csv$' | sort#" | /bin/bash ); do
  population=$( echo $summary | sed -e "s#links/\([0-9]\+\)/.*#\1#" )
  population=$(( $population + 1 ))
  sampler=$( echo $summary |
               sed -e "s#links/[0-9]\+/\(.*\)_sample/${TR}\(\(s[0-9]\)\?\).*#\1-\2#" |
               sed -e "s#\(.*\)\-\$#\1#" )

  # print the title
  if [ "random" = "$sampler" ]; then
    if [ $first -eq 1 ]; then first=0; else echo; fi
    echo ",,,population $population"
    echo ',true,true,7,,,,,,,30'
    echo ',mean,rr,mean,stdev,mse,mrr,srr,prr,mse_rr,mean,stdev,mse,mrr,srr,prr,mse_rr'
  fi
  echo -n $sampler,

  # print the population's true mean
  truerr=$( grep "^population,child,$RR," $summary | cut -d ',' -f6 )
  truemean=$( grep "^population,child,$RR," $summary | cut -d ',' -f7 )
  echo -n $truemean,$truerr,

  for size in "07" "30"; do
    sum=${summary/07/$size}
    if [ -e $sum ]; then
      mean=$( grep "^sample,child,$RR," $sum | cut -d ',' -f14 )
      if [ -z $mean ]; then
        echo -n 'na,na,na,na,na,na,na,'
      else
        stdev=$( grep "^sample,child,$RR," $sum | cut -d ',' -f15 )
        mse=$( echo "scale=10; ($truemean-$mean)^2 + $stdev^2" | bc )
        prr=$( grep "^sample,child,$RR," $sum | cut -d ',' -f13 )
        srr=$( grep "^sample,child,$RR," $sum | cut -d ',' -f12 )
        mse_rr=$( echo "scale=10; ($truerr-$prr)^2 + $srr^2" | bc )
        grep "^sample,child,$RR," $sum | cut -d ',' -f '14 15' | tr '\n' ','
        echo -n $mse,
        grep "^sample,child,$RR," $sum | cut -d ',' -f '13 11 12' | tr '\n' ','
        echo -n $mse_rr,
      fi
    else
      echo -n 'na,na,na,na,na,na,na,'
    fi
  done
  echo
done
