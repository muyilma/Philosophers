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
#include <unistd.h>

long	get_ms(t_philo *philo)
{
	struct timeval	now;
	long			second;
	long			m_second;

	gettimeofday(&now, NULL);
	second = now.tv_sec - philo->start_time.tv_sec;
	m_second = now.tv_usec - philo->start_time.tv_usec;
	return ((second * 1000) + (m_second / 1000));
}

void	print_status(t_philo *philo, char *status)
{
	pthread_mutex_lock(&philo->print_mutex);
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
	philo->eat++;
	philo->last_meal_time = get_ms(philo);
	print_status(philo, "is eating");
	
	usleep(philo->time_to_eat);
	
	// Çatalları bırak
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

void	sleeping(t_philo *philo)
{
	philo->sleep++;
	print_status(philo, "is sleeping");
	usleep(philo->time_to_sleep);
}

void	thinking(t_philo *philo)
{
	print_status(philo, "is thinking");
	// Thinking için küçük bir bekleme süresi
	usleep(1000); // 1ms
}

int	check_death(t_philo *philo)
{
	pthread_mutex_lock(&philo->death_mutex);
	if (philo->is_dead)
	{
		pthread_mutex_unlock(&philo->death_mutex);
		return (1);
	}
	
	if (get_ms(philo) - philo->last_meal_time > philo->time_to_die / 1000)
	{
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
	
	// Tek numaralı filozoflar biraz bekleyerek başlar (deadlock önleme)
	if (philo->thread_no % 2 == 0)
		usleep(1000);
	
	while (!check_death(philo))
	{
		// Yemek ye
		eating(philo);
		
		// Ölüm kontrolü
		if (check_death(philo))
			break;
		
		// Uyku
		sleeping(philo);
		
		// Ölüm kontrolü
		if (check_death(philo))
			break;
		
		// Düşünme
		thinking(philo);
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
	
	// Toplam thread sayısını bul
	while (philo[total_thread])
		total_thread++;
	
	while (1)
	{
		i = 0;
		while (i < total_thread)
		{
			if (check_death(philo[i]))
				return (NULL);
			i++;
		}
		usleep(1000); // 1ms bekle
	}
	return (NULL);
}

void	thread_start(t_philo **philo, int total_thread)
{
	int			i;
	pthread_t	monitor_thread;

	i = 0;
	// Tüm threadleri başlat
	while (i < total_thread)
	{
		pthread_create(&philo[i]->thread, NULL, thread_loop, philo[i]);
		i++;
	}
	
	// Ölüm monitörü threadini başlat
	pthread_create(&monitor_thread, NULL, death_monitor, philo);
	
	// Tüm threadlerin bitmesini bekle
	i = 0;
	while (i < total_thread)
	{
		pthread_join(philo[i]->thread, NULL);
		i++;
	}
	
	pthread_join(monitor_thread, NULL);
}