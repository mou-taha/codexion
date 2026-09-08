/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 20:47:04 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/08 20:51:13 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int		should_swap(char *scheduler, t_heap_node parent, t_heap_node child);

t_heap	init_heap(int capacity, char *scheduler)
{
	t_heap	heap;

	heap.capacity = capacity;
	heap.nodes = malloc(sizeof(t_heap_node) * capacity);
	heap.size = 0;
	heap.scheduler = scheduler;
	return (heap);
}

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
		pthread_mutex_lock(&parent.coder->key);
		parent_burnout_time = parent.coder->last_compile_time
			+ parent.coder->simulation->data->time_to_burnout;
		pthread_mutex_unlock(&parent.coder->key);
		pthread_mutex_lock(&child.coder->key);
		child_burnout_time = child.coder->last_compile_time
			+ child.coder->simulation->data->time_to_burnout;
		pthread_mutex_unlock(&child.coder->key);
		if (child_burnout_time < parent_burnout_time)
			return (1);
		else if (child_burnout_time == parent_burnout_time
				&& child.request_id < parent.request_id)
			return (1);
		return (0);
	}
	return (0);
}

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
