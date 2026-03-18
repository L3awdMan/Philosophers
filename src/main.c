/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 18:01:26 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/18 23:43:57 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

//======================== FUNCTION: check_ac ==========================
//
// PURPOSE:
//    Validates argument count before any parsing begins.
//    First gate: stops garbage from entering the pipeline.
//
// RETURN:
//    int
//        1 → correct count (5 or 6 args)
//        0 → wrong count, error printed to stderr
//
// PARAMETERS:
//    int ac
//        - argc from main
//
// ALGORITHM:
//    1. If ac != 5 AND ac != 6 → write usage to stderr, return 0
//    2. Otherwise return 1
//
// EDGE CASES:
//    - ac < 5 (missing args)
//    - ac > 6 (too many args)

int	check_ac(int ac)
{
	if (ac != 5 && ac != 6)
	{
		ft_error("Usage: philo num_philos t_die t_eat t_sleep [meals]");
		return (0);
	}
	return (1);
}

//======================== FUNCTION: main ==========================
//
// PURPOSE:
//    Program entry point. Orchestrates the full pipeline:
//    validate → parse → init → simulate → cleanup
//
// RETURN:
//    int
//        0 → success
//        1 → any failure (bad args, alloc fail, thread fail)
//
// PARAMETERS:
//    int ac        - argument count
//    char **av     - argument values
//
// VARIABLES:
//    t_data data   - central simulation state, stack-allocated
//
// ALGORITHM:
//    1. Check argument count (check_ac)
//    2. Zero-init data with memset (clean slate, no garbage)
//    3. Parse and validate arguments into data (parse_args)
//    4. Init mutexes, forks array (init_data)
//    5. Init philosopher structs + pre-compute fork order (init_philos)
//    6. Run simulation: barrier + threads + monitor (start_simulation)
//    7. Cleanup and return 0
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
		return (cleanup_data(&data), 1);
	if (!start_simulation(&data))
		return (cleanup_data(&data), 1);
	cleanup_data(&data);
	return (0);
}
