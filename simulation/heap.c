/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 20:47:04 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/03 23:34:20 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"
// TODO: test heap implementation and check leaks (possible in nodes, already freed in destroy())
t_heap init_heap(int capacity, char *scheduler)
{
	t_heap heap;

	heap.capacity = capacity;
	heap.nodes = malloc(sizeof(t_heap_node) * 2);
	heap.size = 0;
	heap.scheduler = scheduler;
	return (heap);
}

int insert_to_heap(t_heap *queue, t_heap_node request)
{
	if (queue->capacity == queue->size)
		return (0);

	queue->nodes[queue->size] = request;
	queue->size++;

	int current = queue->size - 1;
	int parent = (current - 1) / 2;

	while (current > 0 && should_swap(queue->scheduler, queue->nodes[parent], request))
	{
		queue_swap(queue, current, parent);
		parent = (current - 1) / 2;
	}
}

int should_swap(char *scheduler, t_heap_node parent, t_heap_node child)
{
	if (is_fifo(scheduler))
	{
		if (child.priority < parent.priority)
			return (1);
		return (0);
	}
	else if (is_edf(scheduler))
	{
		int parent_burnout_time = parent.coder->last_compile_time + parent.coder->simulation->data->time_to_burnout;
		int child_burnout_time = child.coder->last_compile_time + child.coder->simulation->data->time_to_burnout;

		if (child_burnout_time > parent_burnout_time)
			return (1);
		else if (child_burnout_time == parent_burnout_time && child.request_id <= parent.request_id)
			return (1);
		return (0);
	}
}

t_heap_node *get_first_coder(t_heap *queue)
{
	t_heap_node notFound;
	int smallest_child;

	notFound.request_id = -1;
	if (queue->size == 0)
		return (&notFound);
	t_heap_node *first_coder = &(queue->nodes[0]);
	queue->nodes[0] = queue->nodes[queue->size - 1];
	int current = 0;

	while (((2 * current) + 1) < queue->size)
	{
		smallest_child = (2 * current) + 1;
		if ((2 * current) + 2 < queue->size && should_swap(queue, queue->nodes[smallest_child], queue->nodes[(2 * current) + 2]))
			smallest_child = (2 * current) + 2;
		if (should_swap(queue, queue->nodes[smallest_child], queue->nodes[current]))
		{
			queue_swap(queue, current, smallest_child);
		}
		else
			break;
	}
	return (first_coder);
}
