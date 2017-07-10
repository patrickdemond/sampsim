#!/bin/bash
# 
# Parses the simulation and outputs the mean and stdev prevalence results for each population and sample type.
# Run this script after building the multitown simulation.

for summary in $( ls links/ | sort -n | sed -e "s#.*#find links/&/ -type f | grep '[^/]\\\\+_sample/.*07\\\\.txt$'#" | /bin/bash ); do
  population=$( echo $summary | sed -e "s#links/\([0-9]\+\)/.*#\1#" )
  sampler=$( echo $summary |
               sed -e "s#links/[0-9]\+/\(.*\)_sample/\(\(s[0-9]\)\?\).*#\1-\2#" |
               sed -e "s#\(.*\)\-\$#\1#" )

  # print the title
  if [ "circle_gps" = "$sampler" ]; then
    link=$( echo $summary | sed -e 's#/[^/]\+_sample.*##' )
    params=( $( readlink $link | sed -e 's#\.\./populations/##' | sed -e 's#\/v\?# #g' ) )
    params+=( $( ls $link/*.conf | sed -e 's#.*v\(.*\)\.conf#\1#' ) )
    pindex=0

    echo
    echo ,,,\"sample $population\"
    echo ',,,"unweighted",,,,,,"weighted"'
    echo ',true,,7,,,30,,,7,,,30'
    echo ',mean,,mean,stdev,,mean,stdev,,mean,stdev,,mean,stdev'
  fi
  echo -n $sampler,

  # print the population's true mean
  grep "^child count:" $summary | sed -e "s#.*(prevalence \(.*\))#\1#" | tr '\n' ','

  for prevalence in "prevalence" "weighted prevalence"; do
    for size in "07" "30"; do
      sum=${summary/07/$size}
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
  echo -n ${params[$pindex]},
  ((pindex++))
  echo ${params[$pindex]}
  ((pindex++))
done
