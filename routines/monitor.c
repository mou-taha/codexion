/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 12:37:16 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 15:04:02 by tmousnia         ###   ########.fr       */
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
            if (check_burnout(&sim->coders[i]))
            {
                kill_coder(&sim->coders[i]);
                return (NULL);
            }
            i++;
        }
        usleep(1000);
    }
    return (NULL);
}

int check_burnout(t_coder *coder)
{
    long long elapsed;

    pthread_mutex_lock(&coder->key);
    elapsed = get_current_time() - coder->last_compile_time;
    pthread_mutex_unlock(&coder->key);
    return (elapsed >= coder->simulation->data->time_to_burnout);
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
    pthread_mutex_unlock(&coder->simulation->print_key);
}