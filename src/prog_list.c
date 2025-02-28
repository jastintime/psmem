#include "psmem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int prog_list_add(struct node **head, struct program prog) {
	struct node *newNode,*temp;
	newNode = (struct node*) malloc(sizeof(struct node));
	if (!newNode) {
		return -1;
	}
	newNode->prog = prog;
	newNode->next = NULL;
	if (*head == NULL) {
		*head = newNode;
		return 0;
	}
	temp = *head;

	while (temp->next != NULL) {
		temp = temp->next;
		if (strcmp(temp->prog.name,prog.name) == 0) {
			temp->prog.private_mem += prog.private_mem;
			temp->prog.shared_mem += prog.shared_mem;
			temp->prog.count += 1;
			free(newNode);
			return 0;
		}
	}
	temp->next = newNode;
	return 0;
}

void prog_list_free(struct node **head) {
	struct node* tmp;
	while (*head != NULL) {
		tmp = *head;
		*head = (*head)->next;
		free(tmp);
	}
}

/*TODO sort this linked list */

