#pragma once

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int32_t BEGINNING_CHAINS_AMOUNT = 10;
const int32_t FAIL = 0;
const int32_t MAX_FILE_NAME_LENGTH = 100;

const char STANDART_DUMP_NAME[]  = "dump";
const char STANDART_GRAPH_NAME[] = "graph";

const char D_PATH[]   = "dump//";
const char D_FORMAT[] = ".htm";

const float INCREASE_MULTPLIER = 1.5;
const float DECREASE_MULTPLIER = 2;

#define _DEBUG_MODE_ 1

#if (_DEBUG_MODE_ == 1)
    #define LOCATION(...) , { __FILE__, __FUNCTION__, __LINE__, #__VA_ARGS__ }
#else
    #define LOCATION(...)
#endif

struct VarInfo {
    const char* file;
    const char* function;
    int line;

    const char* name;
};

enum ListErrors {
    NO_ERROR = 0,
    UNLINKED_CHAIN_ON_THE_LEFT,
    UNLINKED_CHAIN_ON_THE_RIGHT,
    NEXT_IS_MINUS_ONE,
    FIRST_ELEM_NOT_HEAD,
    LAST_ELEM_NOT_TAIL,
    NON_ZERO_HEAD_WHEN_HAVENT_CHAINS,
    ZERO_HEAD_WHEN_HAVE_CHAINS,
    ZERO_TAIL_WHEN_HAVE_CHAINS,
    HEAD_EQUAL_TO_TAIL,
    MEMORY_EXCESS,
    UNMATCHED_LINKED_CHAINS_COUNTER,
    NULL_BREAKPOINT,
    FREE_CHAIN_GOES_TO_NON_FREE_CHAIN,
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
    uint32_t free;

    #if (_DEBUG_MODE_)
        VarInfo creationInfo;
    #endif

    bool fastTranslationFlag;
};

#define ListCtor(name)          \
    List name = {};             \
    ListCtor_(&name LOCATION (name));           

#if (_DEBUG_MODE_ == 1)
    void ListCtor_(List* list, VarInfo info);
#else
    void ListCtor_(List* list);
#endif

void ListDtor(List* list);
void FillChainsWithPoison(List* list, uint32_t index);

void PrintList(List* list);

ListErrors ListVerifier(List* list);
ListErrors VerifyListStructure(List* list);

uint32_t ListInsertBack(List* list, ListElem appElem);
uint32_t ListInsert(List* list, ListElem insElem, uint32_t index);
void ListDelete(List* list, uint32_t index);

Chain* MemoryIncrease(List* list);
Chain* MemoryOptimizer_Slow_Slow(List* list);
bool EnableSuperSpeedTranslation(List* list);

int32_t FindFreeChain(List* list);
int32_t FindLastChain(List* list);
int32_t FindFirstChain(List* list);
int32_t FindNChain(List* list, uint32_t chainNum);
int32_t TranslateLogicalNumberToPhisicalFunctionAsSlowAsItsNameLong(List* list, uint32_t index);

void MakeListDump(List* list, ListErrors error, char* graphName, VarInfo dumpInfo);
const char* ConvertErrorToString(ListErrors error);
