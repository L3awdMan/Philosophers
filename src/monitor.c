/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 00:05:33 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/25 00:06:13 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//======================== FUNCTION: get_meal_data ==========================
//
// PURPOSE:
//   Thread-safely get/bring the last meal time and meal count of a philosopher
//
// RETURN:
//    void
//        - Modifies values through pointers.
//
// PARAMETERS:
//    t_philo *philo - Philosopher to check
//    uint64_t *lm - Pointer to store the last meal time
//    int *mc - Pointer to store the meal count
//
// ALGORITHM:
//    1. Lock `philo->meal_mutex`
//    2. Assign `*lm` to `philo->last_meal_time`
//    3. Assign `*mc` to `philo->meal_count`
//    4. Unlock `philo->meal_mutex`
//
// EDGE CASES:
//    - Must be called whenever reading these variables to prevent data races.
//
// EXAMPLE:
//    get_meal_data(p, &lm, &mc) → lm = 500, mc = 2

void	get_meal_data(t_philo *philo, uint64_t *lm, int *mc)
{
	t_philo		*philo;
	uint64_t	*lm;
	int			*mc;
}

//======================== FUNCTION: stop_sim_death ==========================
//
// PURPOSE:
//    Triggers the global stop flag and prints the death message.
//
// RETURN:
//    void
//        - No return value
//
// PARAMETERS:
//    t_data *data
//        - Main data structure
//    int id
//        - ID of the philosopher who died
//
// VARIABLES:
//    None
//
// ALGORITHM:
//    1. Call `set_sim_stop(data)`
//    2. Lock `data->print_mutex`
//    3. Print the death message using `printf` with `elapsed(data)` and `id`
//    4. Unlock `data->print_mutex`
//
// EDGE CASES:
// - Printing inside a separate mutex ensures the death message doesn't overlap.
//
// EXAMPLE:
//    stop_sim_death(data, 3) → Sets flag and prints "801 3 died"
void	stop_sim_death(t_data *data, int id)
{
}

//======================= FUNCTION: check_philo_death ==========================
//
// PURPOSE:
//    Checks if a specific philosopher has starved
//    (time since last meal > time_to_die).
//
// RETURN:
//    int
//        - 1  → Philosopher died
//        - 0  → Philosopher is still alive (or hasn't started eating yet)
//
// PARAMETERS:
//    t_data *data
//        - Main structure
//    int i
//        - Index of the philosopher in the array
//
// VARIABLES:
//    uint64_t last_meal
//        - Local variable for last meal time
//    int mc
//        - Local variable for meal count
//
// ALGORITHM:
//    1. Call `get_meal_data(&data->philos[i], &last_meal, &mc)`
//    2. If `last_meal` is 0,
//	return 0 (means thread hasn't properly initialized yet)
//    3. Check if `time_since(last_meal)` is greater than `(uint64_t)data->time_to_die`
//       3a. If true, call `stop_sim_death(data, i + 1)`
//       3b. Return 1
//    4. Return 0
//
// EDGE CASES:
//    - last_meal == 0 happens at very start,	we must ignore it
//      to avoid false deaths.
//
// EXAMPLE:
//  check_philo_death(data,	2) → 1 (if time since meal is 805 and limit is 800)

int	check_philo_death(t_data *data, int i)
{
}

//======================== FUNCTION: check_all_ate ==========================
//
// PURPOSE:
//    Checks if every philosopher has eaten the required number of meals.
//
// RETURN:
//    int
//        - 1  → Everyone ate at least `max_meals` times
//        - 0  → Not everyone has finished
//
// PARAMETERS:
//    t_data *data
//        - Main structure containing max_meals and philos
//
// VARIABLES:
//    uint64_t lm
//        - Dummy variable for last meal
//    int mc
//        - Variable to store meal count
//    int i
//        - Loop counter
//
// ALGORITHM:
//    1. If `data->max_meals` is -1, return 0 (infinite meals mode)
//    2. Initialize `i` to 0
//    3. Enter a while loop that runs while `i < data->philo_count`
//       3a. Call `get_meal_data(&data->philos[i], &lm, &mc)`
//       3b. If `mc < data->max_meals`,
//           return 0 (found someone who needs to eat)
//       3c. Increment `i` by 1
//    4. Return 1 (loop finished, everyone ate enough)
//
// EDGE CASES:
//    - Properly handles the case where `max_meals` wasn't provided in CLI (-1).
//
// EXAMPLE:
//    check_all_ate(data) → 1

int	check_all_ate(t_data *data)
{
}

//======================== FUNCTION: monitor_routine ==========================
//
// PURPOSE:
//    The infinite loop running in the monitor thread to check
//    for deaths or meal fulfillment.
//
// RETURN:
//    void *
//        - Returns NULL when simulation stops
//
// PARAMETERS:
//    void *arg
//        - Cast to `t_data *`
//
// VARIABLES:
//    t_data *data
//        - The main data struct
//    int i
//        - Loop counter for checking each philo
//
// ALGORITHM:
//    1. Assign `data` to `(t_data *)arg`
//    2. Enter a while loop that runs while `!get_all_started(data)`
//       2a. Call `usleep(50)`
//    3. Enter a while loop that runs while `!get_sim_stop(data)`
//       3a. Initialize `i` to 0
//       3b. Enter a while loop that runs while `i < data->philo_count`
//           3b1. If `check_philo_death(data, i)` returns true,
//              	return NULL (thread exits)
//           3b2. Increment `i` by 1
//       3c. If `check_all_ate(data)` returns true
//           3c1. Call `set_sim_stop(data)`
//           3c2. Return NULL
//       3d. Call `usleep(500)` to prevent 100% CPU usage during loop
//    4. Return NULL
//
// EDGE CASES:
//    - Must wait for `all_started` flag before checking conditions,
//     	otherwise it triggers false positives.
//
// EXAMPLE:
//    monitor_routine(data) → Starts monitoring

void	*monitor_routine(void *arg)
{
}
