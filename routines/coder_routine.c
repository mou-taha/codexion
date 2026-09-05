/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 07:02:26 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/05 13:27:23 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void grab_dongles(t_coder *coder);
void compile(t_coder *coder);

void *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    if (coder->simulation->data->nb_coders == 1)
    {
        print_status("has taken a dongle", coder);
        ft_usleep(coder->simulation->data->time_to_burnout);
        return (NULL);
    }

    coder = (t_coder *)arg;
    while (!check_stop(coder->simulation) && !is_coder_finished(coder))
    {
        request_and_grab_dongles(coder);

        compile(coder);

        drop_dongle(coder->right_dongle);
        drop_dongle(coder->left_dongle);

        print_status("is debuging", coder);
        ft_usleep(coder->simulation->data->time_to_debug);

        print_status("is refactoring", coder);
        ft_usleep(coder->simulation->data->time_to_refactor);
    }
    coder->simulation->stop_simulation = 1;
    return NULL;
}

int check_stop(t_simulation *simulation)
{
    int stop_value;
    pthread_mutex_lock(&(simulation->stop_simulation_key));
    stop_value = simulation->stop_simulation;
    pthread_mutex_unlock(&(simulation->stop_simulation_key));
    return stop_value;
}

void compile(t_coder *coder)
{
    pthread_mutex_lock(&(coder->key));
    coder->last_compile_time = get_current_time_ms();
    coder->nb_compiles += 1;
    pthread_mutex_unlock(&(coder->key));
    print_status("is compiling", coder);
    ft_usleep(coder->simulation->data->time_to_compile);
}