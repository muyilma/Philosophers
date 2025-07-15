/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: musyilma <musyilma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 10:33:54 by musyilma          #+#    #+#             */
/*   Updated: 2025/07/15 13:35:38 by musyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int	check_death_status(t_philo *philo)
{
	pthread_mutex_lock(&philo->gen->death_mutex);
	if (philo->gen->someone_died)
	{
		pthread_mutex_unlock(&philo->gen->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->gen->death_mutex);
	return (0);
}

void	print_action(t_philo *philo, char *action)
{
	pthread_mutex_lock(&philo->gen->death_mutex);
	if (!philo->gen->someone_died)
		printf("%ld %d %s\n", get_ms(philo), philo->thread_no, action);
	pthread_mutex_unlock(&philo->gen->death_mutex);
}

int	is_single_philosopher(t_philo *philo)
{
	// Eğer sol ve sağ çatal aynıysa, tek filozof var demektir
	return (philo->left_fork == philo->right_fork);
}

void	single_philosopher_routine(t_philo *philo)
{
	// Tek çatalı al
	pthread_mutex_lock(philo->left_fork);
	if (check_death_status(philo))
	{
		pthread_mutex_unlock(philo->left_fork);
		return ;
	}
	print_action(philo, "has taken a fork");
	// Ölmeyi bekle (ikinci çatal asla gelmeyecek)
	while (!check_death_status(philo))
		usleep(1000);
	pthread_mutex_unlock(philo->left_fork);
}

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
	if (is_single_philosopher(philo))
	{
		single_philosopher_routine(philo);
		return ;
	}
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

void	sleeping(t_philo *philo)
{
	print_action(philo, "is sleeping");
	ms_usleep(philo->time_to_sleep, philo);
}

void	*thread_loop(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->thread_no % 2 == 0)
		usleep(1000);
	// Tek filozof durumu - sadece eating'e git
	if (is_single_philosopher(philo))
	{
		eating(philo);
		return (NULL);
	}
	while (1)
	{
		if (check_death_status(philo))
			break ;
		// Yemek sayısı kontrolü
		pthread_mutex_lock(&philo->gen->death_mutex);
		if (philo->meat_eat != -1 && philo->eat >= philo->meat_eat)
		{
			pthread_mutex_unlock(&philo->gen->death_mutex);
			break ;
		}
		pthread_mutex_unlock(&philo->gen->death_mutex);
		print_action(philo, "is thinking");
		eating(philo);
		sleeping(philo);
	}
	return (NULL);
}

int	thread_start(t_philo **philo, int total_thread)
{
	int			i;
	pthread_t	monitor_thread;
	void		*exit_code;
	int			exit_c;

	i = -1;
	while (++i < total_thread)
		pthread_create(&philo[i]->thread, NULL, thread_loop, philo[i]);
	pthread_create(&monitor_thread, NULL, death_monitor, philo);
	pthread_join(monitor_thread, &exit_code);
	exit_c = (int)(__intptr_t)exit_code;
	i = -1;
	while (++i < total_thread)
		pthread_join(philo[i]->thread, NULL);
	if (exit_c == 1)
		return (1);
	else
		return (0);
}
