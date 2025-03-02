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
static void mergeSplit(struct node *head, struct node **front, struct node **back) {
	struct node *fast,*slow;
	if (head == NULL || head->next == NULL) {
		*front = head;
		*back = NULL;
	} else {
		slow = head;
		fast = head->next;
		while (fast != NULL) {
			fast = fast->next;
			if (fast != NULL) {
				slow = slow->next;
				fast = fast->next;
			}
		}
		*front = head;
		*back = slow->next;
		slow->next = NULL;
	}

}
static struct node* mergeLists(struct node *a, struct node *b) {
	struct node* mergedList;
	mergedList = NULL;
	if (a == NULL) {
		return b;
	}
	if (b == NULL) {
		return a;
	}
	if (a->prog.private_mem + a->prog.shared_mem <= b->prog.private_mem + b->prog.shared_mem) {
		mergedList = a;
		mergedList->next = mergeLists(a->next, b);
	} else {
		mergedList = b;
		mergedList->next = mergeLists(a, b->next);
	}
	return mergedList;
}

void mergeSort(struct node **list) {
	struct node *head, *a,*b;
	head = *list;
	if(head == NULL || head->next == NULL) {
		return;
	}
	mergeSplit(head, &a, &b);
	mergeSort(&a);
	mergeSort(&b);

	*list = mergeLists(a,b);
}

