/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: musyilma <musyilma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 10:33:54 by musyilma          #+#    #+#             */
/*   Updated: 2025/07/15 17:25:07 by musyilma         ###   ########.fr       */
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
