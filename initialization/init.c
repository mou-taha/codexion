/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 06:58:12 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/03 21:22:29 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int init_simulation(t_data *data, t_simulation *simulation, t_coder *coders);
int init_dongles(t_dongle **dongles, int nb_dongles, char *scheduler);
int init_coders(t_coder **coders, t_dongle **dongles, t_simulation *simulation, int nb_coders);

int init(t_data *data, t_simulation *simulation, t_dongle **dongles, t_coder **coders)
{
    if (!init_dongles(dongles, data->nb_coders, data->scheduler))
    {
        return 0;
    }
    if (!init_coders(coders, dongles, simulation, data->nb_coders))
    {
        return 0;
    }
    if (!init_simulation(data, simulation, *coders))
    {
        return 0;
    }
    return (1);
}

int init_simulation(t_data *data, t_simulation *simulation, t_coder *coders)
{
    if (data)
    {
        if (simulation)
        {
            simulation->stop_simulation = 0;
            simulation->data = data;
            simulation->coders = coders;
            pthread_mutex_init(&simulation->print_key, NULL);
            pthread_mutex_init(&simulation->stop_simulation_key, NULL);
            return (1);
        }
    }
    printf("\n Error while initializing simulation\n");
    return (0);
}

int init_dongles(t_dongle **dongles, int nb_dongles, char *scheduler)
{
    int i;

    i = 0;
    *dongles = malloc(sizeof(t_dongle) * nb_dongles);
    if (*dongles != NULL)
    {
        while (i < nb_dongles)
        {
            (*dongles)[i].id = i + 1;
            (*dongles)[i].nb_coder = 0;
            (*dongles)[i].next_availability = 0;
            (*dongles)[i].queue = init_heap(2, scheduler);
            pthread_mutex_init(&((*dongles)[i].key), NULL);
            pthread_cond_init(&(*dongles)[i].signal, NULL);
            i++;
        }
        return (1);
    }
    printf("\n Error while initializing dongles\n");
    return (0);
}

int init_coders(t_coder **coders, t_dongle **dongles, t_simulation *simulation, int nb_coders)
{
    int i;

    i = 0;
    *coders = malloc(sizeof(t_coder) * nb_coders);
    if (*coders != NULL)
    {
        while (i < nb_coders)
        {
            (*coders)[i].id = i + 1;
            (*coders)[i].nb_compiles = 0;
            (*coders)[i].simulation = simulation;
            (*coders)[i].left_dongle = &(*dongles)[i];
            (*coders)[i].right_dongle = &(*dongles)[(i + 1) % nb_coders];
            pthread_mutex_init(&((*coders)[i].key), NULL);
            i++;
        }
        return (1);
    }
    printf("\n Error while initializing coders\n");
    return (0);
}
