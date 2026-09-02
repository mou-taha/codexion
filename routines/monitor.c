/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 12:37:16 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 13:20:06 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int check_burnout(t_coder *coder);
void kill_coder(t_coder *coder);


void *monitor_routine(void *arg)
{
    t_simulation *sim;
    int i;

    sim = (t_simulation *)arg;
    while (!check_stop(sim))
    {
        i = 0;
        while (i < sim->data->nb_coders)
        {
            if (check_burnout(&(sim->coders[i])))
            {
                kill_coder(&sim->coders[i]);
                return (NULL);
            }
            i++;
        }
        usleep(1000); // Short sleep to prevent 100% CPU usage
    }
    return (NULL);
}

int check_burnout(t_coder *coder)
{
    int res;

    res = 0;
    pthread_mutex_lock(&(coder->key));
    res = get_current_time() - coder->last_compile_time; 
    pthread_mutex_unlock(&(coder->key));
    if (res > coder->simulation->data->time_to_burnout)
        return (1);
    return (0);
}

void kill_coder(t_coder *coder)
{
    long long interval;

    pthread_mutex_lock(&(coder->simulation->print_key));
    pthread_mutex_lock(&(coder->simulation->stop_simulation_key));
    coder->simulation->stop_simulation = 1;
    pthread_mutex_unlock(&(coder->simulation->stop_simulation_key));
    interval = get_current_time() - coder->simulation->start_time;
    printf("%lld %d burned out\n", interval, coder->id);
}