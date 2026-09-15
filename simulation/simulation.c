/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 07:08:23 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/15 11:11:27 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int	create_coders(t_coder *coders, int nb_coders, long long start_time);

/**
 * @brief Starts the simulation.
 * 
 * @param data The simulation data.
 * @param simulation The simulation structure.
 * @param coders The array of coders.
 * @return 1 if the simulation starts successfully, 0 otherwise.
 */
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

/**
 * @brief Creates the coder threads.
 * 
 * @param coders The array of coders.
 * @param nb_coders The number of coders.
 * @param start_time The start time of the simulation.
 * @return 1 if the coders are created successfully, 0 otherwise.
 */
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
