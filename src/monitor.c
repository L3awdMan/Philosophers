/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 00:05:33 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/26 00:10:00 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//======================== FUNCTION: monitor_routine ==========================
//
// PURPOSE:
//    The infinite loop running in the monitor thread to check
//    for deaths or meal fulfillment.
//
//        - Returns NULL when simulation stops (thread exit)
//
// ALGORITHM:
//    2. Enter a while loop waits until the main thread has successfully
//       created all philosopher thread and set the all_started flag to 1
//    3. Enter a while loop that runs while `!get_sim_stop(data)`
//       3b. Enter a while loop that runs as long as the simulation hasn't
//           been stopped
//           3b1. If `check_philo_death(data, i)` returns true, means
//                  a philosopher died, the monitor immediately returns NULL,
//                  which stops the thread .
//           3b2. Increment `i` by 1
//       3c. After checking all philosophers for death, it calls check_all_ate()
//           3c1. If every philosopher has finished their required
//                number of meals (if a limit was set), it calls set_sim_stop()
//                to end the simulation and returns NULL
//       3d. Call `usleep(500)`
//           "This prevents the monitor thread from using
//           100% of the CPU while it waits for"

void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		i;

	data = (t_data *)arg;
	while (!get_all_started(data))
		usleep(50);
	while (!get_sim_stop(data))
	{
		i = 0;
		while (i < data->philo_count)
		{
			if (check_philo_death(data, i))
				return (NULL);
			i++;
		}
		if (check_all_ate(data))
			return (set_sim_stop(data), NULL);
		usleep(500);
	}
	return (NULL);
}

//======================== FUNCTION: get_meal_data ==========================
//
// PURPOSE:
//   Thread-safely get/bring the last_meal_time and meal_count of a philosopher
//
// EDGE CASES:
//    - Must be called whenever reading these variables to prevent data races.
//
// EXAMPLE:
//    get_meal_data(p, &lm, &mc) → lm = 500, mc = 2

void	get_meal_data(t_philo *philo, uint64_t *last_m, int *m_count)
{
	pthread_mutex_lock(&philo->meal_mutex);
	*last_m = philo->last_meal_time;
	*m_count = philo->meal_count;
	pthread_mutex_unlock(&philo->meal_mutex);
}

//======================== FUNCTION: stop_sim_death ==========================
//
// PURPOSE:
//    Triggers the global stop flag and prints the death message.
//
// EDGE CASES:
// - Printing inside a separate mutex ensures the death message doesn't overlap
//
// EXAMPLE:
//    stop_sim_death(data, 3) → Sets flag and prints "801 3 died"

void	stop_sim_death(t_data *data, int id)
{
	set_sim_stop(data);
	pthread_mutex_lock(&data->print_mutex);
	printf("%lu %d died\n", time_since(data->start_time), id);
	pthread_mutex_unlock(&data->print_mutex);
}

//======================= FUNCTION: check_philo_death ==========================
//
// PURPOSE:
//    Checks if a specific philosopher has starved
//    (time since last meal > time_to_die).
//
// RETURN:
//        - 1  → Philosopher died
//        - 0  → Philosopher is still alive (or hasn't started eating yet)
//
// ALGORITHM:
//    2. If `last_meal` is 0,
//			return 0 (means thread hasn't properly initialized yet)
//    3. Check if `time_since(last_meal)` > `(uint64_t)data->time_to_die`
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
	uint64_t	last_meal;
	int			m_count;

	get_meal_data(&data->philos[i], &last_meal, &m_count);
	if (last_meal == 0)
		return (0);
	if (time_since(last_meal) > (uint64_t)data->time_to_die)
		return (stop_sim_death(data, i + 1), 1);
	return (0);
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
// ALGORITHM:
//    1. If `data->max_meals` is -1, return 0 (infinite meals mode)
//    2. Initialize `i` to 0
//    3. Enter a while loop that runs while `i < data->philo_count`
//       3a. Call `get_meal_data(&data->philos[i], &lm, &mc)`
//       3b. If `mc < data->max_meals`,
//           return 0 (found someone who needs to eat)
//       3c. Increment `i` by 1
//    4. Return 1 (loop finished, everyone ate enough)

int	check_all_ate(t_data *data)
{
	uint64_t	last_m;
	int			m_count;
	int			i;

	if (data->max_meals == -1)
		return (0);
	i = 0;
	while (i < data->philo_count)
	{
		get_meal_data(&data->philos[i], &last_m, &m_count);
		if (m_count < data->max_meals)
			return (0);
		i++;
	}
	return (1);
}
