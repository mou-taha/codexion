/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 10:48:12 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/15 10:42:13 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	destroy_coders(t_coder *coders, int nb_coders);
void	destroy_dongles(t_dongle *dongles, int nb_dongles);

/**
 * @brief Destroys the simulation structures and frees allocated memory.
 * 
 * @param data Pointer to the t_data structure to be destroyed.
 * @param simulation Pointer to the t_simulation structure to be destroyed.
 * @param dongles Pointer to the t_dongle structures to be destroyed.
 * @param coders Pointer to the t_coder structures to be destroyed.
 */
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

/**
 * @brief Destroys the coder structures and frees allocated memory.
 * 
 * @param coders Pointer to the t_coder structures to be destroyed.
 * @param nb_coders Number of coders to destroy.
 */
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

/**
 * @brief Destroys the dongle structures and frees allocated memory.
 * 
 * @param dongles Pointer to the t_dongle structures to be destroyed.
 * @param nb_dongles Number of dongles to destroy.
 */
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
