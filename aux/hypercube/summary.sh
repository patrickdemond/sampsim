#!/bin/bash
# 
# Parses the simulation and outputs the mean and stdev prevalence results for each population and sample type.
# Run this script after building the multitown simulation.

# Paths to executables is assuming the build is parallel to source directory but
# with source/ replaced with build/
build_dir="../../.."
population=$( which $build_dir/population )

stop=false

for sample_file in $( ls links/ | sort -n | sed -e "s#.*#find links/&/ -type f | grep '[^/]\\\\+_sample/.*\\\\.json.tar.gz$' | sort#" | /bin/bash ); do
  summary_file=`echo $sample_file | sed -e "s#\.json\.tar\.gz#.txt#"`
  if [ ! -e $summary_file ]; then
    type=`echo $sample_file | sed -e "s#links/[0-9]\+/\([a-z_]\+\)_sample/.*#\1#"`
    echo "Adding job: building missing summary file for $sample_file"
    batch_file=${index}.sh
    printf "#!/bin/bash\n#SBATCH --time=00:10:00\n#SBATCH --mem=32G\n $population -q -s --type $type $sample_file\n" > $batch_file
    sbatch $batch_file
    stop=true
  fi
done

if [ "$stop" = true ]; then
  exit
fi

for summary in $( ls links/ | sort -n | sed -e "s#.*#find links/&/ -type f | grep '[^/]\\\\+_sample/.*07\\\\.txt$' | sort#" | /bin/bash ); do
  population=$( echo $summary | sed -e "s#links/\([0-9]\+\)/.*#\1#" )
  sampler=$( echo $summary |
               sed -e "s#links/[0-9]\+/\(.*\)_sample/\(\(s[0-9]\)\?\).*#\1-\2#" |
               sed -e "s#\(.*\)\-\$#\1#" )

  # print the title
  if [ "arc_epi-s1" = "$sampler" ]; then
    link=$( echo $summary | sed -e 's#/[^/]\+_sample.*##' )
    params=( $( readlink $link | sed -e 's#\.\./populations/##' | sed -e 's#\/v\?# #g' | sed -e 's#,#;#g' ) )
    params+=( $( ls $link/*.conf | sed -e 's#.*v\(.*\)\.conf#\1#' ) )
    pindex=0

    echo
    echo ,,\"sample $population\"
    echo ',,"unweighted",,,,,,"weighted"'
    echo ',true,7,,,30,,,7,,,30'
    echo ',mean,mean,stdev,,mean,stdev,,mean,stdev,,mean,stdev'
  fi
  echo -n $sampler,

  # print the population's true mean
  grep "^population,child," $summary | cut -d ',' -f6 | tr '\n' ','

  # the 6th and 7th args are mean and stdev for unweighted, the 8th and 9th args are mean and stdev for weighted
  for cut in "6 7" "8 9"; do
    for size in "07" "30"; do
      sum=${summary/07/$size}
      if [ -e $sum ]; then
        grep "^sample,child," $sum | cut -d ',' -f $cut | tr '\n' ','
      else
        echo -n 'na,na,'
      fi
      echo -n ','
    done
  done
  echo ${params[$pindex]},${params[$pindex+1]},${params[$pindex+14]},${params[$pindex+15]}
  ((pindex+=2))
done
