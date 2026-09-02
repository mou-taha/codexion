/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 06:58:12 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 06:58:13 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int init_simulation(t_data *data, t_simulation *simulation);
int init_dongles(t_dongle **dongles, int nb_dongles);
int init_coders(t_coder **coders, t_dongle **dongles, t_simulation *simulation, int nb_coders);

int init(t_data *data, t_simulation *simulation, t_dongle **dongles, t_coder **coders)
{
    if (init_simulation(data, simulation))
    {
        if (init_dongles(dongles, data->nb_coders))
        {
            if (init_coders(coders, dongles, simulation, data->nb_coders))
            {
                return (1);
            }
        }
    }
    return (0);
}

int init_simulation(t_data *data, t_simulation *simulation)
{
    if (data)
    {
        if (simulation)
        {
            simulation->stop_simulation = 0;
            simulation->data = data;
            pthread_mutex_init(&simulation->print_key, NULL);
            pthread_mutex_init(&simulation->stop_simulation_key, NULL);
            return (1);
        }
    }
    printf("\n Error while initializing simulation\n");
    return (0);
}

int init_dongles(t_dongle **dongles, int nb_dongles)
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

void exit_free(t_data *data, t_simulation *simulation, t_dongle *dongles, t_coder *coders)
{
    int i;

    i = 0;
    while (i < data->nb_coders)
    {
        pthread_mutex_destroy(&(coders[i].key));
        pthread_mutex_destroy(&(dongles[i].key));
        pthread_cond_destroy(&(dongles[i].signal));
        i++;
    }
    pthread_mutex_destroy(&(simulation->stop_simulation_key));
    pthread_mutex_destroy(&(simulation->print_key));
    free(dongles);
    free(coders);
    free(data);
}