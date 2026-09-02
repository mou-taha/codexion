/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 12:50:26 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 12:50:38 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void print_status(char *status, t_coder *coder)
{
    int interval;
    
    pthread_mutex_lock(&(coder->simulation->print_key));
    if (!check_stop(coder->simulation))
    {
        interval = get_current_time() - coder->simulation->start_time;
        printf("%d %d %s\n", interval, coder->id, status);
    }
    pthread_mutex_unlock(&(coder->simulation->print_key));
}