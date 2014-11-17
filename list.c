#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <string.h>
#include <ucontext.h>

/* your list function definitions */


void list_clear(thread *list) {
    while (list != NULL) {
        thread *tmp = list;
        list = list->next;
		free(((tmp->ctx)->uc_stack).ss_sp);
		free(tmp->ctx);
        free(tmp);
    }
}

int list_delete(thread *ctx, thread **head) {
    // your code here
	thread *temp = *head;
	if (temp == ctx){
		*head = temp->next;
		free(((temp->ctx)->uc_stack).ss_sp);
		free(temp->ctx);
		free(temp);
		return 1;
	}
	temp = temp->next;
	thread *prev = *head;
	while (temp != NULL){
		if (temp == ctx){
			prev->next = temp->next;
			free(((temp->ctx)->uc_stack).ss_sp);
			free(temp->ctx);
			free(temp);
			return 1;
		}
		prev = temp;
		temp = temp->next;
	}
	return 0;
}

void list_append(ucontext_t *ctx, thread **head) {
    // your code here
	thread *newNode = malloc(sizeof(thread));
	thread *temp = *head;
	newNode->ctx = ctx;
	newNode->next = NULL;
	if (*head == NULL){
		*head = newNode;
	}else{
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = newNode;
	}
}

void list_take(thread *item, thread **head){
	thread *temp = *head;
	if (temp == item){
		*head = temp->next;
		temp->next = NULL;
		return;
	}
	temp = temp->next;
	thread *prev = *head;
	while (temp != NULL){
		if (temp == item){
			prev->next = temp->next;
			temp->next = NULL;
			return;
		}
		prev = temp;
		temp = temp->next;
	}
}

void list_push(thread *item, thread **head){
	thread *temp = *head;
	if (*head == NULL){
		*head = item;
	}else{
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = item;
	}
}

thread *list_pop(thread **head){
	if (*head == NULL){
		return NULL;
	}
	thread *tmp = *head;
	*head = tmp->next;
	tmp->next = NULL;
	return tmp;
}
