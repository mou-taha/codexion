/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heapHelper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 22:51:27 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/03 23:30:09 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int is_fifo(char *scheduler)
{
    if (strcmp(scheduler, "fifo") == 0)
        return (1);
    return (0);
}

int is_edf(char *scheduler)
{
    if (strcmp(scheduler, "edf") == 0)
        return (1);
    return (0);
}

void queue_swap(t_heap *queue, int current, int parent)
{
    t_heap_node temp;

    temp = queue->nodes[parent];
    queue->nodes[parent] = queue->nodes[current];
    queue->nodes[current] = temp;
    current = parent;
}