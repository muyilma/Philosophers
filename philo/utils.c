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
	size_t	timing;

	timing = get_ms(philo);
	while (get_ms(philo) - timing < ms)
	{
		pthread_mutex_lock(&philo->gen->death_mutex);
		if (philo->gen->someone_died)
		{
			pthread_mutex_unlock(&philo->gen->death_mutex);
			return (1);
		}
		pthread_mutex_unlock(&philo->gen->death_mutex);
		usleep(100);
	}
	return (0);
}

int	check_death(t_philo *philo)
{
	pthread_mutex_lock(&philo->gen->death_mutex);
	if (philo->gen->someone_died)
	{
		pthread_mutex_unlock(&philo->gen->death_mutex);
		return (1);
	}
	if (get_ms(philo) - philo->last_meal_time > philo->time_to_die)
	{
		philo->gen->someone_died = 1;
		printf("%ld %d %s\n", get_ms(philo), philo->thread_no, "died");
		pthread_mutex_unlock(&philo->gen->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->gen->death_mutex);
	return (0);
}

int	ft_must_eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->gen->death_mutex);
	if (philo->meat_eat != -1 && (philo->meat_eat <= philo->eat))
	{
		pthread_mutex_unlock(&philo->gen->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->gen->death_mutex);
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
		i = -1;
		while (++i < total_thread)
		{
			if (check_death(philo[i]))
				return ((void *)1);
		}
		i = -1;
		while (++i < total_thread && ft_must_eat(philo[i]))//ekleme yapıldı
		{
			if (++i == total_thread - 1)
				return ((void *)1);
		}
		usleep(100);
	}
	return ((void *)0);
}
