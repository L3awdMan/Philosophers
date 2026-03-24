/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 14:58:41 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/24 20:59:44 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
{
	int	i;

	i = 0;
	while (i < data->philo_count)
	{
		data->philos[i].id = i + 1;
		data->philos[i].last_meal_time = 0;
		data->philos[i].meal_count = 0;
		data->philos[i].data = data;
		set_forks(&data->philos[i], data);
		i++;
	}
	return (1);
}

//======================== FUNCTION: set_forks =========================
//
// PURPOSE:
//    Pre-computes which fork each philo grabs first and second.
//    Dijkstra resource hierarchy — lower index fork always first.
//    Called once at init — zero branching at runtime.
//
// VARIABLES:
//    int left  — index of left fork  = philo->id - 1
//    int right — index of right fork = philo->id % philo_count

void	set_forks(t_philo *philo, t_data *data)
{
	int	left;
	int	right;

	left = philo->id - 1;
	right = philo->id % philo_count;
	if (left < right)
	{
		philo->first_fork = &data->forks[left];
		philo->second_fork = &data->forks[right];
	}
	else
	{
		philo->first_fork = &data->forks[right];
		philo->second_fork = &data->forks[left];
	}
}

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

int	init_mutexes(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->philo_count)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
			return (0);
		if (pthread_mutex_init(&data->philos[i].meal_mutex, NULL) != 0)
			return (0);
		i++;
	}
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->state_mutex, NULL) != 0)
		return (0);
	return (1);
}

//======================== FUNCTION: destroy_mutexes ===================
//
// PURPOSE:
//    Destroys ALL mutexes in reverse init order.
//    Called by cleanup before program exits.

void	destroy_mutexes(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->philo_count)
	{
		pthread_mutex_destroy(&data->forks[i]);
		pthread_mutex_destroy(&data->philos[i].meal_mutex);
		i++;
	}
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->state_mutex);
}

//======================== FUNCTION: init_data =========================
//
// PURPOSE:
//    Top-level init entry point.
//    Zeros barrier vars explicitly, then inits mutexes and philos.
//
// RETURN:
//        1 → fully initialized
//        0 → any step failed

int	init_data(t_data *data)
{
	data->start_time = 0;
	data->sim_stop = 0;
	data->threads_ready = 0;
	data->all_started = 0;
	if (!init_mutexes(data))
		return (0);
	if (!init_philos(data))
		return (0);
	return (1);
}
