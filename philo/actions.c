/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: musyilma <musyilma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 10:33:54 by musyilma          #+#    #+#             */
/*   Updated: 2025/07/12 13:57:39 by musyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void	eating(t_philo *philo)
{
	pthread_mutex_t	*first_fork;
	pthread_mutex_t	*second_fork;

	first_fork = philo->left_fork;
	second_fork = philo->right_fork;
	if (philo->thread_no % 2 == 0)
	{
		first_fork = philo->right_fork;
		second_fork = philo->left_fork;
	}
	pthread_mutex_lock(first_fork);
	pthread_mutex_lock(&philo->gen->death_mutex);
	if (philo->gen->someone_died)
	{
		pthread_mutex_unlock(&philo->gen->death_mutex);
		pthread_mutex_unlock(first_fork);
		return ;
	}
	printf("%ld %d %s\n", get_ms(philo), philo->thread_no, "has taken a fork");
	pthread_mutex_unlock(&philo->gen->death_mutex);
	pthread_mutex_lock(second_fork);
	pthread_mutex_lock(&philo->gen->death_mutex);
	if (philo->gen->someone_died)
	{
		pthread_mutex_unlock(&philo->gen->death_mutex);
		pthread_mutex_unlock(second_fork);
		pthread_mutex_unlock(first_fork);
		return ;
	}
	printf("%ld %d %s\n", get_ms(philo), philo->thread_no, "has taken a fork");
	philo->eat++;
	philo->last_meal_time = get_ms(philo);
	printf("%ld %d %s\n", get_ms(philo), philo->thread_no, "is eating");
	pthread_mutex_unlock(&philo->gen->death_mutex);
	ms_usleep(philo->time_to_eat, philo);
	pthread_mutex_unlock(second_fork);
	pthread_mutex_unlock(first_fork);
}

void	sleeping(t_philo *philo)
{
	pthread_mutex_lock(&philo->gen->death_mutex);
	if (!philo->gen->someone_died)
		printf("%ld %d %s\n", get_ms(philo), philo->thread_no, "is sleeping");
	pthread_mutex_unlock(&philo->gen->death_mutex);
	ms_usleep(philo->time_to_sleep, philo);
}

void	thinking(t_philo *philo)
{
	pthread_mutex_lock(&philo->gen->death_mutex);
	if (!philo->gen->someone_died)
		printf("%ld %d %s\n", get_ms(philo), philo->thread_no, "is thinking");
	pthread_mutex_unlock(&philo->gen->death_mutex);
}

void	*thread_loop(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->thread_no % 2 == 0)
		usleep(100);
	while (1)
	{
		pthread_mutex_lock(&philo->gen->death_mutex);
		if (philo->gen->someone_died)
		{
			pthread_mutex_unlock(&philo->gen->death_mutex);
			break ;
		}
		pthread_mutex_unlock(&philo->gen->death_mutex);
		thinking(philo);
		eating(philo);
		sleeping(philo);
	}
	return (NULL);
}

int	thread_start(t_philo **philo, int total_thread)
{
	int i;
	pthread_t monitor_thread;
	void *exit_code;
	int exit_c;

	i = -1;
	while (total_thread != 1 && ++i < total_thread)
		pthread_create(&philo[i]->thread, NULL, thread_loop, philo[i]);
	pthread_create(&monitor_thread, NULL, death_monitor, philo);
	pthread_join(monitor_thread, &exit_code);
	exit_c = (int)(__intptr_t)exit_code;
	if (exit_c == 1)
		return (1);
	i = -1;
	while (++i < total_thread)
		pthread_join(philo[i]->thread, NULL);
	return (0);
}