#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const int32_t BEGINNING_CHAINS_AMOUNT = 2;
const int32_t FAIL = 0;

const float INCREASE_MULTPLIER = 1.5;
const float DECREASE_MULTPLIER = 2;

#define _DEBUG_MODE_ 1

enum ListErrors {
    NO_ERROR = 0,
    UNLINKED_CHAIN_ON_THE_LEFT,
    UNLINKED_CHAIN_ON_THE_RIGHT,
    NEXT_IS_MINUS_ONE,
    PREV_IS_MINUS_ONE,
    FIRST_ELEM_NOT_HEAD,
    LAST_ELEM_NOT_TAIL,
    NON_ZERO_HEAD_WHEN_HAVENT_CHAINS,
    ZERO_HEAD_WHEN_HAVE_CHAINS,
    ZERO_TAIL_WHEN_HAVE_CHAINS,
    HEAD_EQUAL_TO_TAIL,
    MEMORY_EXCESS,
    UNMATCHED_LINKED_CHAINS_COUNTER,
    NULL_BREAKPOINT,
    NON_ZERO_LOOP,
};

#define IsListOk(list)                                          \
    {                                                           \
    ListErrors errorNumber = NO_ERROR;                          \
    if ((errorNumber = ListVerifier(list)) != NO_ERROR) {       \
        fprintf(stderr, "ERROR %d ERROR\n", errorNumber);       \
        assert(FAIL);                                           \
    }                                                           \
}  

typedef int32_t ListElem;

struct Chain {
    ListElem data;
    int32_t next;
    int32_t previous;
};

struct List {
    Chain* chains;

    uint32_t memoryAmount;
    uint32_t chainsAmount;

    uint32_t head;
    uint32_t tail;
};

#define ListCtor(name)          \
    List name = {};             \
    ListCtor_(&name);           

void ListCtor_(List* list);
void ListDtor(List* list);
void FillChainsWithPoison(List* list, uint32_t index);

void PrintList(List* list);

ListErrors ListVerifier(List* list);

uint32_t ListInsertBack(List* list, ListElem appElem);
uint32_t ListInsert(List* list, ListElem insElem, uint32_t index);
void ListDelete(List* list, uint32_t index);

Chain* MemoryIncrease(List* list);
Chain* MemoryDecrease(List* list);

int32_t FindFreeChain(List* list);
int32_t FindLastChain(List* list);
int32_t FindFirstChain(List* list);
int32_t FindNChain(List* list, uint32_t chainNum);

