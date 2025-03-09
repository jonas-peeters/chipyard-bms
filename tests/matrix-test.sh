VARIANTS="0 1 2 3"
PREDICTABLES="0 1"

PROGRAM="quicksort"

OUTPUT_FILE="/home/peeters/master-thesis/chipyard/sims/verilator/output/chipyard.harness.TestHarness.CustomBoomConfig/$PROGRAM.cyclecount"

for VARIANT in $VARIANTS; do
    for PREDICTABLE in $PREDICTABLES; do
        export VARIANT
        export PREDICTABLE
        echo "Running test with variant $VARIANT and predictable $PREDICTABLE"
        cd /home/peeters/master-thesis/chipyard/tests
        make clean
        make dumps
        cd /home/peeters/master-thesis/chipyard/sims/verilator
        make run-binary CONFIG=CustomBoomConfig BINARY=~/master-thesis/chipyard/tests/$PROGRAM.riscv
        # Extract cycle count from output *** PASSED *** Completed after               177176 simulation cycles
        tail -n 1 /home/peeters/master-thesis/chipyard/sims/verilator/output/chipyard.harness.TestHarness.CustomBoomConfig/$PROGRAM.out | grep -o '[0-9]*' >> $OUTPUT_FILE
        truncate -s-1 $OUTPUT_FILE
        echo -n ";" >> $OUTPUT_FILE
    done
    truncate -s-1 $OUTPUT_FILE
    echo "" >> $OUTPUT_FILE
done