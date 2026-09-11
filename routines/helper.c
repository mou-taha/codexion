/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 11:17:27 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/10 22:41:56 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

long long	get_current_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void	ft_usleep(int milliseconds_to_sleep, t_simulation *simulation)
{
	long long	time_to_wait;

	time_to_wait = get_current_time_ms() + milliseconds_to_sleep;
	while (get_current_time_ms() <= time_to_wait && !check_stop(simulation))
	{
		usleep(500);
	}
}

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
