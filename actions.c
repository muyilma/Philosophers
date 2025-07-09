/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: musyilma <musyilma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 10:33:54 by musyilma          #+#    #+#             */
/*   Updated: 2025/06/29 19:16:46 by musyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

long	get_ms(t_philo *philo)
{
	struct timeval	now;
	long			seconds;
	long			useconds;

	gettimeofday(&now, NULL);
	seconds = now.tv_sec - philo->start_time.tv_sec;
	useconds = now.tv_usec - philo->start_time.tv_usec;
	return (seconds * 1000) + (useconds / 1000);
}
long	ms_usleep(size_t ms, t_philo *philo)
{
	size_t	timeing;

	timeing = get_ms(philo);
	while (get_ms(philo) - timeing < ms)
	{
		if (check_death(philo))
			return (1);
		usleep(100);
	}
	if (check_death(philo))
		return (1);
	return (0);
}
void	print_status(t_philo *philo, char *status)
{
	pthread_mutex_lock(&philo->print_mutex);
	// Eğer "died" mesajı ise yazdır, sonra is_dead flag'ini set et
	if (strcmp(status, "died") == 0) // BURAYI FT_STRCMP İLE DEĞİŞTİR
	{
		printf("%ld %d %s\n", get_ms(philo), philo->thread_no, status);
		philo->is_dead = 1;
		pthread_mutex_unlock(&philo->print_mutex);
		return ;
	}
	// Herhangi bir filosof öldüyse diğer mesajları yazdırma
	if (philo->is_dead)
	{
		pthread_mutex_unlock(&philo->print_mutex);
		return ;
	}
	printf("%ld %d %s\n", get_ms(philo), philo->thread_no, status);
	pthread_mutex_unlock(&philo->print_mutex);
}

void	eating(t_philo *philo)
{
	// Çatalları mutex ile güvenli şekilde al
	pthread_mutex_lock(philo->left_fork);
	print_status(philo, "has taken a fork");
	pthread_mutex_lock(philo->right_fork);
	print_status(philo, "has taken a fork");
	// Yemek yeme
	philo->last_meal_time = get_ms(philo);
	print_status(philo, "is eating");
	philo->eat++;
	ms_usleep(philo->time_to_eat, philo);
	// Çatalları bırak
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

void	sleeping(t_philo *philo)
{
	philo->sleep++;
	print_status(philo, "is sleeping");
	ms_usleep(philo->time_to_sleep, philo);
}

void	thinking(t_philo *philo)
{
	print_status(philo, "is thinking");
}

int	check_death(t_philo *philo)
{
	pthread_mutex_lock(&philo->death_mutex);
	if (philo->is_dead)
	{
		pthread_mutex_unlock(&philo->death_mutex);
		return (1);
	}
	if (get_ms(philo) - philo->last_meal_time > philo->time_to_die)
	{
		//  printf("BURADA OLDU:%ld - lmt(%ld) > ttd(%ld)\n", get_ms(philo),
		// 		philo->last_meal_time, philo->time_to_die / 1000);
		philo->is_dead = 1;
		pthread_mutex_unlock(&philo->death_mutex);
		print_status(philo, "died");
		return (1);
	}
	pthread_mutex_unlock(&philo->death_mutex);
	return (0);
}

void	*thread_loop(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	while (1)
	{
		if (philo->thread_no % 2 != 0)
		{
			if (philo->sleep == philo->eat)
				eating(philo);
			else
			{
				sleeping(philo);
				thinking(philo);
			}
		}
		else
		{
			if (philo->sleep != philo->eat)
				eating(philo);
			else
			{
				sleeping(philo);
				thinking(philo);
			}
		}
	}
	return (NULL);
}

void	*death_monitor(void *arg)
{
	t_philo	**philo;
	int		i;
	int		total_thread;

	philo = (t_philo **)arg;
	total_thread = 0;
	while (philo[total_thread])
		total_thread++;
	while (1)
	{
		i = 0;
		while (i < total_thread)
		{
			if (check_death(philo[i]))
				return ((void *)1);
			i++;
		}
		i = 0;
		while (philo[i]->meat_eat != -1 && philo[i]->meat_eat <= philo[i]->eat)
		{
			if (i == total_thread - 1)
				return ((void *)1);
			i++;
		}
		usleep(100);
	}
	return ((void *)0);
}

int	thread_start(t_philo **philo, int total_thread)
{
	int			i;
	pthread_t	monitor_thread;
	void		*exit_code;
	int			exit_c;

	i = -1;
	if (total_thread % 2 == 0)
	{
		while (++i < total_thread)
			pthread_create(&philo[i]->thread, NULL, thread_loop, philo[i]);
	}
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
