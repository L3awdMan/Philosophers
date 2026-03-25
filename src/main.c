/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 18:01:26 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/26 00:04:54 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//======================== FUNCTION: main ==========================
//
// PURPOSE:
//    validate → parse → init → simulate → cleanup
//
// EDGE CASES:
//    - Any step fails → cleanup called before return to avoid leaks
//    - data is on the stack → memset mandatory before use

int	main(int ac, char **av)
{
	t_data	data;

	if (!check_ac(ac))
		return (1);
	memset(&data, 0, sizeof(t_data));
	if (!parse_args(&data, ac, av))
		return (1);
	if (!init_data(&data))
		return (1);
	if (!init_philos(&data))
		return (destroy_mutexes(&data), 1);
	if (!start_simulation(&data))
		return (destroy_mutexes(&data), 1);
	join_threads(&data);
	destroy_mutexes(&data);
	return (0);
}

//======================== FUNCTION: check_ac ==========================
//
// PURPOSE:
//    Validates argument count before any parsing begins.
//    First gate: stops garbage from entering the pipeline.

int	check_ac(int ac)
{
	if (ac != 5 && ac != 6)
	{
		ft_error("Usage: philo num_philos t_die t_eat t_sleep [meals]");
		return (0);
	}
	return (1);
}

int	start_simulation(t_data *data)
{
	int	i;

	if (pthread_create(&data->monitor_thread, NULL, monitor_routine, data) != 0)
		return (0);
	i = 0;
	while (i < data->philo_count)
	{
		if (pthread_create(&data->philos[i].thread, NULL, philo_routine,
				&data->philos[i]) != 0)
			return (0);
		i++;
	}
	wait_for_start(data);
	pthread_mutex_lock(&data->state_mutex);
	data->start_time = get_time_ms();
	data->all_started = 1;
	pthread_mutex_unlock(&data->state_mutex);
	return (1);
}

void	join_threads(t_data *data)
{
	int	i;

	pthread_join(data->monitor_thread, NULL);
	i = 0;
	while (i < data->philo_count)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}
}
