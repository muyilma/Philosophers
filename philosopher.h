#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

# include <pthread.h>
#include <sys/time.h>

typedef struct s_philo
{
	int				thread_no;
	long			eat;
	long			sleep;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	pthread_t		thread;
	pthread_mutex_t	mutex;
	struct timeval  start_time;
}					t_philo;

#endif