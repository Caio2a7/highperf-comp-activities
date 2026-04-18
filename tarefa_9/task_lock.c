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

omp_lock_t linked_list_lock;

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
  } else {
    list->tail->next = node;
    list->tail = node;
  }
  list->size += 1;
}

void lock_append_node(List *list, int number){
  Node *node = create_node(number);
  omp_set_lock(&linked_list_lock);
  if(list->head == NULL){
    list->head = node;
    list->tail = node;
  } else {
    list->tail->next = node;
    list->tail = node;
  }
  list->size += 1;
  omp_unset_lock(&linked_list_lock);
}

List *create_list(){
  List *list = (List*)malloc(sizeof(List)); 
  *list = (List){.head = NULL, .tail = NULL, .size = 0};
  return list;
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

int main(){
  omp_init_lock(&linked_list_lock);

  int lists_size, insertions, threads_num;
  
  printf("- Digite o número de quantas listas ligadas deseja: ");
  scanf("%d", &lists_size);
  List **linked_lists = (List**)malloc(sizeof(List*) * lists_size);
  for(int i = 0; i < lists_size; i++){
    linked_lists[i] = create_list();
  }

  printf("- Digite o número de quantas inserções aleatórias deseja: ");
  scanf("%d", &insertions);


  printf("- Digite o número de quantas threads para execução deseja: ");
  scanf("%d", &threads_num);

  #pragma omp parallel num_threads(threads_num) default(none) shared(insertions, linked_lists, lists_size)
  {
    #pragma omp for
    for(int i = 0; i < insertions; i++){
      int random_insertion = rand() % lists_size;
      lock_append_node(linked_lists[random_insertion], i);
    }
  }
  
  omp_destroy_lock(&linked_list_lock);

  printf("\n");
  for(int i = 0; i < lists_size; i++){
    print_list(linked_lists[i]);
  }


  for(int i = 0; i < lists_size; i++){
    clean_list(linked_lists[i]);
  }


  for(int i = 0; i < lists_size; i++){
    print_list(linked_lists[i]);
  }

  return 0;
}
