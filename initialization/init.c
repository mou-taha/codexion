/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 06:58:12 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/15 10:41:45 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int	init_simulation(t_data *data, t_simulation *simulation, t_coder *coders);
int	init_dongles(t_dongle **dongles, int nb_dongles, char *scheduler);
int	init_coders(t_coder **coders, t_dongle **dongles, t_simulation *simulation,
		int nb_coders);

/**
 * @brief Initializes the simulation by setting up dongles, coders, and the simulation structure.
 * 
 * This function initializes the simulation by creating and configuring the necessary dongles and coders based on the provided simulation parameters. It also sets up the simulation structure with the relevant data and coders.
 * 
 * @param data Pointer to the t_data structure containing simulation parameters.
 * @param simulation Pointer to the t_simulation structure to be initialized.
 * @param dongles Pointer to a pointer of t_dongle structures to be initialized.
 * @param coders Pointer to a pointer of t_coder structures to be initialized.
 */
int	init(t_data *data, t_simulation *simulation, t_dongle **dongles,
		t_coder **coders)
{
	if (!init_dongles(dongles, data->nb_coders, data->scheduler))
	{
		return (0);
	}
	if (!init_coders(coders, dongles, simulation, data->nb_coders))
	{
		return (0);
	}
	if (!init_simulation(data, simulation, *coders))
	{
		return (0);
	}
	return (1);
}

/**
 * @brief Initializes the simulation structure with the provided data and coders.
 * 
 * @param data Pointer to the t_data structure containing simulation parameters.
 * @param simulation Pointer to the t_simulation structure to be initialized.
 * @param coders Pointer to the t_coder structures to be associated with the simulation.
 * @return Returns 1 on successful initialization, 0 on failure.
 *
 */
int	init_simulation(t_data *data, t_simulation *simulation, t_coder *coders)
{
	if (data)
	{
		if (simulation)
		{
			simulation->stop_simulation = 0;
			simulation->data = data;
			simulation->coders = coders;
			pthread_mutex_init(&simulation->print_key, NULL);
			pthread_mutex_init(&simulation->stop_simulation_key, NULL);
			return (1);
		}
	}
	printf("\n Error while initializing simulation\n");
	return (0);
}

/**
 * @brief Initializes the dongle structures with the provided parameters.
 * 
 * @param dongles Pointer to a pointer of t_dongle structures to be initialized.
 * @param nb_dongles Number of dongles to initialize.
 * @param scheduler Pointer to the scheduler string.
 * @return Returns 1 on successful initialization, 0 on failure.
 */
int	init_dongles(t_dongle **dongles, int nb_dongles, char *scheduler)
{
	int	i;

	i = 0;
	*dongles = malloc(sizeof(t_dongle) * nb_dongles);
	if (*dongles != NULL)
	{
		while (i < nb_dongles)
		{
			(*dongles)[i].id = i + 1;
			(*dongles)[i].nb_coder = 0;
			(*dongles)[i].in_use = 0;
			(*dongles)[i].next_availability = 0;
			(*dongles)[i].request_counter = 0;
			(*dongles)[i].queue = init_heap(2, scheduler);
			pthread_mutex_init(&((*dongles)[i].key), NULL);
			pthread_cond_init(&(*dongles)[i].signal, NULL);
			i++;
		}
		return (1);
	}
	printf("\n Error while initializing dongles\n");
	return (0);
}

/**
 * @brief Initializes the coder structures with the provided parameters.
 * 
 * @param coders Pointer to a pointer of t_coder structures to be initialized.
 * @param dongles Pointer to a pointer of t_dongle structures to be associated with the coders.
 * @param simulation Pointer to the t_simulation structure to be associated with the coders.
 * @param nb_coders Number of coders to initialize.
 * @return Returns 1 on successful initialization, 0 on failure.
 */
int	init_coders(t_coder **coders, t_dongle **dongles, t_simulation *simulation,
		int nb_coders)
{
	int	i;

	i = 0;
	*coders = malloc(sizeof(t_coder) * nb_coders);
	if (*coders != NULL)
	{
		while (i < nb_coders)
		{
			(*coders)[i].id = i + 1;
			(*coders)[i].nb_compiles = 0;
			(*coders)[i].is_compiling = 0;
			(*coders)[i].simulation = simulation;
			(*coders)[i].last_compile_time = get_current_time_ms();
			(*coders)[i].left_dongle = &(*dongles)[i];
			(*coders)[i].right_dongle = &(*dongles)[(i + 1) % nb_coders];
			pthread_mutex_init(&((*coders)[i].key), NULL);
			i++;
		}
		return (1);
	}
	printf("\n Error while initializing coders\n");
	return (0);
}
