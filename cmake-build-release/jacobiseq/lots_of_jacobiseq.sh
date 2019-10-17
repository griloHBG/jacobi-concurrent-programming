log_name=$1

N_ARRAY=( 100 500 1000 )

RUNS=30
r=0
total_runs=0

for n in ${N_ARRAY[@]}
do


		r=0
		while [[ $r -lt $RUNS ]]
		do
			r=$((r+1))
			echo -e N=$n'\t'    '\t'RUN=$r'\t'TOTAL_RUNS=$total_runs
			./jacobiseq.exe $n    >> $log_name
			total_runs=$((total_runs+1))
		done

done

echo TOTAL_RUNS = $total_runs
echo
