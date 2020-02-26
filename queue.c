#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;
    list_ele_t *tmp;
    while (q->head != NULL) {
        tmp = q->head->next;
        free(q->head->value);
        free(q->head);
        q->head = tmp;
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    size_t length;

    if (!q || !s)
        return false;
    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    /* Copy the string to newh->value */
    length = strlen(s) + 1;
    newh->value = (char *) malloc(length * sizeof(char));
    if (!newh->value) {
        free(newh);
        return false;
    }
    memcpy(newh->value, s, length);

    /* Change head ptr to new one */
    newh->next = q->head;
    q->head = newh;
    if (q->size == 0)
        q->tail = q->head;
    q->size += 1;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    list_ele_t *newh;
    size_t length;

    if (!q || !s)
        return false;
    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    /* Copy the string to newh->value */
    length = strlen(s) + 1;
    newh->value = (char *) malloc(length * sizeof(char));
    if (!newh->value) {
        free(newh);
        return false;
    }
    memcpy(newh->value, s, length);

    /* Change tail ptr to new one */
    if (q->size == 0) {
        q->head = newh;
    } else {
        q->tail->next = newh;
    }
    q->tail = newh;
    newh->next = NULL;
    q->size += 1;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    list_ele_t *tmp;
    if (!q || q->size == 0)
        return false;

    /* Remove element from head */
    tmp = q->head;
    q->head = q->head->next;
    if (--q->size == 0) {
        q->tail = NULL;
    }

    if (sp != NULL) {
        memcpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    free(tmp->value);
    free(tmp);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q)
        return q->size;
    return 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q)
        return;
    if (q->size == 0)
        return;

    list_ele_t *tmp_prev, *tmp_now, *tmp_next, *tmp;
    tmp_prev = NULL;
    tmp_now = q->head;
    tmp_next = q->head->next;
    while (tmp_next != NULL) {
        tmp_now->next = tmp_prev;
        tmp_prev = tmp_now;
        tmp_now = tmp_next;
        tmp_next = tmp_now->next;
    }
    tmp_now->next = tmp_prev;
    tmp = q->tail;
    q->tail = q->head;
    q->head = tmp;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q)
        return;
    if (q->size == 1 || q->size == 0) {
        return;
    }
    queue_t left_queue, right_queue;
    int i;
    list_ele_t *tmp;

    left_queue.size = q->size >> 1;
    right_queue.size = (q->size >> 1) + (q->size & 1);
    left_queue.head = q->head;
    right_queue.tail = q->tail;

    tmp = q->head;

    for (i = 1; i < left_queue.size; i++) {
        tmp = tmp->next;
    }
    left_queue.tail = tmp;
    right_queue.head = tmp->next;
    left_queue.tail->next = NULL;
    right_queue.tail->next = NULL;
    q_sort(&left_queue);
    q_sort(&right_queue);
    q_merge(&left_queue, &right_queue, q);
}

void q_merge(queue_t *left_queue, queue_t *right_queue, queue_t *q)
{
    list_ele_t *left_tmp, *right_tmp, *left_next, *right_next, *current;
    list_ele_t dummy_node;
    left_tmp = left_queue->head;
    right_tmp = right_queue->head;
    q->head = e_compare(left_tmp, right_tmp) ? left_tmp : right_tmp;
    current = &dummy_node;
    while (left_tmp != NULL && right_tmp != NULL) {
        left_next = left_tmp->next;
        right_next = right_tmp->next;
        if (e_compare(left_tmp, right_tmp)) {
            current->next = left_tmp;
            current = left_tmp;
            left_tmp = left_next;
        } else {
            current->next = right_tmp;
            current = right_tmp;
            right_tmp = right_next;
        }
    }
    if (left_tmp == NULL) {
        current->next = right_tmp;
        q->tail = right_queue->tail;
    } else {
        current->next = left_tmp;
        q->tail = left_queue->tail;
    }
}

bool e_compare(list_ele_t *element_a, list_ele_t *element_b)
{
    if (strcmp(element_a->value, element_b->value) > 0) {
        return false;
    } else {
        return true;
    }
}
