#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct Node {
    char *filename;
    struct Node *next;
} Node;

Node *create_node(char *filename) {
    Node *node = malloc(sizeof(Node));
    node->filename = filename;
    node->next = NULL;
    return node;
}

void append(Node **head, char *filename) {
    Node *new_node = create_node(filename);
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    Node *current = *head;
    while (current->next != NULL)
        current = current->next;
    current->next = new_node;
}

void free_list(Node *head) {
    while (head != NULL) {
        Node *tmp = head;
        head = head->next;
        free(tmp);
    }
}

int main() {
    Node *head = NULL;

    append(&head, "relatorio_janeiro.txt");
    append(&head, "relatorio_fevereiro.txt");
    append(&head, "relatorio_marco.txt");
    append(&head, "dados_vendas.csv");
    append(&head, "backup_2024.zip");
    append(&head, "config_sistema.cfg");
    append(&head, "log_erros.log");
    append(&head, "planilha_financeira.xlsx");

    #pragma omp parallel
    {
        #pragma omp single
        {
            Node *current = head;
            while (current != NULL) {
                Node *node = current;
                #pragma omp task firstprivate(node)
                {
                    printf("Arquivo: %-30s | Thread: %d\n",
                           node->filename, omp_get_thread_num());
                }
                current = current->next;
            }
        }
    }

    free_list(head);
    return 0;
}
