/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zotaj-di <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 00:14:20 by zotaj-di          #+#    #+#             */
/*   Updated: 2026/03/26 00:00:58 by zotaj-di         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define MAX_PHILOS 256

typedef struct s_data	t_data;

typedef enum e_status
{
	DIED,
	EATING,
	SLEEPING,
	THINKING,
	TAKE_FORK,
}						t_status;

/*
** struct s_philo represents an individual philosopher.
**
** id             : The philosopher's unique identifier (1 to philo_count).
** last_meal_time : Timestamp in ms of when the philosopher last started eating
** meal_count     : Number of times this philosopher has eaten.
** thread         : The pthread handling this philosopher's lifecycle.
** first_fork     : Pointer to the first mutex fork to lock
** second_fork    : Pointer to the second mutex fork to lock.
** meal_mutex     : Mutex protecting read/writes to last_meal_time and m_count
** data           : Pointer to the shared global data structure.
*/
typedef struct s_philo
{
	int					id;
	uint64_t			last_meal_time;
	int					meal_count;
	pthread_t			thread;
	pthread_mutex_t		*first_fork;
	pthread_mutex_t		*second_fork;
	pthread_mutex_t		meal_mutex;
	t_data				*data;
}						t_philo;

/*
** struct s_data contains all global rules and shared resources.
**
** philo_count    : Total number of philosophers (and forks) at the table.
** start_time     : Timestamp in ms when the simulation officially starts.
** time_to_die    : Maximum ms a philosopher can survive without eating.
** time_to_eat    : Duration in ms it takes to eat a meal.
** time_to_sleep  : Duration in ms it takes to sleep.
** max_meals      : Target number of meals per philosopher (-1 if infinite).
** philos         : Array storing each philosopher's unique structure.
** forks          : Array of mutexes representing the shared forks.
** monitor_thread : The pthread running the death/meal checking routine.
** print_mutex    : Mutex to prevent scrambled logging in standard output.
** state_mutex    : Mutex protecting global flags (sim_stop, threads_ready,
	all_started).
** sim_stop       : Flag indicating if the simulation should end (1 = stop).
** threads_ready  : Counter for how many threads have successfully initialized
** all_started    : Flag for the main thread to sync the start of all philos.
*/

struct					s_data
{
	int					philo_count;
	uint64_t			start_time;
	int					time_to_die;
	int					time_to_eat;
	int					time_to_sleep;
	int					max_meals;
	t_philo				philos[MAX_PHILOS];
	pthread_mutex_t		forks[MAX_PHILOS];
	pthread_t			monitor_thread;
	pthread_mutex_t		print_mutex;
	pthread_mutex_t		state_mutex;
	int					sim_stop;
	int					threads_ready;
	int					all_started;
};

typedef enum e_err
{
	ERR_ARGC,
	ERR_INVALID,
	ERR_OVERFLOW,
	ERR_NEGATIVE,
	ERR_PHILO,
}						t_err;

/* Error utils function */
int						ft_strlen(char *str);
int						ft_error(char *str);

/* Main function */
int						main(int ac, char **av);
int						check_ac(int ac);
int						start_simulation(t_data *data);
void					join_threads(t_data *data);

/* Parsing function */
int						parse_args(t_data *data, int ac, char **av);

/* init.c */
int						init_data(t_data *data);
int						init_philos(t_data *data);
void					set_forks(t_philo *philo, t_data *data);
int						init_mutexes(t_data *data);
void					destroy_mutexes(t_data *data);

/* routine.c */
void					*philo_routine(void *arg);
void					eat(t_philo *philo);
void					philo_sleep(t_philo *philo);
void					think(t_philo *philo);
void					alone_philo(t_philo *philo);

/* monitor.c */
void					*monitor_routine(void *arg);
int						check_philo_death(t_data *data, int i);
int						check_all_ate(t_data *data);
void					get_meal_data(t_philo *philo, uint64_t *lm, int *mc);
void					stop_sim_death(t_data *data, int id);

/* print.c */
void					print_status(t_philo *philo, t_status status);
void					print_line(t_data *data, int id, t_status status);
int						get_sim_stop(t_data *data);
void					set_sim_stop(t_data *data);

/* time.c */
uint64_t				get_time_ms(void);
uint64_t				time_since(uint64_t start);
void					precise_wait(uint64_t ms, t_data *data);

/* utils.c */
void					signal_ready(t_data *data);
void					wait_for_start(t_data *data);
int						get_all_started(t_data *data);
void					set_last_meal(t_philo *philo, uint64_t t);
void					print_error(t_err err, char *extra);

#endif
