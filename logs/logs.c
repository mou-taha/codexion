/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 12:50:26 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/15 10:45:50 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * @brief Prints the status of a coder.
 * 
 * this function prints the status of a coder, including the time interval since the simulation started, the coder's ID, and the provided status message.
 * It ensures that the printing is thread-safe by locking the print mutex before printing and unlocking it afterward.
 * If the simulation has been stopped, it will not print any status messages.
 * 
 * @param status The status message to print.
 * @param coder Pointer to the t_coder structure.
 */
void	print_status(char *status, t_coder *coder)
{
	int	interval;

	interval = get_current_time_ms() - coder->simulation->start_time;
	pthread_mutex_lock(&(coder->simulation->print_key));
	if (!check_stop(coder->simulation))
	{
		printf("%d %d %s\n", interval, coder->id, status);
	}
	pthread_mutex_unlock(&(coder->simulation->print_key));
}
