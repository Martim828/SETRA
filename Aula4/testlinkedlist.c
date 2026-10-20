// Testes Unity do módulo linkedlist (correr com: make test)
#include <unity.h>
#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 16    // máximo de nós que assert_list_equals consegue verificar

static struct Node* head;   // lista usada em cada teste

void setUp(void)
{
    // Configuração antes de cada teste: começa com a lista vazia
    head = NULL;
}

void tearDown(void)
{
    // Limpeza após cada teste: liberta os nós (corre mesmo que o teste falhe)
    free_list(&head);
}

// Verifica que a lista tem exatamente os n valores de expected[], por esta ordem
static void assert_list_equals(const int expected[], int n)
{
    int values[MAX_NODES];
    int count = 0;

    for (struct Node* node = head; node != NULL && count < MAX_NODES; node = node->next) {
        values[count++] = node->data;
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(n, count, "numero de nos da lista");
    if (n > 0) {
        TEST_ASSERT_EQUAL_INT_ARRAY(expected, values, n);
    }
}

// Cria a lista 3 -> 2 -> 1 (insert_node insere no início)
static void create_list_321(void)
{
    insert_node(&head, 1);
    insert_node(&head, 2);
    insert_node(&head, 3);
}

// O teste original: inserir um nó e apagá-lo
void test_linkedlist(void)
{
    insert_node(&head, 3);
    TEST_ASSERT_NOT_NULL(head);
    TEST_ASSERT_EQUAL_INT(3, head->data);
    TEST_ASSERT_NULL(head->next);

    delete_node(&head, 3);
    TEST_ASSERT_NULL(head);         // a lista ficou vazia
}

void test_insert_node_InsertsAtTheHead(void)
{
    const int expected[] = {3, 2, 1};

    create_list_321();
    assert_list_equals(expected, 3);
}

void test_delete_node_Head(void)
{
    const int expected[] = {2, 1};

    create_list_321();
    delete_node(&head, 3);
    assert_list_equals(expected, 2);
}

void test_delete_node_Middle(void)
{
    const int expected[] = {3, 1};

    create_list_321();
    delete_node(&head, 2);
    assert_list_equals(expected, 2);
}

void test_delete_node_Last(void)
{
    const int expected[] = {3, 2};

    create_list_321();
    delete_node(&head, 1);
    assert_list_equals(expected, 2);
}

void test_delete_node_MissingValue(void)
{
    const int expected[] = {3, 2, 1};

    create_list_321();
    delete_node(&head, 42);         // não existe: a lista fica igual
    assert_list_equals(expected, 3);
}

void test_delete_node_EmptyList(void)
{
    delete_node(&head, 1);          // não pode rebentar
    TEST_ASSERT_NULL(head);
}

void test_delete_node_OnlyFirstOccurrence(void)
{
    const int expected[] = {8, 7};

    insert_node(&head, 7);
    insert_node(&head, 8);
    insert_node(&head, 7);          // lista: 7 -> 8 -> 7
    delete_node(&head, 7);
    assert_list_equals(expected, 2);
}

void test_free_list_LeavesHeadNull(void)
{
    create_list_321();
    free_list(&head);
    TEST_ASSERT_NULL(head);

    free_list(&head);               // lista vazia: não pode rebentar
    TEST_ASSERT_NULL(head);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_linkedlist);
    RUN_TEST(test_insert_node_InsertsAtTheHead);
    RUN_TEST(test_delete_node_Head);
    RUN_TEST(test_delete_node_Middle);
    RUN_TEST(test_delete_node_Last);
    RUN_TEST(test_delete_node_MissingValue);
    RUN_TEST(test_delete_node_EmptyList);
    RUN_TEST(test_delete_node_OnlyFirstOccurrence);
    RUN_TEST(test_free_list_LeavesHeadNull);

    return UNITY_END();
}
