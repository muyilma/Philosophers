/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
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

	gettimeofday(&now,NULL);
	second = now.tv_sec-philo->start_time.tv_sec;
	m_second = now.tv_usec-philo->start_time.tv_usec;
	return ((second * 1000) + (m_second/1000));
}

void	eating(t_philo *philo)
{
	philo->eat++;
	printf("%ld %d   has taken a fork\n",get_ms(philo), philo->thread_no);
	printf("%ld %d   has taken a fork\n",get_ms(philo), philo->thread_no);
	printf("%ld %d  is eating\n",get_ms(philo), philo->thread_no);
	usleep(philo->time_to_eat);
}

void	sleeping(t_philo *philo)
{
	philo->sleep++;
	printf("%ld %d  is sleeping\n",get_ms(philo), philo->thread_no);
	usleep(philo->time_to_sleep);
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
			{
				eating(philo);
			}
			else
				sleeping(philo);
		}
		else
		{
			if (philo->sleep != philo->eat)
				eating(philo);
			else
				sleeping(philo);
		}
	}
}

void	thread_start(t_philo **philo, int total_thread)
{
	int	i;

	i = -1;
	while (++i < total_thread)
		pthread_create(&philo[i]->thread, NULL, thread_loop, philo[i]);
    
    	i = -1;
	while (++i < total_thread)
        pthread_join(philo[i]->thread,NULL);
}
