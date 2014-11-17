#ifndef __LIST_H__
#define __LIST_H__
#include <ucontext.h>

/* your list data structure declarations */
typedef struct node {
    ucontext_t *ctx;
    struct node *next; 
}thread;

/* your function declarations associated with the list */
void list_clear(thread *);
int list_delete(thread *, thread **);
void list_append(ucontext_t *, thread **);
void list_take(thread *, thread **);
void list_push(thread *, thread **);
thread *list_pop(thread **);

#endif // __LIST_H__
