#!/bin/bash
echo "Building program and tools..."
make all

TEST_CASES=("test_0.meta" "test_1.meta" "test_2.meta")

for test_case in "${TEST_CASES[@]}"; do
    echo "Running data generation tool on $test_case..."
    
    ./tool "testcases/$test_case"
done

for test_case in "${TEST_CASES[@]}"; do
    echo "Running performance tests for $test_case..."
    
    # Run the program
    hyperfine --warmup 3 \
        --min-runs 5 \
        "./program testcases/$test_case" \
        --shell bash
done

echo "Performance tests completed."
