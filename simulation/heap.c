/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 20:47:04 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/15 11:04:30 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int			should_swap(char *scheduler, t_heap_node parent, t_heap_node child);

/*
 * @brief Initializes a heap.
 * 
 * This function initializes a heap with the specified capacity and scheduler. allocates memory for the heap nodes and sets the initial size to zero.
 * 
 * @param capacity The maximum number of elements the heap can hold.
 * @param scheduler The scheduling algorithm to use.
 * @return The initialized heap.
 */
t_heap	init_heap(int capacity, char *scheduler)
{
	t_heap	heap;

	heap.capacity = capacity;
	heap.nodes = malloc(sizeof(t_heap_node) * capacity);
	heap.size = 0;
	heap.scheduler = scheduler;
	return (heap);
}
/*
 * @brief Inserts a request into the heap.
 * 
 * This function inserts a request into the heap, maintaining the heap property based on the specified scheduling algorithm. If the heap is full, the insertion fails.
 * 
 * @param queue Pointer to the heap structure.
 * @param request The request to be inserted.
 * @return 1 if the insertion is successful, 0 if the heap is full.
*/
int	insert_to_heap(t_heap *queue, t_heap_node request)
{
	int	current;
	int	parent;

	if (queue->capacity == queue->size)
		return (0);
	queue->nodes[queue->size] = request;
	queue->size++;
	current = queue->size - 1;
	parent = (current - 1) / 2;
	while (current > 0 && should_swap(queue->scheduler, queue->nodes[parent],
			queue->nodes[current]))
	{
		queue_swap(queue, current, parent);
		current = parent;
		parent = (current - 1) / 2;
	}
	return (1);
}

/*
 * @brief Gets the burnout time for a heap node.
 * 
 * This function calculates the burnout time for a given heap node based on its coder's last compile time and the simulation data.
 * 
 * @param node The heap node for which to calculate the burnout time.
 * @return The calculated burnout time.
 */
long long	get_burnout_time(t_heap_node node)
{
	long long	burnout_time;

	pthread_mutex_lock(&node.coder->key);
	burnout_time = node.coder->last_compile_time
		+ node.coder->simulation->data->time_to_burnout;
	pthread_mutex_unlock(&node.coder->key);
	return (burnout_time);
}

/*
 * @brief Determines if two heap nodes should be swapped.
 * 
 * This function checks if the parent and child nodes should be swapped based on the specified scheduling algorithm.
 * 
 * @param scheduler The scheduling algorithm to use.
 * @param parent The parent heap node.
 * @param child The child heap node.
 * @return 1 if the nodes should be swapped, 0 otherwise.
 */
int	should_swap(char *scheduler, t_heap_node parent, t_heap_node child)
{
	long long	parent_burnout_time;
	long long	child_burnout_time;

	if (is_fifo(scheduler))
	{
		if (child.request_time < parent.request_time)
			return (1);
		return (0);
	}
	else if (is_edf(scheduler))
	{
		parent_burnout_time = get_burnout_time(parent);
		child_burnout_time = get_burnout_time(child);
		if (child_burnout_time < parent_burnout_time)
			return (1);
		else if (child_burnout_time == parent_burnout_time
			&& child.request_id < parent.request_id)
			return (1);
		return (0);
	}
	return (0);
}

/*
 * @brief Removes the root node from the heap.
 * 
 * This function removes the root node from the heap, maintaining the heap property based on the specified scheduling algorithm.
 * 
 * @param queue Pointer to the heap structure.
 */
void	pop_coder(t_heap *queue)
{
	int	current;
	int	child;

	if (!queue->size)
		return ;
	queue->nodes[0] = queue->nodes[--queue->size];
	current = 0;
	while (2 * current + 1 < queue->size)
	{
		child = 2 * current + 1;
		if (child + 1 < queue->size && should_swap(queue->scheduler,
				queue->nodes[child], queue->nodes[child + 1]))
			child++;
		if (!should_swap(queue->scheduler,
				queue->nodes[child],
				queue->nodes[current]))
			break ;
		queue_swap(queue, current, child);
		current = child;
	}
}
