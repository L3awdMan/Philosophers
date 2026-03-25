/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 18:24:59 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/26 00:12:17 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//======================== FUNCTION: philo_routine ==========================
//
// PURPOSE:
//    The main lifecycle loop for a philosopher thread: eat, sleep, think.
//
//        - Returns NULL when thread ends
//
// ALGORITHM:
//    1. Assign `philo` to `(t_philo *)arg`
//    2. Assign `data` to `philo->data`
//    3. Call `signal_ready(data)` to acknowledge creation and wait for start
//       signal, basically like a handshake
//    4. Call `set_last_meal(philo, data->start_time)`
//    5. If `data->philo_count == 1`
//       5a. Call `alone_philo(philo)` and Return NULL
//    6. If `data->philo_count` is odd AND `philo->id == data->philo_count`
//       6a. Call `precise_wait((uint64_t)data->time_to_eat * 2, data)`
//    7. Else if `philo->id % 2 == 0` (even philosophers)
//       7a. Call `precise_wait((uint64_t)data->time_to_eat / 2, data)`
//    8. Enter a while loop that runs while `!get_sim_stop(data)`
//       8a. Call `eat(philo)`
//       8b. If `get_sim_stop(data)` is true, break out of loop
//       8c. Call `philo_sleep(philo)`
//       8d. If `get_sim_stop(data)` is true, break out of loop
//       8e. Call `think(philo)`
//    9. Return NULL

void	*philo_routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;

	philo = (t_philo *)arg;
	data = philo->data;
	signal_ready(data);
	set_last_meal(philo, data->start_time);
	if (data->philo_count == 1)
		return (alone_philo(philo), NULL);
	if (data->philo_count % 2 == 1 && philo->id == data->philo_count)
		precise_wait((uint64_t)data->time_to_eat * 2, data);
	else if (philo->id % 2 == 0)
		precise_wait((uint64_t)data->time_to_eat / 2, data);
	while (!get_sim_stop(data))
	{
		eat(philo);
		if (get_sim_stop(data))
			break ;
		philo_sleep(philo);
		if (get_sim_stop(data))
			break ;
		think(philo);
	}
	return (NULL);
}
//======================== FUNCTION: alone_philo ==========================
//
// PURPOSE:
//    Special routine for when there is only 1 philosopher.
//    They grab their single fork and then wait indefinitely until the monitor
//    thread detects their death.
//
// EDGE CASES:
//    - Prevents deadlock since one philo can never get a second fork.
//
// EXAMPLE:
//    alone_philo(philo) → waits to die

void	alone_philo(t_philo *philo)
{
	print_status(philo, TAKE_FORK);
	precise_wait((uint64_t)philo->data->time_to_die + 1, philo->data);
}

//======================== FUNCTION: eat ==========================
//
// PURPOSE:
//    Performs the entire eating cycle: lock both forks, update meal timers,
//    wait for eating duration, and release forks.
//
// EXAMPLE:
//    eat(philo) → locks, eats, updates, unlocks

void	eat(t_philo *philo)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(philo->first_fork);
	print_status(philo, TAKE_FORK);
	pthread_mutex_lock(philo->second_fork);
	print_status(philo, TAKE_FORK);
	print_status(philo, EATING);
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_meal_time = get_time_ms();
	philo->meal_count += 1;
	pthread_mutex_unlock(&philo->meal_mutex);
	precise_wait((uint64_t)data->time_to_eat, data);
	pthread_mutex_unlock(philo->second_fork);
	pthread_mutex_unlock(philo->first_fork);
}

//======================== FUNCTION: philo_sleep ==========================
//
// PURPOSE:
//    Handles the sleeping state for the specified duration.
//
// EXAMPLE:
//    philo_sleep(philo) → prints sleep, waits

void	philo_sleep(t_philo *philo)
{
	print_status(philo, SLEEPING);
	precise_wait((uint64_t)philo->data->time_to_sleep, philo->data);
}

//======================== FUNCTION: think ==========================
//
// PURPOSE:
//    Calculates the optimal time to think before trying to eat again.
//    This prevents cpu hogging and organizes fork distribution a
//    mong odd/even philos.
//
// ALGORITHM:
//    1. Call `get_meal_data(philo, &lm, &mc)`
//    2. Assign `since_meal` to `time_since(lm)`
//       " how much time has passed from the last meal until now "
//    3. Check if `since_meal + time_to_eat >= time_to_die`
//       "If I start eating right now, will I finish before my time runs out?"
//       3a. If TRUE, (Danger): It sets think_time = 0
//    4. Else
//       4a. Set : think_time = (time_to_die - since_meal - time_to_eat) / 2
//           - "time_to_die - since_meal" : This is how much time you have left
//                                        before you must be finished eating.
//           - "... - time_to_eat" : This subtracts the time you actually need
//                                  to spend eating
//           - " THE / 2 ": If a philosopher uses all of their time to think,
//                        they might die
//    5. If `think_time > time_to_eat`
//       5a. Assign `think_time` to `time_to_eat` (cap it)

void	think(t_philo *philo)
{
	uint64_t	since_meal;
	uint64_t	think_time;
	uint64_t	lm;
	int			mc;

	get_meal_data(philo, &lm, &mc);
	since_meal = time_since(lm);
	if (since_meal + philo->data->time_to_eat >= philo->data->time_to_die)
		think_time = 0;
	else
		think_time = ((uint64_t)philo->data->time_to_die - since_meal
				- (uint64_t)philo->data->time_to_eat) / 2;
	if (think_time > (uint64_t)philo->data->time_to_eat)
		think_time = (uint64_t)philo->data->time_to_eat;
	print_status(philo, THINKING);
	precise_wait(think_time, philo->data);
}
