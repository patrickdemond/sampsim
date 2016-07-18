#!/bin/bash
# 
# Parses the simulation and outputs the mean and stdev prevalence results for each population and sample type.
# Run this script after building the multitown simulation.

find -type d | grep sample | sed -e "s#\./\(.*\)_sample#\1#" | while read sample; do
  echo $sample
  echo "mean,mean,stdev,mean,stdev,mean,stdev,,mean,mean,stdev,mean,stdev,mean,stdev"
  grep "^child" vanilla.flat.txt | sed -e "s#.*(prevalence \(.*\))#\1#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/vanilla.flat.7.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/vanilla.flat.10.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/vanilla.flat.30.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  echo -n "," &&
  grep "^child" popdens.flat.txt | sed -e "s#.*(prevalence \(.*\))#\1#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/popdens.flat.7.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/popdens.flat.10.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/popdens.flat.30.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" &&

  grep "^child" vanilla.b00.txt | sed -e "s#.*(prevalence \(.*\))#\1#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/vanilla.b00.7.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/vanilla.b00.10.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/vanilla.b00.30.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  echo -n "," &&
  grep "^child" popdens.b00.txt | sed -e "s#.*(prevalence \(.*\))#\1#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/popdens.b00.7.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/popdens.b00.10.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/popdens.b00.30.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" &&

  grep "^child" vanilla.b00b01b10.txt | sed -e "s#.*(prevalence \(.*\))#\1#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/vanilla.b00b01b10.7.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/vanilla.b00b01b10.10.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/vanilla.b00b01b10.30.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  echo -n "," &&
  grep "^child" popdens.b00b01b10.txt | sed -e "s#.*(prevalence \(.*\))#\1#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/popdens.b00b01b10.7.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/popdens.b00b01b10.10.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" | tr '\n' ',' &&
  grep "^sampled child" ${sample}_sample/popdens.b00b01b10.30.txt | sed -e "s#.*(prevalence \(.*\) (\(.*\)))#\1,\2#" &&
  echo "";
done
