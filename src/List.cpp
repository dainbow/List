#include "List.h"

int main() {
    ListCtor(list);

    ListInsertBack(&list, 228);
    ListInsertBack(&list, 228);
    ListInsertBack(&list, 228);
    ListInsertBack(&list, 228);
    ListInsertBack(&list, 228);
    ListInsertBack(&list, 228);
    

    PrintList(&list);

    ListDtor(&list);
}

void ListCtor_(List* list) {
    assert(list != nullptr);

    list->memoryAmount = BEGINNING_CHAINS_AMOUNT;
    list->chainsAmount = 1;
    list->head         = 0;
    list->tail         = 0;

    list->chains = (Chain*)calloc(BEGINNING_CHAINS_AMOUNT, sizeof(list->chains[0]));
    FillChainsWithPoison(list, 0);

    list->chains[0] = {0, 0, 0};
}

void ListDtor(List* list) {
    assert(list != nullptr);

    free(list->chains);

    list->memoryAmount = 0;
    list->chainsAmount = 0;
    list->head         = 0;
    list->tail         = 0;
}

void FillChainsWithPoison(List* list, uint32_t index) {
    assert(list != nullptr);

    for (uint32_t curChain = index; curChain < list->memoryAmount; curChain++) {
        list->chains[curChain].next     = -1;
        list->chains[curChain].previous = -1;
    }
}

uint32_t ListInsertBack(List* list, ListElem appElem) {
    return ListInsert(list, appElem, list->tail);
}

int32_t FindFreeChain(List* list) {
    assert(list != nullptr);

    for (uint32_t curChain = 0; curChain < list->memoryAmount; curChain++) {
        if (list->chains[curChain].next == -1) {
            return curChain;
        }
    }

    return -1;
}

void PrintList(List* list) {
    assert(list != nullptr);

    for (int32_t curChain = list->head, chainNum = 1; ; curChain = list->chains[curChain].next, chainNum++) {
        printf("[p%d][l%d] chain is %d\n", curChain, chainNum, list->chains[curChain].data);

        if (list->chains[curChain].next == 0) {
            break;
        }
    }
}

void ListDelete(List* list, uint32_t index) {
    assert(list != nullptr);
    assert(index != 0);
    assert(list->chains[index].next     != -1);
    assert(list->chains[index].previous != -1);

    #if(_DEBUG_MODE_ == 1)
        IsListOk(list);
    #endif

    if ((list->chainsAmount - 1) < (uint32_t)((float)list->memoryAmount / DECREASE_MULTPLIER)) {
        list->chains = MemoryDecrease(list);
    }

    if (list->chains[index].next != 0) {
        list->chains[list->chains[index].next].previous = list->chains[index].previous;
    }
    else {
        list->tail = list->chains[index].previous;
    }

    if (list->chains[index].previous != 0) {
        list->chains[list->chains[index].previous].next = list->chains[index].next;
    }
    else {
        list->head = list->chains[index].next;
    }

    list->chains[index].next     = -1;
    list->chains[index].previous = -1;

    list->chainsAmount--;

    #if(_DEBUG_MODE_ == 1)
        IsListOk(list);
    #endif
}

uint32_t ListInsert(List* list, ListElem insElem, uint32_t index) {
    assert(list != nullptr);
    assert(list->chains[index].next != -1);
    #if(_DEBUG_MODE_ == 1)
        IsListOk(list);
    #endif

    if ((list->chainsAmount + 1) > list->memoryAmount) {
        list->chains = MemoryIncrease(list);
    }
    
    int32_t freeChain = FindFreeChain(list);
    assert(freeChain != -1);

    if ((list->chains[index].next != 0) || ((index == 0) && (list ->chainsAmount != 1))) {
        printf("Setted prev of %d to %d\n" 
               "Setted prev of %d to %u\n", list->chains[index].next, freeChain, freeChain, index);

        if (index != 0) {
            list->chains[list->chains[index].next].previous = freeChain;
        }
        else {
            list->chains[list->head].previous = freeChain;
        }
        list->chains[freeChain].previous = index;
    }
    else {
        printf("Setted tail to %d\n" 
               "Setted prev of %d to %u\n", freeChain, freeChain, list->tail);

        list->chains[freeChain].previous = list->tail;
        list->tail = freeChain;
    }

    if ((index != 0) || (list->chainsAmount == 2)) {
        printf("Setted next of %u to %d\n" 
               "Setted next of %d to %d\n", index, freeChain, freeChain, list->chains[index].next);
        list->chains[freeChain].next     = list->chains[index].next;
        list->chains[index].next         = freeChain;
    }
    else {
        list->chains[freeChain].next     = list->head;
        printf("Setted head to %d\n" 
               "Setted next of %d to %u\n", freeChain, freeChain, list->head);
        list->head = freeChain;
    }

    list->chains[freeChain].data = insElem;
    list->chainsAmount++;
    
    return freeChain;

    #if(_DEBUG_MODE_ == 1)
        IsListOk(list);
    #endif
}

ListErrors ListVerifier(List* list) {
    assert(list != nullptr);

    uint32_t myChainCounter = 1;
    bool oneElemZeroLoopFlag = 0;
    
    if (list->head == list->tail) {
        if (list->chainsAmount > 2) {
            return HEAD_EQUAL_TO_TAIL;
        }
        else if ((list->chainsAmount == 1) && (list->head != 0)) {
            return NON_ZERO_HEAD_WHEN_HAVENT_CHAINS;
        }
    }

    if (list->chainsAmount > list->memoryAmount) {
        return MEMORY_EXCESS;
    }

    if (list->chainsAmount > 1) {
        if (list->head == 0) {
            return ZERO_HEAD_WHEN_HAVE_CHAINS;
        }
        if (list->tail == 0){
            return ZERO_TAIL_WHEN_HAVE_CHAINS;
        }

    }

    for (uint32_t curChain = 0; curChain < list->memoryAmount; curChain++) {
        Chain currentChain = list->chains[curChain];

        if ((currentChain.next == -1) ^ (currentChain.previous == -1)) {
            if (currentChain.next == -1) {
                return NEXT_IS_MINUS_ONE;
            }

            if (currentChain.previous == -1) {
                return PREV_IS_MINUS_ONE;
            }
        }
        if (currentChain.next == -1) {
            continue;
        }

        if (curChain == 0) {
            if (currentChain.next != currentChain.previous) {
                return NULL_BREAKPOINT;
            }

            continue;
        }

        if (currentChain.next == currentChain.previous) {
            if ((currentChain.next != 0) && (curChain != 0)) {
                if (oneElemZeroLoopFlag || list->chainsAmount > 2) {
                    return NON_ZERO_LOOP;
                }
                else {
                    oneElemZeroLoopFlag = 1;
                }
            }
        }

        if (currentChain.previous == 0) {
            if (curChain != list->head) {
                fprintf(stderr, "curChain is %u; Head should be %u\n", curChain, list->head);

                return FIRST_ELEM_NOT_HEAD;
            }
        }
        else if ((int32_t)curChain != list->chains[currentChain.previous].next) {
            return UNLINKED_CHAIN_ON_THE_LEFT;
        }

        if (currentChain.next == 0) {
            if (curChain != list->tail) {
                return LAST_ELEM_NOT_TAIL;
            }
        }
        else if ((int32_t)curChain != list->chains[currentChain.next].previous) {
            fprintf(stderr, "My chain next is %d; Next chain prev is %d\n", currentChain.next, list->chains[currentChain.next].previous);
            
            return UNLINKED_CHAIN_ON_THE_RIGHT;
        }

        myChainCounter++;
    }

    if (myChainCounter != list->chainsAmount) {
        fprintf(stderr, "I counted %u; In structure %u;\n", myChainCounter, list->chainsAmount);

        return UNMATCHED_LINKED_CHAINS_COUNTER;
    }  

    return NO_ERROR;
}

Chain *MemoryIncrease(List* list) {
    assert(list != nullptr);
    
    list->memoryAmount = (uint32_t)((float)list->memoryAmount * INCREASE_MULTPLIER);
    Chain* newChains = (Chain*)realloc(list->chains, list->memoryAmount * sizeof(newChains[0]));
    assert(newChains != nullptr);

    FillChainsWithPoison(list, list->chainsAmount);
    return newChains;
}

Chain *MemoryDecrease(List* list) {
    assert(list != nullptr);
    
    list->memoryAmount = (uint32_t)((float)list->memoryAmount / DECREASE_MULTPLIER);
    Chain* newChains = (Chain*)realloc(list->chains, list->memoryAmount * sizeof(newChains[0]));
    assert(newChains != nullptr);

    return newChains;
}
