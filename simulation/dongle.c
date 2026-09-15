/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:10:11 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/15 11:00:36 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	wait_dongle(t_dongle *dongle, t_simulation *simulation);

/**
 * @brief Grabs a dongle for a coder.
 * 
 * This function attempts to grab a dongle for the specified coder.
 * if the dongle is not available, the coder will wait until it becomes available. The function also checks if the simulation should stop during the waiting period.
 * 
 * @param coder Pointer to the coder structure.
 * @param dongle Pointer to the dongle structure.
 */
void	grab_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->key);
	while (1)
	{
		if (check_stop(coder->simulation))
		{
			pthread_mutex_unlock(&dongle->key);
			return ;
		}
		if (dongle->in_use == 0 && dongle->queue.size != 0
			&& dongle->queue.nodes[0].coder->id == coder->id)
		{
			if (get_current_time_ms() < dongle->next_availability)
			{
				wait_dongle(dongle, coder->simulation);
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

/**
 * @brief Requests and grabs dongles for a coder.
 * 
 * This function requests two dongles for the specified coder and attempts to grab them. when its done it prints a message indicating that the coder has taken both dongles.
 * 
 * @param coder Pointer to the coder structure.
 */
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

/**
 * @brief Drops a dongle.
 * 
 * This function drops the specified dongle and makes it available for other coders. and broadcasts a signal to notify waiting coders that the dongle is now available.
 * 
 * @param dongle Pointer to the dongle structure.
 * @param dongle_cooldown Cooldown period for the dongle.
 */
void	drop_dongle(t_dongle *dongle, long long dongle_cooldown)
{
	pthread_mutex_lock(&dongle->key);
	dongle->in_use = 0;
	dongle->next_availability = get_current_time_ms() + dongle_cooldown;
	pthread_cond_broadcast(&(dongle->signal));
	pthread_mutex_unlock(&dongle->key);
}
/**
 * @brief Waits for a dongle to become available.
 * 
 * This function waits for the specified dongle to become available before proceeding.
 * 
 * @param dongle Pointer to the dongle structure.
 * @param simulation Pointer to the simulation structure.
 */
void	wait_dongle(t_dongle *dongle, t_simulation *simulation)
{
	long long	wait_time;

	wait_time = dongle->next_availability - get_current_time_ms();
	pthread_mutex_unlock(&dongle->key);
	ft_usleep(wait_time, simulation);
	pthread_mutex_lock(&dongle->key);
}
