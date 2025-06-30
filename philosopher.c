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

long	ft_atol(const char *str)
{
	int		i;
	long	val;
	long	a;

	i = 0;
	val = 0;
	a = 1;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '-')
	{
		a *= -1;
		i++;
	}
	else if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		val = (str[i] - 48) + (10 * val);
		i++;
	}
	if (val * a <= 0)
		return (0);
	return (val * a);
}

long	ft_numeric(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+')
		i++;
	while (str[i])
	{
		if (str[i] >= '0' && str[i] <= '9')
			i++;
		else
			return (0);
	}
	return (1);
}

void	arg_create(int arg, char **args, t_philo ***philo)
{
	int	i;
	int	size;

	size = ft_atol(args[1]);
	*philo = malloc(sizeof(t_philo *) * (size + 1));
	i = 0;
	while (i < size)
	{
		(*philo)[i] = malloc(sizeof(t_philo));
		(*philo)[i]->thread_no = i + 1;
		(*philo)[i]->time_to_die = ft_atol(args[2]) * 1000;
		(*philo)[i]->time_to_eat = ft_atol(args[3]) * 1000;
		(*philo)[i]->time_to_sleep = ft_atol(args[4]) * 1000;
		(*philo)[i]->eat = 0;
		(*philo)[i]->sleep = 0;
		gettimeofday(&(*philo)[i]->start_time, NULL);
		i++;
	}
	(*philo)[i] = NULL;
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

	i = 0;
	while (i < total_thread)
	{
		pthread_create(&philo[i]->thread, NULL, thread_loop, philo[i]);
		i++;
	}
}

int	main(int arg, char **args)
{
	int		i;
	t_philo	**philo;

	i = 0;
	if (!(arg == 5 || arg == 6))
	{
		printf("Wrong argument count\n");
		return (1);
	}
	i++;
	while (args[i])
	{
		if (!ft_atol(args[i]) || !ft_numeric(args[i]))
		{
			printf("Value is wrong\n");
			return (1);
		}
		i++;
	}
	arg_create(arg, args, &philo);
	thread_start(philo, (int)atol(args[1]));
	for (int i = 0; i < atol(args[1]); i++)
        pthread_join(philo[i]->thread, NULL);
}
