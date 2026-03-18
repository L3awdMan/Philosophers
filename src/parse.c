/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 18:47:12 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/18 22:59:46 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//======================== FUNCTION: is_valid_str ==========================
//
// PURPOSE:
//    Rejects anything that isn't a pure positive integer string.

static int	is_valid_str(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

//======================== FUNCTION: ft_atol ==========================
//
// PURPOSE:
//    Safe string-to-long. Only called AFTER is_valid_str passes.
//    Returns long (not int) so overflow check can happen in caller.

static long	ft_atol(char *str)
{
	long	n;
	int		i;

	n = 0;
	i = 0;
	if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		n = n * 10 + (str[i] - '0');
		i++;
	}
	return (n);
}

//======================== FUNCTION: set_value ==========================
//
// PURPOSE:
//    Single-field validator. Wraps the 3 steps:
//    format → overflow/range → write into struct field.
//    Prints specific error to stderr on failure.

static int	set_value(char *dest, char *str, char *name)
{
	long	val;

	if (!is_valid_str(str))
		return (ft_error("Error: invalid argument format"), 0);
	val = ft_atol(str);
	if (val <= 0 || val > 2147483647)
		return (ft_error("Error: argument out of range"), 0);
	(void)name;
	*dest = (int)val;
	return (1);
}

//======================== FUNCTION: parse_args ==========================
//
// PURPOSE:
//    Fills all t_data timing/count fields from argv.
//    meals_required defaults to -1 (means "no limit") when arg 5 absent.

int	parse_args(t_data *data, int ac, char **av)
{
	if (!set_value(&data->num_philos, av[1], "num_philos"))
		return (0);
	if (!set_value(&data->time_to_die, av[2], "time_to_die"))
		return (0);
	if (!set_value(&data->time_to_eat, av[3], "time_to_eat"))
		return (0);
	if (!set_value(&data->time_to_sleep, av[4], "time_to_sleep"))
		return (0);
	data->meals_required = -1;
	if (ac == 6 && !set_value(&data->meals_required, av[5], "meals"))
		return (0);
	return (1);
}
