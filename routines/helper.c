/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 11:17:27 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/15 10:56:23 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * @brief Gets the current time in milliseconds.
 * 
 * This function retrieves the current time and converts it to milliseconds.
 * 
 * @return The current time in milliseconds.
 */
long long	get_current_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}
/**
 * @brief Sleeps for a specified number of milliseconds.
 * 
 * This function sleeps for the specified number of milliseconds, checking the simulation stop condition periodically, if its stoped the sleep is interrupted.
 * 
 * @param milliseconds_to_sleep The number of milliseconds to sleep.
 * @param simulation Pointer to the simulation structure.
 */
void	ft_usleep(int milliseconds_to_sleep, t_simulation *simulation)
{
	long long	time_to_wait;

	time_to_wait = get_current_time_ms() + milliseconds_to_sleep;
	while (get_current_time_ms() <= time_to_wait && !check_stop(simulation))
	{
		usleep(500);
	}
}
/**
 * @brief Creates a dongle request.
 * 
 * This function creates a request for a dongle, adding it to the dongle's request queue.
 * 
 * @param coder Pointer to the coder structure.
 * @param dongle Pointer to the dongle structure.
 */
void	create_dongle_request(t_coder *coder, t_dongle *dongle)
{
	t_heap_node	request;

	pthread_mutex_lock(&(dongle->key));
	request.coder = coder;
	request.request_time = get_current_time_ms();
	request.request_id = dongle->request_counter++;
	insert_to_heap(&(dongle->queue), request);
	pthread_mutex_unlock(&(dongle->key));
}
