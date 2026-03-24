/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 23:53:02 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/25 00:06:13 by zotaj-di         ###   ########.fr       */
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

uint64_t	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	result = (uint64_t)tv.tv_sec * 1000 + ((uint64_t)tv.tv_usec / 1000);
	return (result);
}
//======================== FUNCTION: time_since ========================
//
// PURPOSE:
//    calculate how many milliseconds have passed since a specific starting
//    timestamp
//
// RETURN:
//    uint64_t — ms elapsed, or 0 if clock skew detected
//
// PARAMETERS:
//     uint64_t start — reference timestamp in ms
//
// ALGORITHM:
//    1. Get current time via get_time_ms
//    2. If now >= start → return now - start
//    3. Else → return 0 (protect against unsigned wraparound)
//
// EDGE CASES:
//    - now < start (clock skew) → returns 0 instead of huge number

uint64_t	time_since(uint64_t start)
{
	uint64_t	now;

	now = get_time_ms();
	if (now >= start)
		return (now - start);
	return (0);
}

//======================== FUNCTION: precise_wait ======================
//
// PURPOSE:
//    Performs the waiting , You pass it the amount of time you want to wait
//    (e.g., the time needed to eat or sleep), and it pauses the thread
//    for exactly that long
//
// VARIABLES:
//    uint64_t start     — timestamp when sleep began
//    uint64_t elapsed   — ms passed since start
//    uint64_t remaining — ms left to sleep
//
// ALGORITHM:
//       - remaining > 10ms → usleep(remaining * 500) [coarse]
//       - remaining > 1ms  → usleep(500)             [fine]
//       - remaining <= 1ms → usleep(100)             [ultra-fine]
// TODO : b.u remaining time

void	precise_wait(uint64_t ms, t_data *data)
{
	uint64_t	start;
	uint64_t	elapsed;
	uint64_t	remaining;

	start = get_time_ms();
	while (!get_sim_stop(data))
	{
		elapsed = time_since(start);
		if (elapsed >= start)
			break ;
		remaining = ms - elapsed;
		if (remaining > 10)
			usleep(remaining * 100);
		else if (remaining > 1)
			usleep(500);
		else
			usleep(100);
	}
}
