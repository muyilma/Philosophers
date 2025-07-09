#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

# include <pthread.h>
# include <sys/time.h>

typedef struct s_philo
{
	int				thread_no;
	long			eat;
	long			sleep;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	long			last_meal_time;
	int				is_dead;
	void			*exit_code;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	death_mutex;
	struct timeval	start_time;
}					t_philo;

int				thread_start(t_philo **philo, int total_thread);
long				get_ms(t_philo *philo);
void				print_status(t_philo *philo, char *status);
void				eating(t_philo *philo);
void				sleeping(t_philo *philo);
void				thinking(t_philo *philo);
int					check_death(t_philo *philo);

#endif 
