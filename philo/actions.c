/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: musyilma <musyilma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 10:33:54 by musyilma          #+#    #+#             */
/*   Updated: 2025/07/15 19:21:48 by musyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int	taken_fork(t_philo *philo, pthread_mutex_t **first_fork,
		pthread_mutex_t **second_fork)
{
	if (philo->left_fork < philo->right_fork)
	{
		*first_fork = philo->left_fork;
		*second_fork = philo->right_fork;
	}
	else
	{
		*first_fork = philo->right_fork;
		*second_fork = philo->left_fork;
	}
	pthread_mutex_lock(*first_fork);
	print_action(philo, "has taken a fork");
	pthread_mutex_lock(*second_fork);
	if (check_death_status(philo))
	{
		pthread_mutex_unlock(*second_fork);
		pthread_mutex_unlock(*first_fork);
		return (1);
	}
	print_action(philo, "has taken a fork");
	return (0);
}

void	eating(t_philo *philo)
{
	pthread_mutex_t	*first_fork;
	pthread_mutex_t	*second_fork;

	first_fork = NULL;
	second_fork = NULL;
	if (taken_fork(philo, &first_fork, &second_fork))
		return ;
	pthread_mutex_lock(&philo->gen->death_mutex);
	if (!philo->gen->someone_died)
	{
		philo->eat++;
		philo->last_meal_time = get_ms(philo);
		printf("%ld %d %s\n", get_ms(philo), philo->thread_no, "is eating");
	}
	pthread_mutex_unlock(&philo->gen->death_mutex);
	ms_usleep(philo->time_to_eat, philo);
	pthread_mutex_unlock(second_fork);
	pthread_mutex_unlock(first_fork);
}

void	*single_thread(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(philo->left_fork);
	if (check_death_status(philo))
	{
		pthread_mutex_unlock(philo->left_fork);
		return ((void *)1);
	}
	print_action(philo, "has taken a fork");
	while (!check_death_status(philo))
		usleep(1000);
	pthread_mutex_unlock(philo->left_fork);
	return ((void *)0);
}

void	*thread_loop(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->thread_no % 2 == 0)
		usleep(1000);
	while (1)
	{
		if (check_death_status(philo))
			break ;
		pthread_mutex_lock(&philo->gen->death_mutex);
		if (philo->meat_eat != -1 && philo->eat >= philo->meat_eat)
		{
			pthread_mutex_unlock(&philo->gen->death_mutex);
			break ;
		}
		pthread_mutex_unlock(&philo->gen->death_mutex);
		print_action(philo, "is thinking");
		eating(philo);
		print_action(philo, "is sleeping");
		ms_usleep(philo->time_to_sleep, philo);
	}
	return (NULL);
}
void	thread_start(t_philo **philo, int total_thread)
{
	int			i;
	pthread_t	monitor_thread;

	i = -1;
	while (total_thread != 1 && ++i < total_thread)
		pthread_create(&philo[i]->thread, NULL, thread_loop, philo[i]);
	if (total_thread == 1)
		pthread_create(&philo[0]->thread, NULL, single_thread, philo[0]);
	pthread_create(&monitor_thread, NULL, death_monitor, philo);
	pthread_join(monitor_thread, NULL);
	i = -1;
	while (++i < total_thread)
		pthread_join(philo[i]->thread, NULL);
}
