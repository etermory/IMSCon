#pragma once

typedef struct _node {
    void* data;
    struct _node* next;
} NODE;


typedef struct {
    NODE* head;
} LIST;