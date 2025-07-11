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
		// if (check_death(philo))
		// return (1);
		usleep(100);
	}
	// if (check_death(philo))
	// return (1);
	return (0);
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
		philo->is_dead = 1;
		printf("%ld %d %s\n", get_ms(philo), philo->thread_no, "died");
		pthread_mutex_unlock(&philo->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->death_mutex);
	return (0);
}

int	ft_must_eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->death_mutex);
	if (philo->meat_eat != -1 && (philo->meat_eat <= philo->eat))
	{
		pthread_mutex_unlock(&philo->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->death_mutex);
	return (0);
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
		while (ft_must_eat(philo[i]))
		{
			if (i == total_thread - 1)
				return ((void *)1);
			i++;
		}
		usleep(100);
	}
	return ((void *)0);
}

