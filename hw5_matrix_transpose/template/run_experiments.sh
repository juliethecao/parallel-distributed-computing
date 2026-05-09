#!/bin/bash

# Create results directory
mkdir -p results

# Experiment 1: Single threaded execution time vs matrix size
# Sizes: 100, 500, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000
echo "Running Experiment 1: Sequential execution time vs matrix size..."
echo "n,time" > results/exp1_sequential.csv
for n in 100 500 1000 2000 3000 4000 5000 6000 7000 8000 9000 10000; do
    time_result=$(./hw5.bin $n s 0 2>/dev/null)
    echo "$n,$time_result" >> results/exp1_sequential.csv
    echo "  n=$n: $time_result seconds"
done

# Experiment 2: Fine-grain (grain=1) with varying threads, n=7000
echo "Running Experiment 2: Fine-grain (grain=1) with varying threads, n=7000..."
echo "threads,time" > results/exp2_finegrain.csv
for threads in 1 2 3 4 5 6 8 16 24 32 64 128; do
    time_result=$(./hw5.bin 7000 p 1 $threads 0 2>/dev/null)
    echo "$threads,$time_result" >> results/exp2_finegrain.csv
    echo "  threads=$threads: $time_result seconds"
done

# Experiment 3: Coarse-grain with grain=n and grain=(n*n)/threads, n=7000
echo "Running Experiment 3: Coarse-grain configurations, n=7000..."
echo "threads,time_grain_n,time_grain_n2_div_t" > results/exp3_coarsegrain.csv
n=7000
grain_n=$n
for threads in 1 2 3 4 5 6 8 16 24 32 64 128; do
    grain_n2_div_t=$(( (n * n) / threads ))
    time_grain_n=$(./hw5.bin $n p $grain_n $threads 0 2>/dev/null)
    time_grain_n2_div_t=$(./hw5.bin $n p $grain_n2_div_t $threads 0 2>/dev/null)
    echo "$threads,$time_grain_n,$time_grain_n2_div_t" >> results/exp3_coarsegrain.csv
    echo "  threads=$threads, grain=n: $time_grain_n, grain=(n*n)/t: $time_grain_n2_div_t"
done

# Experiment 4: Speedup comparison (fine vs coarse vs sequential)
echo "Running Experiment 4: Speedup analysis, n=7000..."
echo "threads,time_seq,time_finegrain,time_coarsegrain,speedup_fine,speedup_coarse" > results/exp4_speedup.csv
n=7000
time_seq=$(./hw5.bin $n s 0 2>/dev/null)
echo "Baseline sequential time: $time_seq"
for threads in 1 2 3 4 6 8 16 32 64 128; do
    time_fine=$(./hw5.bin $n p 1 $threads 0 2>/dev/null)
    time_coarse=$(./hw5.bin $n p $n $threads 0 2>/dev/null)
    speedup_fine=$(echo "scale=4; $time_seq / $time_fine" | bc)
    speedup_coarse=$(echo "scale=4; $time_seq / $time_coarse" | bc)
    echo "$threads,$time_seq,$time_fine,$time_coarse,$speedup_fine,$speedup_coarse" >> results/exp4_speedup.csv
    echo "  threads=$threads: fine_speedup=$speedup_fine, coarse_speedup=$speedup_coarse"
done

# Experiment 5: Speedup for various sizes with best coarse-grain
echo "Running Experiment 5: Speedup vs matrix size..."
echo "n,time_seq,time_coarse,speedup" > results/exp5_size_speedup.csv
for n in 10 50 100 500 1000 2000 3000 4000 5000 6000 7000 8000 9000 10000; do
    time_seq=$(./hw5.bin $n s 0 2>/dev/null)
    # Use coarse-grain with reasonable thread count (4 for most systems)
    time_coarse=$(./hw5.bin $n p $n 4 0 2>/dev/null)
    speedup=$(echo "scale=4; $time_seq / $time_coarse" | bc)
    echo "$n,$time_seq,$time_coarse,$speedup" >> results/exp5_size_speedup.csv
    echo "  n=$n: seq=$time_seq, coarse=$time_coarse, speedup=$speedup"
done

echo "All experiments completed! Results saved in results/ directory"
