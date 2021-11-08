#include "List.h"
#include "Graph.h"

int main() {
    ListCtor(list);

    ListInsertBack(&list, 228);
    ListInsertBack(&list, 322);
    ListInsertAfter(&list, 777, 0);
    ListInsertBack(&list, 1000-7);
    ListDelete(&list, 1);
    list.fastTranslationFlag = EnableSuperSpeedTranslation(&list);
    ListInsertBack(&list, 7);
    ListInsertBack(&list, 7);
    PrintList(&list);
    
    ListDtor(&list);
}

void ListCtor_(List* list, VarInfo info) {
    assert(list != nullptr);

    list->memoryAmount        = BEGINNING_CHAINS_AMOUNT;
    list->chainsAmount        = 1;
    list->head                = 0;
    list->tail                = 0;
    list->free                = 1;
    list->fastTranslationFlag = 0;

    #if (_DEBUG_MODE_)
        list->creationInfo = info;
    #endif

    list->chains = (Chain*)calloc(BEGINNING_CHAINS_AMOUNT, sizeof(list->chains[0]));
    FillChainsWithPoison(list, 0);

    list->chains[0] = {0, 0, 0};
}

void ListDtor(List* list) {
    assert(list != nullptr);

    #if(_DEBUG_MODE_ == 1)
        FillChainsWithPoison(list, 0);
    #endif
    free(list->chains);

    list->memoryAmount        = 0;
    list->chainsAmount        = 0;
    list->head                = 0;
    list->tail                = 0;
    list->fastTranslationFlag = 0;
}

void FillChainsWithPoison(List* list, uint32_t index) {
    assert(list != nullptr);

    for (uint32_t curChain = index; curChain < list->memoryAmount; curChain++) {
        list->chains[curChain].data     = -666;
        list->chains[curChain].next     = curChain + 1;
        list->chains[curChain].previous = -1;
    }

    list->chains[list->memoryAmount - 1].next = 0;
}

uint32_t ListInsertBack(List* list, ListElem appElem) {
    return ListInsertAfter(list, appElem, list->tail);
}

uint32_t ListInsertBeginning(List* list, ListElem appElem) {
    return ListInsertAfter(list, appElem, 0);
}

uint32_t ListInsertBefore(List* list, ListElem insElem, uint32_t index) {
    assert(index != 0);
    return ListInsertAfter(list, insElem, index - 1);
}

int32_t FindFreeChain(List* list) {
    assert(list != nullptr);

    if (list->free != 0)
        return list->free;
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

    list->chains[index].next     = list->free;
    list->chains[index].previous = -1;
    list->chains[index].data = -666;

    list->chainsAmount--;
    list->free = index;
    
    if (index != list->tail) {
        list->fastTranslationFlag = 0;
    }

    #if(_DEBUG_MODE_ == 1)
        IsListOk(list);
    #endif
}

uint32_t ListInsertAfter(List* list, ListElem insElem, uint32_t index) {
    assert(list != nullptr);
    assert(list->chains[index].previous != -1);
    #if(_DEBUG_MODE_ == 1)
        IsListOk(list);
    #endif

    if ((list->chainsAmount + 1) > list->memoryAmount) {
        list->chains = MemoryIncrease(list);
        FillChainsWithPoison(list, list->chainsAmount);

        MakeListDump(list, NO_ERROR, MakeListGraph(list, STANDART_GRAPH_NAME) LOCATION(list));
    }
    
    int32_t freeChain = FindFreeChain(list);
    assert(freeChain != -1);
    list->free = list->chains[list->free].next;

    if ((list->chains[index].next != 0) || ((index == 0) && (list ->chainsAmount != 1))) {
        if (index != 0) {
            list->chains[list->chains[index].next].previous = freeChain;
        }
        else {
            list->chains[list->head].previous = freeChain;
        }
        list->chains[freeChain].previous = index;
    }
    else {
        list->chains[freeChain].previous = list->tail;
        list->tail = freeChain;
    }

    if ((index != 0) || (list->chainsAmount == 2)) {
        list->chains[freeChain].next     = list->chains[index].next;
        list->chains[index].next         = freeChain;
    }
    else {
        list->chains[freeChain].next     = list->head;
        list->head = freeChain;
    }

    list->chains[freeChain].data = insElem;
    list->chainsAmount++;
    if (index != list->tail) {
        list->fastTranslationFlag = 0;
    }

    #if(_DEBUG_MODE_ == 1)
        IsListOk(list);
    #endif
    return freeChain;
}

ListErrors ListVerifier(List* list) {
    assert(list != nullptr);

    uint32_t myChainCounter = 1;
    bool oneElemZeroLoopFlag = 0;

    ListErrors curError = NO_ERROR;

    if ((curError = VerifyListStructure(list)) != NO_ERROR) {
        return curError;
    }

    for (uint32_t curChain = 0; curChain < list->memoryAmount; curChain++) {
        Chain currentChain = list->chains[curChain];

        if (currentChain.next == -1) {
            return NEXT_IS_MINUS_ONE;
        }

        if (curChain == 0) {
            if (currentChain.next != currentChain.previous) {
                return NULL_BREAKPOINT;
            }

            continue;
        }

        if (currentChain.next == currentChain.previous) {
            if (oneElemZeroLoopFlag || list->chainsAmount > 2) {
                return NON_ZERO_LOOP;
            }
            else {
                oneElemZeroLoopFlag = 1;
            }
        }

        if (currentChain.previous == 0) {
            if (curChain != list->head) {
                return FIRST_ELEM_NOT_HEAD;
            }
        }
        else if (currentChain.previous == -1) {
            if ((list->chains[currentChain.next].previous != -1) &&
                (currentChain.next != 0)) {
                return FREE_CHAIN_GOES_TO_NON_FREE_CHAIN;
            }
        }
        else if ((int32_t)curChain != list->chains[currentChain.previous].next) {
            return UNLINKED_CHAIN_ON_THE_LEFT;
        }

        if ((currentChain.next == 0) &&
            (currentChain.previous != -1)) {
            if (curChain != list->tail) {
                return LAST_ELEM_NOT_TAIL;
            }
        }
        else if (((int32_t)curChain != list->chains[currentChain.next].previous) &&
                 (currentChain.previous != -1)) {
            return UNLINKED_CHAIN_ON_THE_RIGHT;
        }

        if (currentChain.previous != -1)
            myChainCounter++;
    }

    if (myChainCounter != list->chainsAmount) {
        return UNMATCHED_LINKED_CHAINS_COUNTER;
    }  

    return NO_ERROR;
}

ListErrors VerifyListStructure(List* list) {
    assert(list != nullptr);
    
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

    return NO_ERROR;
}

Chain *MemoryIncrease(List* list) {
    assert(list != nullptr);
    
    list->memoryAmount = (uint32_t)((float)list->memoryAmount * INCREASE_MULTPLIER);
    list->free         = list->chainsAmount;

    Chain* newChains = (Chain*)realloc(list->chains, list->memoryAmount * sizeof(newChains[0]));
    assert(newChains != nullptr);

    return newChains;
}

Chain *MemoryOptimizer_Slow_Slow(List* list) {
    assert(list != nullptr);
    
    if ((float)list->chainsAmount * DECREASE_MULTPLIER <= (float)list->memoryAmount)
        list->memoryAmount = (uint32_t)((float)list->memoryAmount / DECREASE_MULTPLIER);

    Chain* newChains = (Chain*)calloc(list->memoryAmount, sizeof(newChains[0]));

    newChains = nullptr;
    for (uint32_t curChain = list->head, chainNum = 1; curChain != 0; curChain = list->chains[curChain].next, chainNum++) {
        Chain curChainStruct = list->chains[curChain];
        if (curChain == list->head) {
            if (newChains != nullptr) {
                newChains[chainNum] = {curChainStruct.data, (int32_t)(chainNum + 1), 0};
            }      
        }
        else if (curChain == list->tail) {
            if (newChains == nullptr) {
                list->chains[curChainStruct.previous].next         = (int32_t)(chainNum);
                list->chains[curChainStruct.previous].previous     = (int32_t)(chainNum - 2);
            }
            else
                newChains[chainNum] = {curChainStruct.data, 0, (int32_t)(chainNum - 1)};
        }
        else {
            if (newChains == nullptr) {
                list->chains[curChainStruct.previous].next         = (int32_t)(chainNum);
                list->chains[curChainStruct.previous].previous     = (int32_t)(chainNum - 2);
            }
            else
                newChains[chainNum] = {curChainStruct.data, (int32_t)(chainNum + 1), (int32_t)(chainNum - 1)};
        }
    }

    if (newChains == nullptr) {
        list->chains[list->tail].next     = 0;
        list->chains[list->tail].previous = (int32_t)(list->chainsAmount - 2);

        qsort(list->chains, list->memoryAmount, sizeof(list->chains[0]), ChainsComparator);
    }

    list->head = 1;
    list->tail = list->chainsAmount - 1;
    list->free = list->chainsAmount;

    return newChains;
}

bool EnableSuperSpeedTranslation(List* list) {
    assert(list != nullptr);

    Chain* newChains = 0;

    if ((newChains = MemoryOptimizer_Slow_Slow(list)) != nullptr) {
        list->chains = newChains;
    }
    FillChainsWithPoison(list, list->chainsAmount);

    list->fastTranslationFlag = 1;
    return 1;
}

int32_t FindFirstChainWithValue(List* list, ListElem value) {
    assert(list != nullptr);

    for (uint32_t curChain = list->head; curChain != 0; curChain = list->chains[curChain].next) {
        if (list->chains[curChain].data == value) {
            return (int32_t)curChain;
        }
    }

    return -1;
}

int32_t TranslateLogicalNumberToPhisicalFunctionAsSlowAsItsNameLong(List* list, uint32_t index) {
    assert(list != nullptr);
    assert(index < list->chainsAmount);

    if (list->fastTranslationFlag) {
        return (int32_t)index;
    }

    for (uint32_t curChain = list->head, chainNum = 1; curChain != 0; curChain = list->chains[curChain].next, chainNum++) {
        if (chainNum == index) {
            return (int32_t)curChain;
        }
    }

    return -1;
}

const char* ConvertErrorToString(ListErrors error) {
    switch (error) {
        case NO_ERROR:                         return "Ok";
        case UNLINKED_CHAIN_ON_THE_LEFT:       return "UNLINKED CHAIN ON THE LEFT SIDE";
        case UNLINKED_CHAIN_ON_THE_RIGHT:      return "UNLINKED CHAIN ON THE RIGHT SIDE";
        case NEXT_IS_MINUS_ONE:                return "SOME CHAIN NEXT IS MINUS ONE";
        case FIRST_ELEM_NOT_HEAD:              return "FIRST ELEMENT OF LIST ISN'T HEAD";
        case LAST_ELEM_NOT_TAIL:               return "LAST ELEMENT OF LIST ISN'H TAIL";
        case NON_ZERO_HEAD_WHEN_HAVENT_CHAINS: return "THE HEAD ISN'T ZERO WHEN DON'T HAVE NON_NULL CHAINS";
        case ZERO_HEAD_WHEN_HAVE_CHAINS:       return "THE HEAD IS ZERO WHEN HAVE NON_NULL CHAINS";
        case ZERO_TAIL_WHEN_HAVE_CHAINS:       return "THE TAIL IS ZERO WHEN HAVE NON_NULL CHAINS";
        case HEAD_EQUAL_TO_TAIL:               return "HEAD IS EQUAL TO TAIL WHEN HAVE 2 OR MORE NON_NULL CHAINS";
        case MEMORY_EXCESS:                    return "AMOUNT OF CHAINS IS MORE THAN MEMORY AMOUNT";
        case UNMATCHED_LINKED_CHAINS_COUNTER:  return "HAVE MORE LINKED CHAINS THAN STRUCTURE STORES";
        case NULL_BREAKPOINT:                  return "NULL CHAIN DOESN'T LOOP";
        case FREE_CHAIN_GOES_TO_NON_FREE_CHAIN:return "FREE CHAIN POINTS TO NON_FREE CHAIN";
        case NON_ZERO_LOOP:                    return "NON ZERO LOOP";

        default:                               return "UNKNOWN ERROR";
    }
}

void MakeListDump(List* list, ListErrors error, char* graphName, VarInfo dumpInfo) {
    assert(graphName != nullptr);
    assert(list != nullptr);

    char endName[MAX_FILE_NAME_LENGTH] = "";

    GenerateOutputName(STANDART_DUMP_NAME, endName, D_PATH, D_FORMAT);
    strcat(endName, D_FORMAT);

    FILE* output = fopen(endName, "w");
    fprintf(output, "<pre>\n");

    fprintf(output, "<font color=\"Red\">\n");
    fprintf(output, "ERROR %s ERROR", ConvertErrorToString(error));
    fprintf(output, "</font>\n");

    fprintf(output, "Dump from %s() at %s(%d) in list called now \"%s\": IsListOk() FAILED\n", 
                    dumpInfo.function, dumpInfo.file, dumpInfo.line, dumpInfo.name);
    fprintf(output, "list [%p] ", list);
    fprintf(output, "from %s (%d), %s()\n",
            list->creationInfo.file,  list->creationInfo.line, list->creationInfo.function);

    fprintf(output, "<font color=\"Blue\">\n");
    fprintf(output, "\tList head is %u\n" 
                    "\tList tail is %u\n" 
                    "\tList free is %u\n"
                    "\tList chains[%p]",
                    list->head, list->tail, list->free, list->chains);
    fprintf(output, "</font>\n");

    fprintf(output, "<font color=\"Green\">\n");
    fprintf(output, "\tList chains amount is %u\n" 
                    "\tList memory amount is %u\n"
                    "\tList super speed transition flag is %d\n", 
                    list->chainsAmount, list->memoryAmount, list->fastTranslationFlag);
    fprintf(output, "</font>\n");

    fprintf(output, "<img src = \"..\\%s%s\">", graphName, G_OUTPUT_FORMAT);
    fclose(output);

    char startCmd[MAX_COMMAND_NAME] = "";
    sprintf(startCmd, "start %s", endName);

    system(startCmd);
}

int32_t ChainsComparator(const void* chain1ptr, const void* chain2ptr) {
    assert(chain1ptr != nullptr);
    assert(chain2ptr != nullptr);

    const Chain chain1 = *((const Chain*)chain1ptr);
    const Chain chain2 = *((const Chain*)chain2ptr);

    if ((uint32_t)chain1.previous >= (uint32_t)chain2.previous)
        return 1;
    return -1;
}
