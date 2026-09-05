/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:10:11 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/05 15:35:09 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void grab_dongle(t_coder *coder, t_dongle *dongle)
{
    struct timespec ts;
    int waiting_res;

    waiting_res = 0;
    ts.tv_sec = ((coder->last_compile_time + coder->simulation->data->time_to_burnout) - get_current_time_ms()) / 1000;
    while (waiting_res != 110)
    {
        if (dongle->queue.nodes[0].coder->id == coder->id)
        {
            if (get_current_time_ms() < dongle->next_availability)
                ft_usleep(get_current_time_ms() - dongle->next_availability);
            print_status("has taken a dongle", coder);
            break;
        }
        waiting_res = pthread_cond_timedwait(&(dongle->signal), &(dongle->key), &ts);
    }
    pthread_mutex_unlock(&(dongle->key));
}

void request_and_grab_dongles(t_coder *coder)
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
    create_dongle_request(coder, first_dongle);
    create_dongle_request(coder, second_dongle);
    grab_dongle(coder, first_dongle);
    grab_dongle(coder, second_dongle);
}

// TODO: add next availability for dongle for cooldown
void drop_dongle(t_dongle *dongle)
{
    pop_coder(&(dongle->queue));
    pthread_cond_broadcast(&(dongle->signal));
}