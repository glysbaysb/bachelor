#/bin/bash

for i in `seq 0 10 40`
do
	echo $i " Prozent, config veraendern & server neustarten"
	read -n1

	for j in `seq 0 4`
	do
		./build/test_programms/fi > test_results/fi-$i-$j.csv
	done
done
