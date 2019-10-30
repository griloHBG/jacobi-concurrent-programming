N_ARRAY=( 100 200 300 400 500 1000 2000 3000 4000 5000)
T_ARRAY=( 10 20 30 40 50 100 200 300 400 500 1000 2000 3000 4000 5000 )
RUNS=10
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
			./jacobipar.exe $n $t >> par_windows_HUGE.txt
			total_runs=$((total_runs+1))
		done
	done
done

echo TOTAL_RUNS = $total_runs
echo
