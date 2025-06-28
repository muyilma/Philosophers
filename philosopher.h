#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

# include <pthread.h>

typedef struct s_philo
{
	int				thread_no;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	pthread_t		thread;
	pthread_mutex_t	mutex;
}					t_philo;

#endif