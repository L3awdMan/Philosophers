/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 00:14:20 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/18 23:44:55 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <limits.h> // INT_MAX
# include <pthread.h>
# include <stdint.h> // uint64_t
# include <stdio.h>
# include <stdlib.h>
# include <string.h>   // memset
# include <sys/time.h> // gettimeofday
# include <unistd.h>

# define MAX_PHILOS 256

typedef struct s_philo	t_philo;
typedef struct s_data	t_data;

typedef enum e_status
{
	DIED,
	EATING,
	SLEEPING,
	THINKING,
	TAKE_FORK, // was FORK — matches subject string "has taken a fork"
}						t_status;

struct					s_data
{
	int philo_count; // av[1]
	uint64_t			start_time;
	// ms timestamp when ALL threads are ready
	int time_to_die;                   // av[2] in ms
	int time_to_eat;                   // av[3] in ms
	int time_to_sleep;                 // av[4] in ms
	int max_meals;                     // av[5], or -1 if absent
	t_philo philos[MAX_PHILOS];        // all philo stack-allocated
	pthread_mutex_t forks[MAX_PHILOS]; // one mutex per fork
	pthread_t monitor_thread;          // death-check thread
	pthread_mutex_t print_mutex;       // serializes all printf
	pthread_mutex_t state_mutex;       // guards sim_stop + barrier vars
	int sim_stop;                      // 1 = end simulation
	int threads_ready;                 // barrier: counts ready threads
	int all_started;                   // barrier: main sets to 1
};

typedef struct s_philo
{
	int id;                       // 1-indexed display number
	uint64_t last_meal_time;      // written by philo, read by monitor
	int meal_count;               // incremented each eat cycle
	pthread_t thread;             // this philo's thread handle
	pthread_mutex_t *first_fork;  // grab first (pre-computed, Dijkstra)
	pthread_mutex_t *second_fork; // grab second
	pthread_mutex_t meal_mutex;   // guards last_meal_time + meal_count
	t_data *data;                 // back-pointer to shared state
}						t_philo;

/* Error utils function */
const char				*ft_strlen(char *str);
int						ft_error(char *str);

/* Main function */
int						main(int ac, char **av);
int						check_ac(int ac);

/* Parsing function */
int						parse_args(t_data *data, int ac, char **av);

#endif
