/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 17:11:26 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/26 00:07:46 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//======================== FUNCTION: print_error ==========================
//
// PURPOSE:
//    Prints standardized error messages to standard error (fd 2).

void	print_error(t_err err, char *extra)
{
	if (err == ERR_ARGC)
		ft_error("Error: expected 4 or 5 arguments");
	else if (err == ERR_INVALID)
		ft_error("Error: invalid number");
	else if (err == ERR_OVERFLOW)
		ft_error("Error: integer overflow");
	else if (err == ERR_NEGATIVE)
		ft_error("Error: negative number");
	else if (err == ERR_PHILO)
		ft_error("Error: philo count must be 1-256");
	(void)extra;
}
//======================== FUNCTION: signal_ready ==========================
//
// PURPOSE:
//    Called by each philosopher thread on startup to notify the main thread
//    that it is alive, then spins until the global start flag is set.
//
// EDGE CASES:
//    - Ensures no thread starts executing its routine before all threads exist

void	signal_ready(t_data *data)
{
	pthread_mutex_lock(&data->state_mutex);
	data->threads_ready++;
	pthread_mutex_unlock(&data->state_mutex);
	while (!get_all_started(data))
		usleep(50);
}

//======================== FUNCTION: wait_for_start ==========================
//
// PURPOSE:
//    Called by the main thread. Blocks until all philosopher threads have
//    signaled they are ready to begin.
//
// EXAMPLE:
//    wait_for_start(data) → holds until 5 out of 5 threads report ready

void	wait_for_start(t_data *data)
{
	int	ready;

	ready = 0;
	while (ready < data->philo_count)
	{
		pthread_mutex_lock(&data->state_mutex);
		ready = data->threads_ready;
		pthread_mutex_unlock(&data->state_mutex);
		usleep(50);
	}
}

//======================== FUNCTION: get_all_started ==========================
//
// PURPOSE:
//    Thread-safely reads the `all_started` flag.

int	get_all_started(t_data *data)
{
	int	flag;

	pthread_mutex_lock(&data->state_mutex);
	flag = data->all_started;
	pthread_mutex_unlock(&data->state_mutex);
	return (flag);
}

//======================== FUNCTION: set_last_meal ==========================
//
// PURPOSE:
//    Thread-safely sets the last meal timestamp for a specific philosopher.
//
// EDGE CASES:
//    - Generally used at simulation start so the monitor doesn't think
//      they instantly died.
//
// EXAMPLE:
//    set_last_meal(philo, 160000) → sets time to 160000

void	set_last_meal(t_philo *philo, uint64_t t)
{
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_meal_time = t;
	pthread_mutex_unlock(&philo->meal_mutex);
}
