/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 10:48:12 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/06 22:07:27 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	destroy_coders(t_coder *coders, int nb_coders);
void	destroy_dongles(t_dongle *dongles, int nb_dongles);

void	destroy(t_data *data, t_simulation *simulation, t_dongle *dongles,
		t_coder *coders)
{
	destroy_coders(coders, data->nb_coders);
	destroy_dongles(dongles, data->nb_coders);
	if (simulation)
	{
		pthread_mutex_destroy(&(simulation->stop_simulation_key));
		pthread_mutex_destroy(&(simulation->print_key));
	}
	if (dongles)
		free(dongles);
	if (coders)
		free(coders);
	if (data)
		free(data);
}

void	destroy_coders(t_coder *coders, int nb_coders)
{
	int	i;

	i = 0;
	while (coders && i < nb_coders)
	{
		pthread_mutex_destroy(&(coders[i].key));
		i++;
	}
}

void	destroy_dongles(t_dongle *dongles, int nb_dongles)
{
	int	i;

	i = 0;
	while (dongles && i < nb_dongles)
	{
		pthread_mutex_destroy(&(dongles[i].key));
		pthread_cond_destroy(&(dongles[i].signal));
		free(dongles[i].queue.nodes);
		i++;
	}
}
