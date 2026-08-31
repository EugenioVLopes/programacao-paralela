#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int data;
    struct node *next;
} Node;

Node *create_node(int data) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

void insert_at_beginning(Node **head, int data) {
    Node *new_node = create_node(data);
    new_node->next = *head;
    *head = new_node;
}

void insert_at_end(Node **head, int data) {
    Node *new_node = create_node(data);
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    Node *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

void insert_at_position(Node **head, int data, int position) {
    if (position == 0) {
        insert_at_beginning(head, data);
        return;
    }
    Node *new_node = create_node(data);
    Node *current = *head;
    for (int i = 0; i < position - 1; i++) {
        if (current == NULL) {
            printf("Position out of bounds\n");
            return;
        }
        current = current->next;
    }
    if (current == NULL) {
        printf("Position out of bounds\n");
        return;
    }
    new_node->next = current->next;
    current->next = new_node;
}

void delete_at_beginning(Node **head) {
    if (*head == NULL) {
        printf("List is empty\n");
        return;
    }
    Node *temp = *head;
    *head = (*head)->next;
    free(temp);
}

void delete_at_end(Node **head) {
    if (*head == NULL) {
        printf("List is empty\n");
        return;
    }
    if ((*head)->next == NULL) {
        free(*head);
        *head = NULL;
        return;
    }
    Node *current = *head;
    Node *previous = NULL;
    while (current->next != NULL) {
        previous = current;
        current = current->next;
    }
    previous->next = NULL;
    free(current);
}

void delete_at_position(Node **head, int position) {
    if (*head == NULL) {
        printf("List is empty\n");
        return;
    }
    if (position == 0) {
        delete_at_beginning(head);
        return;
    }
    Node *current = *head;
    Node *previous = NULL;
    for (int i = 0; i < position; i++) {
        if (current == NULL) {
            printf("Position out of bounds\n");
            return;
        }
        previous = current;
        current = current->next;
    }
    if (current == NULL) {
        printf("Position out of bounds\n");
        return;
    }
    previous->next = current->next;
    free(current);
}

void print_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

int main() {
    Node *head = NULL;
    insert_at_beginning(&head, 10);
    insert_at_end(&head, 20);
    insert_at_position(&head, 15, 1);
    print_list(head);
    delete_at_beginning(&head);
    delete_at_end(&head);
    delete_at_position(&head, 0);
    print_list(head);
    return 0;
}