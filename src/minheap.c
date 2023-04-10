#include "minheap.h"
#include <stdlib.h>
#include <stdio.h>

Node *create_node(Character ch, int fr, Node *left, Node *right)
{
        Node *new_node = malloc(sizeof(*new_node));
        new_node->ch = ch;
        new_node->freq = fr;
        new_node->left = left;
        new_node->right = right;
        return new_node;
}

int parent(int i)
{
        return (i - 1) / 2;
}

int left_child(int i)
{
        return (2 * i + 1);
}

int right_child(int i)
{
        return (2 * i + 2);
}

Node *get_min(minHeap *heap)
{
        return heap->array[0];
}

minHeap *create_min_heap(int capacity)
{
        minHeap *heap = malloc(sizeof(*heap));
        heap->array = malloc(sizeof(Node *) * capacity);
        heap->capacity = capacity;
        heap->size = 0;
        return heap;
}

void swap(Node **elem1, Node **elem2)
{
        Node *tmp = *elem1;
        *elem1 = *elem2;
        *elem2 = tmp;
}

minHeap *insert(minHeap *heap, Character ch, int freq, Node *left, Node *right)
{
        if (heap->capacity == heap->size)
        {
                fprintf(stderr, "Error with num of elements\n");
                exit(1);
        }

        heap->size++;
        heap->array[heap->size - 1] = create_node(ch, freq, left, right);

        int cur = heap->size - 1;
        while (cur > 0 && heap->array[parent(cur)]->freq > heap->array[cur]->freq)
        {
                swap(&heap->array[parent(cur)], &heap->array[cur]);
                cur = parent(cur);
        }
        return heap;
}

minHeap *heapify(minHeap *heap, int i)
{
        if (heap->size <= 1)
        {
                return heap;
        }
        int left = left_child(i);
        int right = right_child(i);
        int min = i;
        if (left < heap->size && heap->array[left]->freq < heap->array[i]->freq)
                min = left;
        if (right < heap->size && heap->array[right]->freq < heap->array[min]->freq)
                min = right;

        if (min != i)
        {
                swap(&heap->array[i], &heap->array[min]);
                heap = heapify(heap, min);
        }

        return heap;
}

minHeap *delete(minHeap *heap)
{
        if (heap == NULL || heap->size == 0)
                return heap;

        int size = heap->size;
        swap(&(heap->array[0]), &(heap->array[size - 1]));
        heap->size--;
        size--;
        heap = heapify(heap, 0);
        return heap;
}

void free_tree(Node *root)
{
        if (root->left)
        {
                free_tree(root->left);
                free(root->left);
        }

        if (root->right)
        {
                free_tree(root->right);
                free(root->right);
        }
}

Node *create_tree(minHeap *heap)
{
        while (heap->size != 1)
        {
                Node *a = get_min(heap);
                delete (heap);

                Node *b = get_min(heap);
                delete (heap);

                Character tmp;
                tmp.c = '\0';

                insert(heap, tmp, a->freq + b->freq, a, b);
        }
        return heap->array[0];
}

void free_heap(minHeap *heap)
{
        free(heap->array);
        free(heap);
}