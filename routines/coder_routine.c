/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 07:02:26 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/15 10:55:19 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	grab_dongles(t_coder *coder);
void	compile(t_coder *coder);
void	do_my_routine(t_coder *coder);

/**
 * @brief The main routine for each coder thread.
 * 
 * This function represents the main logic for each coder thread, handling their actions and interactions within the simulation.
 * 
 * @param arg The argument passed to the thread (should be a pointer to a t_coder structure).
 * @return NULL.
 */
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
	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		usleep(100);
	pthread_mutex_lock(&(coder->key));
	coder->last_compile_time = get_current_time_ms();
	pthread_mutex_unlock(&(coder->key));
	while (1)
	{
		if (check_stop(coder->simulation) || is_coder_finished(coder))
			return (NULL);
		do_my_routine(coder);
	}
	return (NULL);
}

/**
 * @brief Checks if the simulation should stop.
 * 
 * this function checks if the simulation should stop by acquiring a lock on the stop_simulation_key mutex, reading the stop_simulation value, and then releasing the lock.
 * 
 * @param simulation Pointer to the simulation structure.
 * @return 1 if the simulation should stop, 0 otherwise.
 */
int	check_stop(t_simulation *simulation)
{
	int	stop_value;

	pthread_mutex_lock(&(simulation->stop_simulation_key));
	stop_value = simulation->stop_simulation;
	pthread_mutex_unlock(&(simulation->stop_simulation_key));
	return (stop_value);
}

/**
 * @brief coder compile logic.
 * 
 * This function handles the compilation process for a coder, updating their last compile time, incrementing the number of compiles, and simulating the time taken to compile.
 * 
 * @param coder Pointer to the coder structure.
 */
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

/**
 * @brief Performs the main routine for the coder.
 * 
 * This function handles the main logic for each coder thread, including requesting and grabbing dongles, compiling code, and dropping dongles.
 * 
 * @param coder Pointer to the coder structure.
 */
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
