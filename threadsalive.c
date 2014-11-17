/*
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <strings.h>
#include <string.h>
#include <ucontext.h>
#include "list.h"

#include "threadsalive.h"

/* ***************************** 
     stage 1 library functions
   ***************************** */

static thread *head; 
static thread *curr;
static ucontext_t mainCtx;
static int numblockedthreads;

void ta_libinit(void) {
    head = NULL;
	curr = NULL;
	numblockedthreads = 0;
}

void ta_create(void (*func)(void *), void *arg) {
#define STACKSIZE 131072
	unsigned char *stack = (unsigned char *)malloc(STACKSIZE);
	ucontext_t *ctx = malloc(sizeof(ucontext_t));
	getcontext(ctx);
	(ctx->uc_stack).ss_sp = stack;
	(ctx->uc_stack).ss_size = STACKSIZE;
	ctx->uc_link = &mainCtx;
	list_append(ctx, &head);
	makecontext(ctx, (void (*)(void))func, 1, arg);
}

void ta_yield(void) {
	thread *tmp = curr;
	if (curr->next != NULL){		
		curr = curr->next;
	}else{
		curr = head;
	}
	swapcontext(tmp->ctx, curr->ctx);	
}

int ta_waitall(void) {
    while (head->next != NULL){
		thread *next;
		if (curr == NULL){
			next = head;
		}else if (curr == head){
			next = curr->next;
			list_delete(curr, &head);
		}else{
			next = head;
			list_delete(curr, &head);
		}
		curr = next;
		swapcontext(&mainCtx, curr->ctx);
	}
	list_delete(head, &head);
	if (numblockedthreads > 0){
		return -1;
	}else{
		return 0;
	}
}


/* ***************************** 
     stage 2 library functions
   ***************************** */

void ta_sem_init(tasem_t *sem, int value) {
	sem->value = value;
	sem->queue = NULL;
}

void ta_sem_destroy(tasem_t *sem) {
	list_clear(sem->queue);
}

void ta_sem_post(tasem_t *sem) {
	sem->value++;
	thread *woken = list_pop(&sem->queue);
	if (woken != NULL){		
		list_push(woken, &head);
		numblockedthreads--;
	}	
}

void ta_sem_wait(tasem_t *sem) {
	if (sem->value <= 0){
		list_take(curr, &head);
		list_push(curr, &sem->queue);
		numblockedthreads++;
		ta_yield();
	}
	sem->value--;
}

void ta_lock_init(talock_t *mutex) {
	mutex->sem = malloc(sizeof(tasem_t));
	ta_sem_init(mutex->sem, 1);
}

void ta_lock_destroy(talock_t *mutex) {
	ta_sem_destroy(mutex->sem);
	free(mutex->sem);
}

void ta_lock(talock_t *mutex) {
	ta_sem_wait(mutex->sem);
}

void ta_unlock(talock_t *mutex) {
	ta_sem_post(mutex->sem);
}


/* ***************************** 
     stage 3 library functions
   ***************************** */

void ta_cond_init(tacond_t *cond) {
	cond->sem = malloc(sizeof(tasem_t));
	ta_sem_init(cond->sem, 0);
}

void ta_cond_destroy(tacond_t *cond) {
	ta_sem_destroy(cond->sem);
	free(cond->sem);
}

void ta_wait(talock_t *mutex, tacond_t *cond) {
	ta_unlock(mutex);
	ta_sem_wait(cond->sem);
}

void ta_signal(tacond_t *cond) {
	ta_sem_post(cond->sem);
}

