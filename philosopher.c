/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: musyilma <musyilma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 10:33:54 by musyilma          #+#    #+#             */
/*   Updated: 2025/07/15 13:36:11 by musyilma         ###   ########.fr       */
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

void	cleanup(t_philo **philo, pthread_mutex_t *forks, t_general *gen,
		int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		pthread_mutex_destroy(&forks[i]);
		free(philo[i]);
		i++;
	}
	pthread_mutex_destroy(&gen->death_mutex);
	free(philo);
	free(forks);
	free(gen);
}

pthread_mutex_t	*fork_create(t_general **gen, int size)
{
	pthread_mutex_t	*fork;
	int				i;

	i = -1;
	*gen = malloc(sizeof(t_general));
	(*gen)->philo = malloc(sizeof(t_philo *) * (size + 1));
	fork = malloc(sizeof(pthread_mutex_t) * size);
	(*gen)->someone_died = 0;
	while (++i < size)
		pthread_mutex_init(&fork[i], NULL);
	pthread_mutex_init(&(*gen)->death_mutex, NULL);
	return (fork);
}

void	arg_create(char **args, t_general **gen, int size)
{
	int				i;
	pthread_mutex_t	*fork;

	fork = fork_create(gen, size);
	i = -1;
	while (++i < size)
	{
		(*gen)->philo[i] = malloc(sizeof(t_philo));
		(*gen)->philo[i]->thread_no = i + 1;
		(*gen)->philo[i]->time_to_die = ft_atol(args[2]);
		(*gen)->philo[i]->time_to_eat = ft_atol(args[3]);
		(*gen)->philo[i]->time_to_sleep = ft_atol(args[4]);
		if (args[5] == NULL)
			(*gen)->philo[i]->meat_eat = -1;
		else
			(*gen)->philo[i]->meat_eat = ft_atol(args[5]);
		(*gen)->philo[i]->eat = 0;
		(*gen)->philo[i]->last_meal_time = 0;
		gettimeofday(&(*gen)->philo[i]->start_time, NULL);
		(*gen)->philo[i]->left_fork = &fork[i];
		(*gen)->philo[i]->right_fork = &fork[(i + 1) % size];
		(*gen)->philo[i]->gen = *gen;
	}
	(*gen)->philo[i] = NULL;
}

int	main(int arg, char **args)
{
	pthread_mutex_t	*forks;
	t_general		*gen;
	int				i;
	int				status;

	status = 0;
	if (!(arg == 5 || arg == 6))
		return (printf("Wrong argument count\n"), 1);
	i = 1;
	while (args[i])
	{
		if (!ft_atol(args[i]) || !ft_numeric(args[i]))
			return (printf("Value is wrong\n"), 1);
		i++;
	}
	arg_create(args, &gen, ft_atol(args[1]));
	status = thread_start(gen->philo, ft_atol(args[1]));
	if (status == 1)
	{
		forks = gen->philo[0]->left_fork;
		cleanup(gen->philo, forks, gen, ft_atol(args[1]));
		return (1);
	}
	forks = gen->philo[0]->left_fork;
	cleanup(gen->philo, forks, gen, ft_atol(args[1]));
}
