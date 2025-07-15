/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: musyilma <musyilma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 17:28:09 by musyilma          #+#    #+#             */
/*   Updated: 2025/07/15 18:56:16 by musyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

# include <pthread.h>
# include <sys/time.h>

typedef struct s_philo
{
	int					thread_no;
	long				eat;
	long				time_to_die;
	long				time_to_eat;
	long				time_to_sleep;
	long				last_meal_time;
	long				meat_eat;
	pthread_t			thread;
	pthread_mutex_t		*left_fork;
	pthread_mutex_t		*right_fork;
	struct s_general	*gen;
	struct timeval		start_time;
}						t_philo;

typedef struct s_general
{
	pthread_mutex_t		death_mutex;
	int					someone_died;
	t_philo				**philo;
}						t_general;

void						thread_start(t_philo **philo, int total_thread);
int						check_death_status(t_philo *philo);
void					*death_monitor(void *arg);
long					get_ms(t_philo *philo);
long					ms_usleep(size_t ms, t_philo *philo);
void					print_action(t_philo *philo, char *action);
void					cleanup(t_philo **philo, pthread_mutex_t *forks,
							t_general *gen, int size);

#endif