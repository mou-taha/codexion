import subprocess
import sys
from datetime import datetime

# Get current date and time
now = datetime.now()

# Format to a traditional string (e.g., "2026-09-10 18:45:30")
string_timestamp = now.strftime("%Y-%m-%d %H:%M:%S")

# Format to an ISO 8601 standard string (e.g., "2026-09-10T18:45:30.123456")
iso_string = now.isoformat()
# Configuration
COMMAND = ["./codexion", "5", "3000", "200", "200", "200", "10", "800", "fifo"]
NUM_RUNS = 20
TOTAL_CODERS = 5
LOG_FILE = f"tester_execution{string_timestamp}.log"

def log(run_summary, raw_output):
    """
    Appends the summary and raw execution output of a single run to the log file.
    """
    with open(LOG_FILE, "a") as f:
        f.write(f"{run_summary}\n")
        f.write("--- RAW EXECUTION OUTPUT ---\n")
        f.write(raw_output)
        if not raw_output.endswith('\n'):
            f.write("\n")
        f.write("========================================\n\n")


def run_test():
    # Clear the old log file and start fresh
    with open(LOG_FILE, "w") as f:
        f.write("=== CODEXION STRESS TEST LOGS ===\n\n")

    COMMAND = input("Command : ").split()
    NUM_RUNS =  int(input("Number of tests: "))
    success_count = 0
    print(f"Starting {NUM_RUNS} stress tests with {COMMAND[8]} scheduler...\n")

    for i in range(1, NUM_RUNS + 1):
        # Run the simulation and capture the output
        process = subprocess.run(COMMAND, capture_output=True, text=True)
        raw_output = process.stdout
        lines = raw_output.splitlines()

        compiled_coders = set()
        death_line = ""

        # Parse the logs chronologically
        for line in lines:
            if "is compiling" in line:
                parts = line.split()
                if len(parts) >= 2:
                    coder_id = parts[1]
                    compiled_coders.add(coder_id)
            elif "burned out" in line:
                death_line = line
                break

        # Validate the results and generate a summary
        if len(compiled_coders) == TOTAL_CODERS:
            summary = f"✅ Run {i:02d}: PASS - All {TOTAL_CODERS} coders compiled! ({death_line})"
            success_count += 1
        else:
            missing = set(str(c) for c in range(1, TOTAL_CODERS + 1)) - compiled_coders
            summary = f"❌ Run {i:02d}: FAIL - Coders {missing} starved to burn out! ({death_line})"

        # Print to terminal
        print(summary)
        
        # Write to log file via the log() function
        log(summary, raw_output)

    # Final Summary
    print("\n--- TEST SUMMARY ---")
    print(f"Success Rate: {success_count}/{NUM_RUNS} ({(success_count/NUM_RUNS)*100:.1f}%)")
    print(f"Detailed execution logs have been saved to '{LOG_FILE}'.")

if __name__ == "__main__":
    run_test()
