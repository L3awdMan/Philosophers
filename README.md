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

## Resources

### References

- [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/) -- Lawrence Livermore National Laboratory guide to pthreads, covering thread creation, mutexes, condition variables, and synchronization patterns.
- [Dining Philosophers Problem -- Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem) -- Overview of the problem, its history, and classical solutions including resource hierarchy and arbitrator approaches.
- [pthread_create(3)](https://man7.org/linux/man-pages/man3/pthread_create.3.html), [pthread_mutex_lock(3)](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html) -- Linux man pages for the core threading primitives used in this project.
- [Dijkstra, E. W. (1971). "Hierarchical Ordering of Sequential Processes"](https://www.cs.utexas.edu/users/EWD/ewd03xx/EWD310.PDF) -- The original resource hierarchy solution that inspired the fork-ordering strategy used here.

---

> *"I think, therefore I eat."* -- A well-fed philosopher, probably.
