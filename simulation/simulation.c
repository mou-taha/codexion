/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 07:08:23 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/06 23:23:38 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int	create_coders(t_coder *coders, int nb_coders, long long start_time);

int	start_simulation(t_data *data, t_simulation *simulation, t_coder *coders)
{
	pthread_t	monitor;
	int			i;

	i = 0;
	simulation->start_time = get_current_time_ms();
	create_coders(coders, data->nb_coders, simulation->start_time);
	if (pthread_create(&monitor, NULL, &monitor_routine, simulation) != 0)
		return (0);
	i = 0;
	while (i < data->nb_coders)
	{
		if (pthread_join(coders[i].thread_id, NULL) != 0)
			return (0);
		i++;
	}
	if (pthread_join(monitor, NULL) != 0)
		return (0);
	return (1);
}

int	create_coders(t_coder *coders, int nb_coders, long long start_time)
{
	int	i;

	i = 0;
	while (i < nb_coders)
	{
		pthread_mutex_lock(&coders[i].key);
		coders[i].last_compile_time = start_time;
		pthread_mutex_unlock(&coders[i].key);
		if (pthread_create(&coders[i].thread_id, NULL, &coder_routine,
				&coders[i]) != 0)
			return (0);
		i++;
	}
	return (1);
}
