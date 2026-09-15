/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heapHelper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 22:51:27 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/15 11:10:16 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/*
 * @brief Checks if the scheduler is FIFO.
 * 
 * @param scheduler The scheduler to check.
 * @return 1 if the scheduler is FIFO, 0 otherwise.
 */
int	is_fifo(char *scheduler)
{
	if (strcmp(scheduler, "fifo") == 0)
		return (1);
	return (0);
}
/*
 * @brief Checks if the scheduler is EDF.
 * 
 * @param scheduler The scheduler to check.
 * @return 1 if the scheduler is EDF, 0 otherwise.
 */
int	is_edf(char *scheduler)
{
	if (strcmp(scheduler, "edf") == 0)
		return (1);
	return (0);
}
/*
 * @brief Swaps two nodes in the heap.
 * 
 * @param queue The heap structure.
 * @param current The index of the current node.
 * @param parent The index of the parent node.
 */
void	queue_swap(t_heap *queue, int current, int parent)
{
	t_heap_node	temp;

	temp = queue->nodes[parent];
	queue->nodes[parent] = queue->nodes[current];
	queue->nodes[current] = temp;
}
