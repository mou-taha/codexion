/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 11:17:27 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 12:37:01 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

long long get_current_time(void)
{
    struct timeval time;

    gettimeofday(&time, NULL);
    
    return (time.tv_sec *1000 )+ (time.tv_usec/1000);
}

void ft_usleep(int milliseconds_to_sleep, t_simulation *simulation)
{
    long long current_time = get_current_time();
    
    while (get_current_time() <= current_time + milliseconds_to_sleep && !check_stop(simulation))
    {
        usleep(500);   
    }
}