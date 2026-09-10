import subprocess
import sys

# Configuration
COMMAND = ["./codexion", "5", "3000", "200", "200", "200", "10", "800", "edf"]
NUM_RUNS = 20
TOTAL_CODERS = 5

def run_test():
    success_count = 0

    print(f"Starting {NUM_RUNS} stress tests with EDF scheduler...\n")

    for i in range(NUM_RUNS):
        # Run the simulation and capture the output
        process = subprocess.run(COMMAND, capture_output=True, text=True)
        output = process.stdout.splitlines()

        compiled_coders = set()
        death_line = ""

        # Parse the logs chronologically
        for line in output:
            if "is compiling" in line:
                # Extract the coder ID (assuming format: "timestamp coder_id is compiling")
                parts = line.split()
                if len(parts) >= 2:
                    coder_id = parts[1]
                    compiled_coders.add(coder_id)
            elif "burned out" in line:
                death_line = line
                break # Stop tracking once someone dies

        # Validate the results
        if len(compiled_coders) == TOTAL_CODERS:
            print(f"✅ Run {i+1:02d}: PASS - All {TOTAL_CODERS} coders compiled! ({death_line})")
            success_count += 1
        else:
            missing = set(str(c) for c in range(1, TOTAL_CODERS + 1)) - compiled_coders
            print(f"❌ Run {i+1:02d}: FAIL - Coders {missing} starved to death! ({death_line})")

    # Final Summary
    print("\n--- TEST SUMMARY ---")
    print(f"Success Rate: {success_count}/{NUM_RUNS} ({(success_count/NUM_RUNS)*100}%)")

if __name__ == "__main__":
    run_test()