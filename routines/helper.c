/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 11:17:27 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 18:09:16 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

long long get_current_time(void)
{
    struct timeval time;

    gettimeofday(&time, NULL);

    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

void ft_usleep(int milliseconds_to_sleep)
{
    usleep(milliseconds_to_sleep * 1000);
}