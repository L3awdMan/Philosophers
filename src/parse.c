/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 18:47:12 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/26 00:06:10 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//======================== FUNCTION: ft_atoi ===========================
//
// PURPOSE:
//    Converts a digit-only string to int.
//    Called AFTER check_num and check_overflow — no guards needed.

static int	ft_atoi(char *str)
{
	int	result;
	int	i;

	result = 0;
	i = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result);
}

//======================== FUNCTION: check_num =========================
//
// PURPOSE:
//    Step 1 of validation — format check.
//    Rejects anything that is not a pure positive digit string.
//
// RETURN:
//    int
//        1 → valid digit string
//        0 → empty, NULL, or contains non-digit character

static int	check_num(char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

//======================== FUNCTION: check_overflow ====================
//
// PURPOSE:
//    Step 2 of validation — overflow check.
//    Compares string digit-by-digit against INT_MAX "2147483647".
//
// RETURN:
//    int
//        1 → value fits in int
//        0 → value exceeds INT_MAX

static int	check_overflow(char *str)
{
	char	*max;
	int		len;

	max = "2147483647";
	len = 0;
	while (str[len])
		len++;
	if (len > 10)
		return (0);
	if (len < 10)
		return (1);
	while (*max && *str)
	{
		if (*str > *max)
			return (0);
		if (*str < *max)
			return (1);
		str++;
		max++;
	}
	return (1);
}

//======================== FUNCTION: set_value =========================
//
// PURPOSE:
//    Single-field validator. Runs the full 3-step pipeline:
//    format → overflow → convert and write into struct field.
//
// RETURN:
//    int
//        1 → field written successfully
//        0 → any validation step failed, error printed to stderr
//
// PARAMETERS:
//    int  *dst — pointer to the target field in t_data
//    char *str — raw argv string to validate and convert
//
// ALGORITHM:
//    1. check_num  → reject bad format
//    2. check_overflow → reject values > INT_MAX
//    3. ft_atoi → convert and write into *dst
//    4. Return 1

static int	set_value(int *dst, char *str)
{
	if (!check_num(str))
		return (ft_error("Error: invalid argument format"), 0);
	if (!check_overflow(str))
		return (ft_error("Error: integer overflow"), 0);
	*dst = ft_atoi(str);
	return (1);
}

//======================== FUNCTION: parse_args ========================
//
// PURPOSE:
//    Entry point for all argument parsing.
//    Validates argument count, fills all t_data fields.
//    max_meals defaults to -1 when optional arg is absent.
//
// RETURN:
//    int
//        1 → all fields parsed and valid
//        0 → any argument invalid, error printed to stderr
//
// PARAMETERS:
//    t_data  *data — destination struct
//    int      ac   — argc from main
//    char   **av   — argv from main
//
// ALGORITHM:
//    1. Check ac is 5 or 6
//    2. Parse and validate philo_count — also check 0 < n <= MAX_PHILOS
//    3. Parse time_to_die, time_to_eat, time_to_sleep
//    4. If ac == 6, parse max_meals — else default to -1
//    5. Return 1
//
// EDGE CASES:
//    - philo_count == 0 → rejected
//    - philo_count > MAX_PHILOS → rejected
//    - max_meals absent → -1 (no meal limit)

int	parse_args(t_data *data, int ac, char **av)
{
	if (ac != 5 && ac != 6)
		return (ft_error("Usage: philo n t_die t_eat t_sleep [meals]"), 0);
	if (!set_value(&data->philo_count, av[1]))
		return (0);
	if (data->philo_count == 0 || data->philo_count > MAX_PHILOS)
		return (ft_error("Error: invalid philosopher count"), 0);
	if (!set_value(&data->time_to_die, av[2]))
		return (0);
	if (!set_value(&data->time_to_eat, av[3]))
		return (0);
	if (!set_value(&data->time_to_sleep, av[4]))
		return (0);
	data->max_meals = -1;
	if (ac == 6 && !set_value(&data->max_meals, av[5]))
		return (0);
	return (1);
}
