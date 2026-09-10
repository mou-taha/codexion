/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 07:02:26 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/10 18:22:51 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	compile(t_coder *coder);
void	do_my_routine(t_coder *coder);

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->simulation->data->nb_coders == 1)
	{
		print_status("has taken a dongle", coder);
		ft_usleep(coder->simulation->data->time_to_burnout, coder->simulation);
		return (NULL);
	}
	if (coder->id % 2 == 0)
		ft_usleep(coder->simulation->data->time_to_compile / 2,
			coder->simulation);
	while (1)
	{
		if (check_stop(coder->simulation) || is_coder_finished(coder))
			return (NULL);
		do_my_routine(coder);
	}
	return (NULL);
}

int	check_stop(t_simulation *simulation)
{
	int	stop_value;

	pthread_mutex_lock(&(simulation->stop_simulation_key));
	stop_value = simulation->stop_simulation;
	pthread_mutex_unlock(&(simulation->stop_simulation_key));
	return (stop_value);
}

void	compile(t_coder *coder)
{
	pthread_mutex_lock(&(coder->key));
	coder->last_compile_time = get_current_time_ms();
	coder->nb_compiles += 1;
	coder->is_compiling = 1;
	pthread_mutex_unlock(&(coder->key));
	print_status("is compiling", coder);
	ft_usleep(coder->simulation->data->time_to_compile, coder->simulation);
	pthread_mutex_lock(&(coder->key));
	coder->is_compiling = 0;
	pthread_mutex_unlock(&(coder->key));
}

void	do_my_routine(t_coder *coder)
{
	request_and_grab_dongles(coder);
	compile(coder);
	drop_dongle(coder->right_dongle,
		coder->simulation->data->dongle_cooldown);
	drop_dongle(coder->left_dongle,
		coder->simulation->data->dongle_cooldown);
	print_status("is debugging", coder);
	ft_usleep(coder->simulation->data->time_to_debug, coder->simulation);
	print_status("is refactoring", coder);
	ft_usleep(coder->simulation->data->time_to_refactor, coder->simulation);
}
