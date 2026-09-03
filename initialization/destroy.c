/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 10:48:12 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/03 07:03:13 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void destroy(t_data *data, t_simulation *simulation, t_dongle *dongles, t_coder *coders)
{
    int i;

    if (!data)
        return;
    i = 0;
    while (coders && i < data->nb_coders)
    {
        pthread_mutex_destroy(&(coders[i].key));
        i++;
    }
    i = 0;
    while (dongles && i < data->nb_coders)
    {
        pthread_mutex_destroy(&(dongles[i].key));
        pthread_cond_destroy(&(dongles[i].signal));
        free(dongles[i].queue.nodes);
        i++;
    }
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
