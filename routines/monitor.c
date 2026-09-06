/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 12:37:16 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/06 22:19:27 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int		check_burnout(t_coder *coder);
void	kill_coder(t_coder *coder);

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	int				are_all_coders_finished;
	int				i;

	sim = (t_simulation *)arg;
	are_all_coders_finished = 0;
	while (!check_stop(sim))
	{
		i = 0;
		while (i < sim->data->nb_coders)
		{
			are_all_coders_finished = is_coder_finished(&sim->coders[i]);
			if (check_burnout(&sim->coders[i]))
			{
				kill_coder(&sim->coders[i]);
				return (NULL);
			}
			i++;
		}
		if (are_all_coders_finished)
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}

/*
check if a coder is going to burnout base 
on his last compile and time to burnout,
but the coder mustn't be in compiling and 
must not finished yet their required compiles
*/
int	check_burnout(t_coder *coder)
{
	long long	elapsed;

	pthread_mutex_lock(&coder->key);
	elapsed = get_current_time_ms() - coder->last_compile_time;
	pthread_mutex_unlock(&coder->key);
	return (elapsed >= coder->simulation->data->time_to_burnout
		&& !is_coder_finished(coder) && !coder->is_compiling);
}

int	is_coder_finished(t_coder *coder)
{
	int	coder_nb_compiles;

	pthread_mutex_lock(&(coder->key));
	coder_nb_compiles = coder->nb_compiles;
	pthread_mutex_unlock(&(coder->key));
	return (coder_nb_compiles
		>= coder->simulation->data->number_of_compiles_required);
}

void	kill_coder(t_coder *coder)
{
	long long	interval;

	pthread_mutex_lock(&(coder->simulation->print_key));
	pthread_mutex_lock(&(coder->simulation->stop_simulation_key));
	coder->simulation->stop_simulation = 1;
	pthread_mutex_unlock(&(coder->simulation->stop_simulation_key));
	interval = get_current_time_ms() - coder->simulation->start_time;
	printf("%lld %d burned out\n", interval, coder->id);
	pthread_mutex_unlock(&coder->simulation->print_key);
}
