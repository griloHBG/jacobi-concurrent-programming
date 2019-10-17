N_ARRAY=( 100 500 1000 )
T_ARRAY=( 12 50 100 )
RUNS=30
r=0
total_runs=0

for n in ${N_ARRAY[@]}
do
	for t in ${T_ARRAY[@]}
	do
		r=0
		while [[ $r -lt $RUNS ]]
		do
			r=$((r+1))
			echo -e N=$n'\t'T=$t'\t'RUN=$r'\t'TOTAL_RUNS=$total_runs
			./jacobipar.exe $n $t >> par_windows.txt
			total_runs=$((total_runs+1))
		done
	done
done

echo TOTAL_RUNS = $total_runs
echo
