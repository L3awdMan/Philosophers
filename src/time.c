/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 23:53:02 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/19 00:06:22 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//======================== FUNCTION: get_time_ms =======================
//
// PURPOSE:
//    Returns current time in milliseconds as uint64_t.
//    Used for start_time, last_meal_time, and death checks.
//
// RETURN:
//    uint64_t — current time in ms since epoch
//
// VARIABLES:
//    struct timeval tv — seconds + microseconds from gettimeofday
//
// ALGORITHM:
//    1. Call gettimeofday(&tv, NULL)
//    2. Convert: (seconds * 1000) + (microseconds / 1000)
//    3. Return result

//======================== FUNCTION: precise_wait ======================
//
// PURPOSE:
//    Sleeps for exactly `ms` milliseconds while checking sim_stop.
//    Pedro's adaptive 3-granularity sleep — CPU efficient + precise.
//
// RETURN:
//    void
//
// PARAMETERS:
//    uint64_t  ms    — duration to wait in milliseconds
//    t_data   *data  — needed to check sim_stop mid-sleep
//
// VARIABLES:
//    uint64_t start     — timestamp when sleep began
//    uint64_t elapsed   — ms passed since start
//    uint64_t remaining — ms left to sleep
//
// ALGORITHM:
//    1. Record start time
//    2. Loop until elapsed >= ms OR sim is stopped
//    3. Each iteration:
//       - remaining > 10ms → usleep(remaining * 500) [coarse]
//       - remaining > 1ms  → usleep(500)             [fine]
//       - remaining <= 1ms → usleep(100)             [ultra-fine]
//
// EDGE CASES:
//    - sim_stop mid-sleep → exits immediately, no overshoot
//    - ms == 0 → loop never entered
