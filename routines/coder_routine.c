/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 07:02:26 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 13:19:40 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void grab_dongles(t_coder *coder);
void drop_dongles(t_coder *coder);
void compile(t_coder *coder);


void *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    if (coder->simulation->data->nb_coders == 1)
    {
        print_status("has taken a dongle", coder);
        ft_usleep(coder->simulation->data->time_to_burnout, coder->simulation);
        return (NULL);
    }

    coder = (t_coder *)arg;
    while (!check_stop(coder->simulation))
    {
        // 1. Grab Dongles
        grab_dongles(coder);
        
        // 2. Compile (eat)
        compile(coder);
        
        // 3. Drop Dongles
        drop_dongles(coder);

        // 4. Debug
        print_status("is debuging", coder);
        ft_usleep(coder->simulation->data->time_to_debug, coder->simulation);
                
        // 5. Refactor
        print_status("is refactoring", coder);
        ft_usleep(coder->simulation->data->time_to_refactor, coder->simulation);
    }
    return NULL;
}

void grab_dongles(t_coder *coder)
{
    t_dongle *first_dongle;
    t_dongle *second_dongle;

    if (coder->id % 2 == 0)
    {
        first_dongle = coder->right_dongle;
        second_dongle = coder->left_dongle;
    }
    else
    {
        first_dongle = coder->left_dongle;
        second_dongle = coder->right_dongle;   
    }
    pthread_mutex_lock(&(first_dongle->key));
    print_status("has taken a dongle", coder);
    pthread_mutex_lock(&(second_dongle->key));
    print_status("has taken a dongle", coder);
}

void drop_dongles(t_coder *coder)
{
    pthread_mutex_unlock(&(coder->left_dongle->key));
    pthread_cond_signal(&(coder->left_dongle->signal));
    pthread_mutex_unlock(&(coder->right_dongle->key));
    pthread_cond_signal(&(coder->right_dongle->signal));
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
    coder->last_compile_time = get_current_time();
    coder->nb_compiles += 1;
    pthread_mutex_unlock(&(coder->key));
    print_status("is compiling", coder);
    ft_usleep(coder->simulation->data->time_to_compile, coder->simulation);
}