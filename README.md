*This project has been created as part of the 42 curriculum by tmousnia.*

# Codexion

Codexion is a C/POSIX-threads concurrency simulation in which coders compete for a limited set of USB dongles. Each coder must acquire two dongles to compile, then debug and refactor before trying again. The simulation ends when every coder reaches the required number of compilations or when a coder burns out because they did not start compiling before their deadline.

## Description

The project models resource contention, scheduling, timing, and synchronization in a circular coworking hub:

- There is one dongle between each pair of coders.
- Each coder is represented by a thread and needs two dongles simultaneously to compile.
- Dongles have a configurable cooldown after release.
- Requests are scheduled using FIFO or EDF (Earliest Deadline First).
- A monitor thread checks burnout and completion conditions.
- All state-change messages are serialized and printed with a millisecond timestamp.

The implementation includes a custom binary heap for each dongle request queue. FIFO uses request arrival order; EDF uses the coder's next burnout deadline, with request order as the deterministic tie-breaker.

## Instructions

### Requirements

- A C compiler compatible with `cc`
- POSIX threads (`pthread`)
- `make`

### Compilation

From the repository root:

```sh
make
```

This builds the `codexion` executable with `-Wall -Wextra -Werror -pthread`.

To remove object files or the executable:

```sh
make clean
make fclean
make re
```

### Execution

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

The scheduler must be exactly `fifo` or `edf`. All numeric arguments must be valid non-negative integers; timing values are expressed in milliseconds.

Example:

```sh
./codexion 5 2000 200 200 200 10 0 fifo
```

Each output line has the form:

```text
timestamp_in_ms coder_id message
```

Possible messages include `has taken a dongle`, `is compiling`, `is debugging`, `is refactoring`, and `burned out`.

## Technical choices

- **Language:** C
- **Concurrency model:** one `pthread_t` per coder plus one monitor thread
- **Resource model:** one mutex-protected dongle object per shared dongle
- **Scheduling:** custom min-heap queues implementing FIFO and EDF arbitration
- **Timing:** `gettimeofday()` converted to milliseconds, with short sleeps used during simulation phases
- **Memory management:** explicit initialization and destruction of coders, dongles, mutexes, condition variables, heaps, and allocated arrays

## Blocking cases handled

### Deadlock prevention and Coffman's conditions

A coder requests both dongles in a consistent order based on its identifier: even-numbered coders request right then left, while odd-numbered coders request left then right. This breaks circular wait, one of Coffman's necessary deadlock conditions. Each dongle also has a single protected request queue, so access is granted by the scheduler instead of being taken opportunistically.

### Starvation prevention

Under FIFO, requests are served by arrival order. Under EDF, the request with the earliest burnout deadline is served first, and equal deadlines use request order as a deterministic tie-breaker. This gives waiting coders a defined place in each dongle's queue rather than allowing indefinite bypassing.

### Dongle cooldown

When a dongle is released, `next_availability` is set to the current time plus the configured cooldown. A waiting request cannot claim that dongle before the timestamp has passed. The dongle condition variable is signaled after release so waiting threads can reevaluate their eligibility.

### Precise burnout detection

A dedicated monitor thread checks every coder approximately once per millisecond. It compares the current time with the coder's last compile start, ignores coders currently compiling, and stops the simulation when a coder passes the burnout deadline. The burnout message is printed while the logging mutex is held.

### Log serialization

All status output uses the simulation's print mutex. A complete line is written while that mutex is locked, preventing messages from different threads from being interleaved.

### Completion and shutdown

The simulation has a shared stop flag protected by its own mutex. The monitor sets it when burnout occurs; coder threads check it before starting another cycle, and the main thread joins every coder and the monitor before destroying synchronization objects and freeing memory.

## Thread synchronization mechanisms

### `pthread_mutex_t`

Mutexes protect each shared state boundary:

- Each dongle mutex protects `in_use`, `next_availability`, and its heap queue.
- Each coder mutex protects `last_compile_time`, `nb_compiles`, and `is_compiling`.
- The print mutex prevents concurrent output lines from mixing.
- The stop mutex makes reads and writes of the simulation stop flag race-free.

For example, the monitor locks a coder mutex before reading the last compile time and compiling state, while the coder locks the same mutex when it updates those values.

### `pthread_cond_t`

Every dongle owns a condition variable. A coder waits on it while another request has priority, the dongle is in use, or its cooldown has not expired. Releasing a dongle broadcasts the condition so waiting coders wake up and re-check the heap head under the dongle mutex. This provides thread-safe communication without directly modifying another coder's state.

### Custom event mechanism

The project uses a small event-like simulation-stop mechanism built from the shared `stop_simulation` flag and `stop_simulation_key` mutex. The monitor publishes the stop event by setting the flag; coder threads consume it by checking the flag between phases and before requesting resources. Since all accesses are protected by the same mutex, the monitor and coders communicate without a data race.

Together, the mutexes and condition variables ensure that a coder can only remove its own request when it is the scheduler-selected request, and that monitor observations cannot race with coder state updates.

## Resources

- POSIX Threads documentation: <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html>
- `pthread_cond_wait` documentation: <https://man7.org/linux/man-pages/man3/pthread_cond_wait.3.html>
- `pthread_mutex_lock` documentation: <https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html>
- Allen B. Downey, *The Little Book of Semaphores*: <https://greenteapress.com/wp/semaphores/>
- E. W. Dijkstra, dining philosophers problem: <https://en.wikipedia.org/wiki/Dining_philosophers_problem>
- Binary heap overview: <https://en.wikipedia.org/wiki/Heap_(data_structure)>

### Use of AI

AI assistance was used for documentation support: organizing this README, explaining the project architecture and synchronization strategy in clear English, and checking that the compilation and execution instructions reflect the repository's Makefile and command-line interface. The implementation files remain the project's C source code; this README documents the existing design and behavior.

## Project structure

- `codexion.c`, `codexion.h`: program entry point, shared structures, and declarations
- `parser/`: command-line argument parsing and validation
- `initialization/`: allocation and initialization of simulation objects
- `routines/`: coder lifecycle and monitor logic
- `simulation/`: dongle ownership, timing, and heap scheduling
- `logs/`: serialized status output
