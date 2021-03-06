#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "stack_impl.h"
#include "list_stack.h"

struct _stacknode
{
    struct _stacknode *next;
    char data[]; // Данные узла
};

struct _liststack
{
    size_t esize; // Размер одного элемента данных узла
    struct _stacknode *head;
};

int _seamliststkpush(void *s, const void *data)
{
    struct _liststack *stk = s;

    struct _stacknode *new = malloc(offsetof(struct _stacknode, next)+stk->esize); // Место на указатель на следующий узел+Место на данные
    if (!new)
        return NO_MEMORY;

    memcpy(new->data, data, stk->esize);

    new->next = stk->head;
    stk->head = new;
    return SUCCESS;
}

int _seamliststkpeek(const void *s, void *data)
{
    const struct _liststack *stk = s;
    if (!stk->head)
        return EMPTY;

    memcpy(data, stk->head->data, stk->esize);

    return SUCCESS;
}

int _seamliststkpop(void *s, void *data)
{
    int ret = _seamliststkpeek(s, data);
    struct _liststack *stk = s;

    if (ret == SUCCESS)
    {
        struct _stacknode *del = stk->head;
        stk->head = stk->head->next;
        free(del);
    }

    return ret;
}

int _seamliststkempty(const void *s)
{
    return !((struct _liststack *)s)->head;
}

void _seamliststkdest(void *s)
{
    struct _liststack *stk = s;
    struct _stacknode *tmp;
    while (stk->head)
    {
        tmp = stk->head->next;
        free(stk->head);
        stk->head = tmp;
    }
}

static struct _stackvft _seamliststkvft = {
    .push = _seamliststkpush,
    .pop = _seamliststkpop,
    .peek = _seamliststkpeek,
    .empty = _seamliststkempty,
    .dest = _seamliststkdest
};

stack *make_list_stack(size_t esize)
{
    stack *ret = malloc(offsetof(stack, _stack)+sizeof(struct _liststack));
    ret->vftptr = &_seamliststkvft;

    struct _liststack *stk = (struct _liststack *)(ret->_stack);
    stk->esize = esize;
    stk->head = NULL;

    return ret;
}
