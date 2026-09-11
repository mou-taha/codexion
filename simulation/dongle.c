/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:10:11 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/07 22:28:59 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"


void	grab_dongle(t_coder *coder, t_dongle *dongle)
{
	long long	wait_time;

	pthread_mutex_lock(&dongle->key);
	while (1)
	{
		if (dongle->in_use == 0 && dongle->queue.size != 0
			&& dongle->queue.nodes[0].coder->id == coder->id)
		{
			if (get_current_time_ms() < dongle->next_availability)
			{
				wait_time = dongle->next_availability - get_current_time_ms();
				pthread_mutex_unlock(&dongle->key);
				ft_usleep(wait_time, coder->simulation);
				pthread_mutex_lock(&dongle->key);
				continue ;
			}
			dongle->in_use = 1;
			pop_coder(&dongle->queue);
			pthread_mutex_unlock(&dongle->key);
			return ;
		}
		pthread_cond_wait(&dongle->signal, &dongle->key);
	}
}

void	request_and_grab_dongles(t_coder *coder)
{
	t_dongle	*first_dongle;
	t_dongle	*second_dongle;

	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		first_dongle = coder->left_dongle;
		second_dongle = coder->right_dongle;
	}
	else
	{
		first_dongle = coder->right_dongle;
		second_dongle = coder->left_dongle;
	}
	create_dongle_request(coder, first_dongle);
	create_dongle_request(coder, second_dongle);
	grab_dongle(coder, first_dongle);
	grab_dongle(coder, second_dongle);
	print_status("has taken a dongle", coder);
	print_status("has taken a dongle", coder);
}

void	drop_dongle(t_dongle *dongle, long long dongle_cooldown)
{
	pthread_mutex_lock(&dongle->key);
	dongle->in_use = 0;
	dongle->next_availability = get_current_time_ms() + dongle_cooldown;
	pthread_cond_broadcast(&(dongle->signal));
	pthread_mutex_unlock(&dongle->key);
}
