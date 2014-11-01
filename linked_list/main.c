#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int val;
    struct node * next;
} node;

void push( node **head, int val )
{
    node **current = head;

    while ( *current ) current = &( *current )->next;

    *current = ( node * )malloc( sizeof( node ) );

    ( *current )->val  = val;
    ( *current )->next = NULL;
}

node * reverse( node * head )
{
    node *new_head = NULL;
    node *current = head;

    while ( current ) 
    {
        node *next = current->next;
        current->next = new_head;
        new_head = current;
        current = next;
    }

    return new_head;
}

void print( const node *head )
{
    for ( ; head; head = head->next ) printf( "%d ", head->val );
    printf( "\n" );
}

void delete( node *head )
{
    while ( head )
    {
        node *tmp = head;
        head = head->next;
        free( tmp );
    }
}

int main()
{   
    node *head = NULL;
    int n;

    scanf( "%d", &n );
    push( &head, n );

    scanf( "%d", &n );
    push( &head, n );

    scanf( "%d", &n );
    push( &head, n );

    print( head );

    head = reverse( head );

    print( head );

    delete( head );

    return 0;
}
