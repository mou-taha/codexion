/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 07:08:23 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 07:16:02 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int start_dongles_simulation(t_data *data, t_coder *coders);

int start_simulation(t_data *data, t_simulation *simulation, t_dongle *dongles, t_coder *coders)
{
}

int start_dongles_simulation(t_data *data, t_coder *coders)
{
    int i;

    i = 0;
    while (i < data->nb_coders)
    {
        pthread_create(&coders[i].thread_id, NULL, &coder_routine, &coders[i]);
        i++;
    }
    i = 0;
    while (i < data->nb_coders)
    {
        pthread_join(coders[i].thread_id, NULL);
        i++;
    }
}