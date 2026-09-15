from collections import defaultdict


def analyze_logs(required_compiles, nb_coders, logs):
    compile_count = defaultdict(int)

    # Analyze every log line
    for line in logs:
        parts = line.strip().split()

        # We expect:
        # timestamp coder is compiling
        #
        # Example:
        # 1201 4 is compiling

        if len(parts) >= 4 and parts[2] == "is" and parts[3] == "compiling":
            coder = int(parts[1])

            # Only count valid coder numbers
            if 1 <= coder <= nb_coders:
                compile_count[coder] += 1

    print("\n========== COMPILATION REPORT ==========")

    all_valid = True

    for coder in range(1, nb_coders + 1):
        count = compile_count[coder]

        if count >= required_compiles:
            status = "VALID"
            message = (
                f"Coder {coder}: {count} compilation(s) "
                f"-> VALID"
            )
        else:
            missing = required_compiles - count
            status = "NOT VALID"
            message = (
                f"Coder {coder}: {count} compilation(s) "
                f"-> NOT VALID "
                f"({missing} more compilation(s) required)"
            )
            all_valid = False

        print(message)

    print("========================================")

    if all_valid:
        print("SUCCESS: All coders have completed the required number of compiles.")
    else:
        print("WARNING: Some coders have not completed the required number of compiles.")

    return all_valid


def main():
    print("=== Coder Compilation Log Analyzer ===")

    # Input 1
    required_compiles = int(
        input("Number of compiles required per coder: ")
    )

    # Number of coders
    nb_coders = int(
        input("Number of coders: ")
    )

    print("\nPaste the logs below.")
    print("Enter an empty line when you are finished:\n")

    logs = []

    while True:
        line = input()

        if line.strip() == "":
            break

        logs.append(line)

    analyze_logs(
        required_compiles,
        nb_coders,
        logs
    )


if __name__ == "__main__":
    main()
