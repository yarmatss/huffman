#ifndef MINHEAP_H
#define MINHEAP_H

typedef struct Node
{
        char ch;
        int freq;
        struct Node *left;
        struct Node *right;
} Node;

typedef struct Heap
{
        Node **array;
        int size;
        int capacity;
} minHeap;

Node *create_node(char ch, int fr, Node *left, Node *right);
int parent(int i);
int left_child(int i);
int right_child(int i);
Node *get_min(minHeap *heap);
minHeap *create_min_heap(int capacity);
void swap(Node **elem1, Node **elem2);
minHeap *insert(minHeap *heap, char ch, int freq, Node *left, Node *right);
minHeap *heapify(minHeap *heap, int i);
minHeap *delete(minHeap *heap);
void free_tree(Node *root);
Node *create_tree(minHeap *heap);
void free_heap(minHeap *heap);



#endif