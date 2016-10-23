#!/bin/bash
# 
# Parses the simulation and outputs basic details about each town in each population.
# Run this script after building the multitown simulation.

echo "mx=0;my=0"
echo "town,pop,child,income,disease,income,disease"

for i in {0..249}; do
  echo -n "$(($i+1))," &&
  grep "^$i,[0-9]\+," vanilla.flat.individual.csv | wc -l | tr '\n' ',' &&
  grep "^$i,[0-9]\+,child" vanilla.flat.individual.csv | wc -l | tr '\n' ',' &&
  grep "^$i,[0-9]\+," vanilla.flat.household.csv | sed -e "s/^\([0-9]\+\),.*,\([0-9.]\+\),0.000,[01]$/\2/" | awk '{s+=$1}END{print s/NR}' RS="\n" | tr '\n' ',' &&
  grep "^$i,[0-9]\+,child,\(fe\)\?male,1" vanilla.flat.individual.csv | wc -l | tr '\n' ',' &&
  grep "^$i,[0-9]\+," vanilla.bbb.household.csv | sed -e "s/^\([0-9]\+\),.*,\([0-9.]\+\),0.000,[01]$/\2/" | awk '{s+=$1}END{print s/NR}' RS="\n" | tr '\n' ',' &&
  grep "^$i,[0-9]\+,child,\(fe\)\?male,1" vanilla.bbb.individual.csv | wc -l; 
done

echo -n "overall," &&
grep "^[0-9]\+,[0-9]\+," vanilla.flat.individual.csv | wc -l | tr '\n' ',' &&
grep "^[0-9]\+,[0-9]\+,child" vanilla.flat.individual.csv | wc -l | tr '\n' ',' &&
grep "^[0-9]\+,[0-9]\+," vanilla.flat.household.csv | sed -e "s/^\([0-9]\+\),.*,\([0-9.]\+\),0.000,[01]$/\2/" | awk '{s+=$1}END{print s/NR}' RS="\n" | tr '\n' ',' &&
grep "^[0-9]\+,[0-9]\+,child,\(fe\)\?male,1" vanilla.flat.individual.csv | wc -l | tr '\n' ',' &&
grep "^[0-9]\+,[0-9]\+," vanilla.bbb.household.csv | sed -e "s/^\([0-9]\+\),.*,\([0-9.]\+\),0.000,[01]$/\2/" | awk '{s+=$1}END{print s/NR}' RS="\n" | tr '\n' ',' &&
grep "^[0-9]\+,[0-9]\+,child,\(fe\)\?male,1" vanilla.bbb.individual.csv | wc -l; 

echo ""
echo "mx=1;my=1"
echo "town,pop,child,income,disease,income,disease"

for i in {0..249}; do
  echo -n "$(($i+1))," &&
  grep "^$i,[0-9]\+," popdens.flat.individual.csv | wc -l | tr '\n' ',' &&
  grep "^$i,[0-9]\+,child" popdens.flat.individual.csv | wc -l | tr '\n' ',' &&
  grep "^$i,[0-9]\+," popdens.flat.household.csv | sed -e "s/^\([0-9]\+\),.*,\([0-9.]\+\),0.000,[01]$/\2/" | awk '{s+=$1}END{print s/NR}' RS="\n" | tr '\n' ',' &&
  grep "^$i,[0-9]\+,child,\(fe\)\?male,1" popdens.flat.individual.csv | wc -l | tr '\n' ',' &&
  grep "^$i,[0-9]\+," popdens.bbb.household.csv | sed -e "s/^\([0-9]\+\),.*,\([0-9.]\+\),0.000,[01]$/\2/" | awk '{s+=$1}END{print s/NR}' RS="\n" | tr '\n' ',' &&
  grep "^$i,[0-9]\+,child,\(fe\)\?male,1" popdens.bbb.individual.csv | wc -l; 
done

echo -n "overall," &&
grep "^[0-9]\+,[0-9]\+," popdens.flat.individual.csv | wc -l | tr '\n' ',' &&
grep "^[0-9]\+,[0-9]\+,child" popdens.flat.individual.csv | wc -l | tr '\n' ',' &&
grep "^[0-9]\+,[0-9]\+," popdens.flat.household.csv | sed -e "s/^\([0-9]\+\),.*,\([0-9.]\+\),0.000,[01]$/\2/" | awk '{s+=$1}END{print s/NR}' RS="\n" | tr '\n' ',' &&
grep "^[0-9]\+,[0-9]\+,child,\(fe\)\?male,1" popdens.flat.individual.csv | wc -l | tr '\n' ',' &&
grep "^[0-9]\+,[0-9]\+," popdens.bbb.household.csv | sed -e "s/^\([0-9]\+\),.*,\([0-9.]\+\),0.000,[01]$/\2/" | awk '{s+=$1}END{print s/NR}' RS="\n" | tr '\n' ',' &&
grep "^[0-9]\+,[0-9]\+,child,\(fe\)\?male,1" popdens.bbb.individual.csv | wc -l; 
