#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DEFAULT_CAPACITY 5

typedef struct element
{
    int x;
    int y;
} Element;

// both tail and head have data in their locations.
typedef struct queue
{
    size_t head;
    size_t tail;
    size_t capacity;
    bool isEmpty;
    Element *buff;
} Queue;

Queue createQueue(){
    Queue newQueue = {0, 0, DEFAULT_CAPACITY, true, malloc(sizeof(Element)*DEFAULT_CAPACITY)};
    if(newQueue.buff == NULL)
    {
        abort();
    }
    return newQueue;
} 

void printElem(Element element){
    printf("x: %i, y: %i\n", element.x, element.y);
}

void destroyQueue(Queue queue){
    free(queue.buff);
    queue.buff = NULL;
}

size_t getSize(Queue *queue){
    if (queue->head > queue->tail){
        return queue->head - queue->tail;
    }
    else{
        return queue->head + (queue->capacity - queue->tail);
    }
}

void resize(Queue *queue, size_t newCapacity){
    Element *newBuff = malloc(sizeof(Element) * newCapacity);
    if(newBuff == NULL){
        abort();
    }
    if(queue->tail >= queue->head){
        memcpy(newBuff, queue->buff, queue->head * sizeof(Element));
        memcpy((newBuff + newCapacity) - (queue->capacity-queue->tail), queue->buff + queue->tail, queue->capacity - queue->tail);
    }
    else{
        memcpy(newBuff, queue->buff, getSize(queue));
    }
    free(queue->buff);
    queue->buff = newBuff;
    queue->capacity = newCapacity;
}

void enqueue(Queue *queue, Element element) // * by reference (can be stack or heap)
{
    if(queue->head == queue->tail && !queue->isEmpty){
        resize(queue, queue->capacity*2);
    }
    else{
        if(queue->head == queue->capacity)
        {
            if(queue->tail != 0)
            {
                queue->head = 0;
            }
        }
        queue->buff[queue->head] = element;
        queue->head++;
        queue->isEmpty = false;
    }
}

Element dequeue(Queue *queue){
    if(queue->isEmpty)
    {
        abort();
    }

    Element retElem = queue->buff[queue->tail];

    if(queue->tail == queue->capacity-1)
    {
        queue->tail = 0;
    }
    else{
        queue->tail++;
    }

    if(queue->head == queue->tail)
    {
        queue->isEmpty = true;
    }
    return retElem;    
}

int main(void)
{
    Queue queue = createQueue();
    Element element1 = {1, 2};
    enqueue(&queue, element1);
    Element element2 = {3, 4};
    enqueue(&queue, element2);
    printElem(dequeue(&queue));
    printElem(dequeue(&queue));
    enqueue(&queue, element1);
    enqueue(&queue, element2);
    printElem(dequeue(&queue));
    printElem(dequeue(&queue));
    enqueue(&queue, element1);
    enqueue(&queue, element2);
    printElem(dequeue(&queue));
    printElem(dequeue(&queue));
    
    enqueue(&queue, element1);
    enqueue(&queue, element2);

    destroyQueue(queue);
    return 0; 
}




