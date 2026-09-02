/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 07:08:23 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 15:34:58 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


//TODO: One later issue remains: coders can block forever inside pthread_mutex_lock() while waiting for dongles after the monitor sets the stop flag. That will need to be replaced with condition-variable scheduling, but first restore the missing pthread_create() loop.
#include "../codexion.h"

void create_dongles_routine(t_data *data, t_coder *coders);
void create_monitor_routine(t_simulation *simulation);


int start_simulation(t_data *data, t_simulation *simulation, t_coder *coders)
{
    pthread_t   monitor;
    int i;
    
    simulation->start_time = get_current_time();

    i = 0;
    while (i < data->nb_coders)
    {
        pthread_mutex_lock(&coders[i].key);
        coders[i].last_compile_time = simulation->start_time;
        pthread_mutex_unlock(&coders[i].key);
        if (pthread_create(&coders[i].thread_id, NULL,
            &coder_routine, &coders[i]) != 0)
        return (0);
        i++;
    }

    if (pthread_create(&monitor, NULL, &monitor_routine, simulation) != 0)
        return (0);

    i = 0;
    while (i < data->nb_coders)
    {
        if (pthread_join(coders[i].thread_id, NULL) != 0)
            return (0);
        i++;
    }
    if (pthread_join(monitor, NULL) != 0)
        return (0);
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
