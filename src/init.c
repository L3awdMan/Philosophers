/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 14:58:41 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/19 15:37:30 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "philo.h"

//======================== FUNCTION: set_forks =========================
//
// PURPOSE:
//    Pre-computes which fork each philo grabs first and second.
//    Dijkstra resource hierarchy — lower index fork always first.
//    Called once at init — zero branching at runtime.
//
// RETURN:
//    void
//
// PARAMETERS:
//    t_philo *philo — philosopher to assign forks to
//    t_data  *data  — needed for forks array + philo_count
//
// VARIABLES:
//    int left  — index of left fork  = philo->id - 1
//    int right — index of right fork = philo->id % philo_count
//
// ALGORITHM:
//    1. Compute left and right fork indices
//    2. If left < right → first=left, second=right
//    3. Else           → first=right, second=left
//    4. Assign pointers into data->forks array
//
// EDGE CASES:
//    - Last philo: right = 0 (wraps around to fork 0)
//    - Even/odd split naturally emerges from index comparison

void	set_forks(t_philo *philo, t_data *data)

//======================== FUNCTION: init_mutexes ======================
//
// PURPOSE:
//    Initializes ALL mutexes: forks + meal_mutex per philo
//    + print_mutex + state_mutex.
//    One loop handles both forks[i] and philos[i].meal_mutex.
//
// RETURN:
//    int
//        1 → all mutexes initialized
//        0 → any pthread_mutex_init failed
//
// PARAMETERS:
//    t_data *data — shared simulation state
//
// ALGORITHM:
//    1. Loop i from 0 to philo_count:
//       - Init forks[i]
//       - Init philos[i].meal_mutex
//       - Return 0 on any failure
//    2. Init print_mutex
//    3. Init state_mutex
//    4. Return 1

int	init_mutexes(t_data *data)

//======================== FUNCTION: destroy_mutexes ===================
//
// PURPOSE:
//    Destroys ALL mutexes in reverse init order.
//    Called by cleanup before program exits.
//
// RETURN:
//    void
//
// PARAMETERS:
//    t_data *data — shared simulation state
//
// ALGORITHM:
//    1. Loop i: destroy forks[i] + philos[i].meal_mutex
//    2. Destroy print_mutex
//    3. Destroy state_mutex

void	destroy_mutexes(t_data *data)

//======================== FUNCTION: init_philos =======================
//
// PURPOSE:
//    Initializes all t_philo structs.
//    Sets id, zeroes meal data, assigns back-pointer, pre-computes forks.
//
// RETURN:
//    int — always 1 (reserved for future malloc-based versions)
//
// PARAMETERS:
//    t_data *data — contains philos array + all shared state
//
// ALGORITHM:
//    1. Loop i from 0 to philo_count:
//       - id          = i + 1  (1-indexed)
//       - last_meal_time = 0   (set to start_time when barrier fires)
//       - meal_count  = 0
//       - data        = data   (back-pointer)
//       - set_forks()          (pre-compute fork order)
//    2. Return 1

int	init_philos(t_data *data)

//======================== FUNCTION: init_data =========================
//
// PURPOSE:
//    Top-level init entry point.
//    Zeros barrier vars explicitly, then inits mutexes and philos.
//
// RETURN:
//    int
//        1 → fully initialized
//        0 → any step failed
//
// PARAMETERS:
//    t_data *data — already memset to 0 in main
//
// ALGORITHM:
//    1. Explicit zero: start_time, sim_stop, threads_ready, all_started
//    2. init_mutexes → forks + meal mutexes + shared mutexes
//    3. init_philos  → philo structs + fork pointers
//    4. Return 1
//
// EDGE CASES:
//    - data already memset in main — explicit zeros are safety redundancy
//    - failure before any thread exists — no join needed on error

int	init_data(t_data *data)
