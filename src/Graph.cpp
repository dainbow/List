#include "List.h"
#include "graph.h"

char* MakeListGraph(List* list, const char name[]) {
    assert(list != nullptr);
    
    static char endName[MAX_PATH_FILE_LENGTH] = "";

    GenerateOutputName(name, endName, G_PATH, G_OUTPUT_FORMAT);

    FILE* output = fopen(endName, "w");
    assert(output != nullptr);

    fprintf(output, "digraph G{ \n");
    fprintf(output, "\t0[shape=record,label= \"<d0>%d | <n0>%d | <p0>%d\"];\n", 
    list->chains[0].data, list->chains[0].next, list->chains[0].previous);

    for (uint32_t curChain = 1; curChain < list->memoryAmount; curChain++) {
        fprintf(output, "\t%u[shape=record,label= \"<d%u>%d | <n%u>%d | <p%u>%d\"];\n", 
        curChain, curChain, list->chains[curChain].data, curChain, list->chains[curChain].next, 
        curChain, list->chains[curChain].previous);

        fprintf(output, "\t%u->%u[color=\"White\"];\n", curChain - 1, curChain);
        if (list->chains[curChain].previous != -1) {
            fprintf(output, "\t%u:<n%u> ->%d: <p%d>[color=\"Green\"];\n", curChain, curChain, list->chains[curChain].next, list->chains[curChain].next);
            fprintf(output, "\t%d:<d%d> ->%u: <d%u>[color=\"Red\"];\n", list->chains[curChain].previous, list->chains[curChain].previous, 
            curChain, curChain);
        }
        else {
            fprintf(output, "\t%u:<n%u> ->%d: <p%d>[color=\"Yellow\"];\n", curChain, curChain, list->chains[curChain].next, list->chains[curChain].next);  
        }
    }
    fprintf(output, "}\n");

    char command[MAX_COMMAND_NAME] = "";
    char del[MAX_COMMAND_NAME]     = "";
    char start[MAX_COMMAND_NAME]   = "";
    fclose(output);

    sprintf(command, "\"C:\\Program Files\\Graphviz\\bin\\dot.exe\" %s -Tpng -O", endName);
    sprintf(del, "del %s", endName);
    sprintf(start, "start %s.png", endName);

    system(command);
    system(del);
    system(start);

    return endName;
}

void GenerateOutputName(const char beginningName[], char endName[], const char path[], const char format[]) {
    assert(beginningName != nullptr);
    assert(endName != nullptr);

    static uint32_t fileCopyAmount = 0;
    int32_t fileD = 0;

    sprintf(endName, "%s%s(%u)%s", path, beginningName, fileCopyAmount, format);
    while ((fileD = open(endName, O_RDONLY)) != -1) {
        fileCopyAmount++;
        sprintf(endName, "%s%s(%u)%s", path, beginningName, fileCopyAmount, format);
    }
    close(fileD);
    sprintf(endName, "%s%s(%u)", path, beginningName, fileCopyAmount);
}
