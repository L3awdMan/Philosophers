*This project has been created as part of the 42 curriculum by zotaj-di.*

---

# Philosophers

> A multithreaded solution to the classic Dining Philosophers Problem, implemented in C with POSIX threads and mutexes.

---

## Description

The **Dining Philosophers Problem** is a classic concurrency challenge first formulated by Edsger Dijkstra in 1965. A number of philosophers sit around a circular table, each alternating between eating, sleeping, and thinking. Between each pair of philosophers lies a single fork, and a philosopher needs **both** adjacent forks to eat. The challenge: design a system where no philosopher starves, no deadlock occurs, and no data race corrupts shared state.

This project solves the problem using:

- **POSIX threads (pthreads):** Each philosopher runs as an independent thread.
- **Mutexes:** Each fork is represented by a mutex, ensuring exclusive access.
- **Dijkstra resource hierarchy:** Deadlock prevention by always locking the lower-indexed fork first, breaking the circular wait condition.
- **Monitor thread:** A dedicated thread continuously checks whether any philosopher has died (exceeded `time_to_die` without eating) or whether all philosophers have reached the optional meal target.
- **Synchronization barrier:** All philosopher threads wait at a barrier before the simulation begins, ensuring a fair and simultaneous start.
- **Staggered startup:** Even-numbered philosophers delay briefly at the start to avoid immediate fork contention.
- **Smart think-time calculation:** Thinking duration is dynamically adjusted to balance eating opportunities across all philosophers, reducing starvation risk with odd philosopher counts.

### How It Works

A philosopher's lifecycle repeats until death or meal completion:

```
 1. Pick up forks (lower index first, then higher index)
 2. Eat for time_to_eat milliseconds
 3. Put down both forks
 4. Sleep for time_to_sleep milliseconds
 5. Think (duration calculated to balance scheduling)
 6. Repeat from step 1
```

The monitor thread runs concurrently:

```
 1. Iterate over all philosophers
 2. Check if (current_time - last_meal_time) > time_to_die  -->  declare death
 3. Check if all philosophers have eaten >= max_meals        -->  end simulation
 4. Repeat with minimal sleep to avoid busy-waiting
```

---

## Workflow Diagram

```
                         MAIN THREAD
  ┌─────────────────────────────────────────────────────────┐
  │                                                         │
  │   Parse args ──> Validate ──> Init data structures      │
  │        │                          │                     │
  │        v                          v                     │
  │   Error? ──> Exit            Allocate philosophers      │
  │                              Init fork mutexes          │
  │                              Init state mutexes         │
  │                                   │                     │
  │                                   v                     │
  │                          Create philosopher threads     │
  │                          Create monitor thread          │
  │                                   │                     │
  │                                   v                     │
  │                        ┌──────────────────────┐         │
  │                        │  SYNC BARRIER        │         │
  │                        │  (all threads wait)  │         │
  │                        └──────────┬───────────┘         │
  │                                   │                     │
  │              ┌────────────────────┼────────────────┐    │
  │              v                    v                v    │
  │        Philosopher 0       Philosopher 1      Philo N   │
  │              │                    │                │    │
  │              v                    v                v    │
  │     ┌────────────────────────────────────────────────┐  │
  │     │           SIMULATION RUNNING                   │  │
  │     └────────────────────┬───────────────────────────┘  │
  │                          │                              │
  │                          v                              │
  │                   Join all threads                      │
  │                   Cleanup & exit                        │
  └─────────────────────────────────────────────────────────┘


              PHILOSOPHER THREAD             MONITOR THREAD
           ┌───────────────────┐          ┌──────────────────┐
           │                   │          │                  │
           │   ┌───────────┐   │          │  For each philo: │
           │   │   EAT     │   │          │    │             │
           │   │ (lock 2   │   │          │    v             │
           │   │  forks,   │   │          │  Dead? ──> STOP  │
           │   │  update   │   │          │    │             │
           │   │  meal)    │   │          │    v             │
           │   └─────┬─────┘   │          │  All fed? ─> STOP│
           │         │         │          │    │             │
           │         v         │          │    v             │
           │   ┌───────────┐   │          │  Sleep briefly   │
           │   │  SLEEP    │   │          │    │             │
           │   └─────┬─────┘   │          │  Loop back       │
           │         │         │          │                  │
           │         v         │          └──────────────────┘
           │   ┌───────────┐   │
           │   │  THINK    │   │
           │   │ (smart    │   │
           │   │  delay)   │   │
           │   └─────┬─────┘   │
           │         │         │
           │         v         │
           │   Sim stopped? ───┼──> Exit thread
           │         │ No      │
           │         v         │
           │   Loop back to EAT│
           └───────────────────┘
```

---

## Instructions

### Prerequisites

- A Unix-like system (Linux / macOS)
- `gcc` or `cc` compiler with C99 support
- `make`
- POSIX threads library (`-lpthread`)

### Compilation

```bash
make          # Standard build with -Wall -Wextra -Werror -pthread
make re       # Clean and rebuild from scratch
make clean    # Remove object files
make fclean   # Remove object files and the binary
```

### Execution

```bash
./philo <num_philosophers> <time_to_die> <time_to_eat> <time_to_sleep> [max_meals]
```

| Argument             | Description                                                        |
|----------------------|--------------------------------------------------------------------|
| `num_philosophers`     | Number of philosophers (and forks) at the table                    |
| `time_to_die` (ms)     | Time in ms a philosopher can survive without eating                |
| `time_to_eat` (ms)     | Time in ms it takes a philosopher to eat (holds 2 forks)           |
| `time_to_sleep` (ms)   | Time in ms a philosopher spends sleeping                           |
| `max_meals`            | *(Optional)* Sim stops when every philosopher has eaten at least     |
|                      | this many times                                                    |

### Examples

```bash
# 5 philosophers, 800ms to die, 200ms eat, 200ms sleep — no one should die
./philo 5 800 200 200

# Same setup but stop after each philosopher eats 7 times
./philo 5 800 200 200 7

# Edge case: 1 philosopher (must die — only one fork available)
./philo 1 800 200 200

# Stress test: 200 philosophers
./philo 200 800 200 200
```
```

---

## Resources

### References

- [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/) -- Lawrence Livermore National Laboratory guide to pthreads, covering thread creation, mutexes, condition variables, and synchronization patterns.
- [Dining Philosophers Problem -- Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem) -- Overview of the problem, its history, and classical solutions including resource hierarchy and arbitrator approaches.
- [pthread_create(3)](https://man7.org/linux/man-pages/man3/pthread_create.3.html), [pthread_mutex_lock(3)](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html) -- Linux man pages for the core threading primitives used in this project.
- [Dijkstra, E. W. (1971). "Hierarchical Ordering of Sequential Processes"](https://www.cs.utexas.edu/users/EWD/ewd03xx/EWD310.PDF) -- The original resource hierarchy solution that inspired the fork-ordering strategy used here.

### AI Usage Disclosure

**Claude** (Anthropic) was used as an assistive tool during development. Specifically:

- **Debugging:** Identifying and resolving concurrency bugs such as race conditions and timing issues.
- **Compilation errors:** Fixing compiler warnings and errors across different environments.
- **Test script:** Generating the test script (`philo_evo_tester.sh`) for automated validation of edge cases and expected behaviors.

**Not assisted by AI:**
- The core algorithm design, including the fork-ordering strategy, monitor logic, staggered startup approach, and smart think-time calculation, was conceived and documented in pseudo-code by me.

---

> *"I think, therefore I eat."* -- A well-fed philosopher, probably.
