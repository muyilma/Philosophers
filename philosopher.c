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

void	arg_create(char **args, t_philo ***philo)
{
	int				i;
	int				size;
	pthread_mutex_t	*fork;

	size = ft_atol(args[1]);
	*philo = malloc(sizeof(t_philo *) * (size + 1));
	fork = malloc(sizeof(pthread_mutex_t) * size);
	i = -1;
	while (++i < size)
		pthread_mutex_init(&fork[i], NULL);
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
		(*philo)[i]->left_fork = &fork[i];
		(*philo)[i]->right_fork = &fork[(i + 1) % size];
		i++;
	}
	(*philo)[i] = NULL;
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
	arg_create(args, &philo);

}
