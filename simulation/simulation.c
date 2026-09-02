/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 07:08:23 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 14:37:59 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void create_dongles_routine(t_data *data, t_coder *coders);
void create_monitor_routine(t_simulation *simulation);


int start_simulation(t_data *data, t_simulation *simulation, t_coder *coders)
{
    pthread_t   monitor;
    int i;
    
    i = 0;
    simulation->start_time = get_current_time();
    pthread_create(&monitor, NULL, &monitor_routine, simulation);
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
    pthread_join(monitor, NULL);
    return (1) ;
}

// void create_coders_routine(t_data *data, t_coder *coders)
// {
//     int i;

//     i = 0;
//     while (i < data->nb_coders)
//     {
//         pthread_create(&coders[i].thread_id, NULL, &coder_routine, &coders[i]);
//         i++;
//     }
//     i = 0;
//     while (i < data->nb_coders)
//     {
//         pthread_join(coders[i].thread_id, NULL);
//         i++;
//     }
// }

// void create_monitor_routine(t_simulation *simulation)
// {
//     pthread_t   monitor;

//     pthread_create(&monitor, NULL, &monitor_routine, simulation);
//     pthread_join(monitor, NULL);    
// }
