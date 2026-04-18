#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

typedef struct Node{
  int number;
  struct Node *next;
} Node;

typedef struct{
  Node *head;
  Node *tail;
  int size;
} List;

typedef void (*node_fn)(Node*);

Node *create_node(int number){
  Node* node = (Node*)malloc(sizeof(Node));
  node->number = number;
  node->next = NULL;
  return node;
}

void append_node(List *list, int number){
  Node *node = create_node(number);
  if(list->head == NULL){
    list->head = node;
    list->tail = node;
  } else{
    list->tail->next = node;
    list->tail = node;
  }
  list->size += 1;
}

void clean_list(List *list){
  Node *current = list->head;
  while(current != NULL){
    Node *tmp = current->next;
    free(current);
    current = tmp;
  }
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

void print_list(List *list){
  Node *current = list->head;
  printf("- Linked List [Size: %d]: \n", list->size);
  while(current != NULL){
    Node *tmp = current->next;
    printf("[%d]-->", current->number);
    current = tmp;
  }
  printf("[NULL]\n\n");
}

void print_node(Node *node){
  int tid = omp_get_thread_num();
  printf("TID: %d - Node number: %d\n", tid, node->number);
}

Node* process_list(node_fn fn, List *list){
  printf("Linked List Processed: \n");
  Node *current = list->head;
  #pragma omp parallel num_threads(2)
  {
    while(current != NULL){
      Node *tmp = current->next;
      #pragma omp task firstprivate(current)
      fn(current);
      current = tmp;
    }
  }
}

int main(){
  List linked_list_a = {NULL, NULL, 0};
  List linked_list_b = {NULL, NULL, 0};
    
  int insertions = 20;
  #pragma omp parallel num_threads(20) default(none) shared(insertions, linked_list_a, linked_list_b)
  {
    #pragma omp for
    for(int i = 0; i < insertions; i++){
      int random_insertion = rand() % 2;
      if(random_insertion == 0){
        #pragma omp critical(linked_list_a)
        append_node(&linked_list_a, i);
      }
      else if(random_insertion == 1){
        #pragma omp critical(linked_list_b)
        append_node(&linked_list_b, i);
      }
    }
  }
  
  printf("\n");
  print_list(&linked_list_a);
  print_list(&linked_list_b);


  clean_list(&linked_list_a);
  clean_list(&linked_list_b);

  print_list(&linked_list_a);
  print_list(&linked_list_b);

  return 0;
}

