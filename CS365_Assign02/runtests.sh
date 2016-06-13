#! /bin/bash

echo "Running parallel version..."

for g in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
	echo "  $g generations..."
	./runpar 'in/testin2.dat' $g 2 2 > out/testin2_${g}_par.out
done
echo "done"

echo "Comparing with sequential output..."

failures=0
for g in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
	echo "$g generations:"
	diff out/testin2_${g}.out out/testin2_${g}_par.out
	if [ $? -ne 0 ]; then
		failures=$(expr $failures + 1)
	fi
done

if [ $failures -eq 0 ]; then
	echo "No failures...parallel output looks good!"
else
	echo "$g/20 test outputs didn't match the expected output"
fi
