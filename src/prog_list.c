#include "psmem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int prog_list_add(struct node **head, struct program prog) {
	struct node *newNode,*temp,*prev;
	prev = NULL;
	newNode = malloc(sizeof(struct node));
	if (!newNode) {
		return -1;
	}
	newNode->prog = prog;
	newNode->next = NULL;;
	if (*head == NULL) {
		*head = newNode;
		return 0;
	}
	temp = *head;
	while (temp != NULL) {
		if (strcmp(temp->prog.name,prog.name) == 0) {
			temp->prog.private_mem += prog.private_mem;
			temp->prog.shared_mem += prog.shared_mem;
			temp->prog.count += 1;
			free(newNode);
			return 0;
		}
		prev = temp;
		temp = temp->next;
	}
	newNode->next = temp;
	prev->next = newNode;
	return 0;
}
/*TODO sort this linked list */

/*TODO free this list */


