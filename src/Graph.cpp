#include "List.h"
#include "graph.h"

char* MakeListGraph(List* list, const char name[]) {
    assert(list != nullptr);
    
    static char endName[MAX_PATH_FILE_LENGTH] = "";

    GenerateOutputName(name, endName, G_PATH, G_OUTPUT_FORMAT);

    FILE* output = fopen(endName, "w");
    assert(output != nullptr);

    fprintf(output, "digraph G{ \n"
                    "newrank=true;\n");

    fprintf(output, "\t0[shape=record,label= \"<d0>%d | <n0>%d | <p0>%d\"];\n", 
            list->chains[0].data, list->chains[0].next, list->chains[0].previous);
    fprintf(output, "subgraph cluster0 {\n"
		"style = filled;\n"
		"color = lightgray;\n"
		"0;\n"
		"label = \"NULL_CHAIN\";"
	"}\n");

    for (uint32_t curChain = 1; curChain < list->memoryAmount; curChain++) {
        fprintf(output, "\t%u[shape=record, style = rounded, label= \"<d%u>%d | <n%u>%d | <p%u>%d\"];\n", 
                curChain, 
                curChain, list->chains[curChain].data, 
                curChain, list->chains[curChain].next, 
                curChain, list->chains[curChain].previous);

        fprintf(output, "\t%u->%u[style=invis];\n", 
                curChain - 1, curChain);

        fprintf(output, "subgraph cluster%u {\n"
		"style = filled;\n"
		"color = lightgray;\n"
		"%u;\n"
		"label = \"%u\";"
	"}\n", curChain, curChain, curChain);

        if (curChain == list->head) {
                fprintf(output, "head[label = \"HEAD\", color = red, style = rounded];\n");
                fprintf(output, "{rank = same; %u; head}\n", curChain);
        }
        if (curChain == list->free) {
                fprintf(output, "free[label = \"FREE\", color = red, style = rounded];\n");
                fprintf(output, "{rank = same; %u; free}\n", curChain);
        }
        if (curChain == list->tail) {
                fprintf(output, "tail[label = \"TAIL\", color = red, style = rounded];\n");
                fprintf(output, "{rank = same; %u; tail}\n", curChain);
        }

        if (list->chains[curChain].previous != -1) {
            fprintf(output, "\t%u:<n%u> ->%d: <p%d>[color=\"Green\"];\n", 
                    curChain, curChain, 
                    list->chains[curChain].next, list->chains[curChain].next);
            fprintf(output, "\t%d:<d%d> ->%u: <d%u>[color=\"Red\"];\n", 
                    list->chains[curChain].previous, list->chains[curChain].previous, 
                    curChain, curChain);
        }
        else {
            fprintf(output, "\t%u:<n%u> ->%d: <p%d>[color=\"Yellow\"];\n", 
                    curChain, curChain, 
                    list->chains[curChain].next, list->chains[curChain].next);  
        }
    }
    fprintf(output, "}\n");

    char command[MAX_COMMAND_NAME] = "";
    char del[MAX_COMMAND_NAME]     = "";
    fclose(output);

    sprintf(command, "%s %s -Tpng -O", 
            DOT_EXE_PATH, endName);
    sprintf(del, "del %s", endName);

    system(command);
    system(del);

    return endName;
}

void GenerateOutputName(const char beginningName[], char endName[], const char path[], const char format[]) {
    assert(beginningName != nullptr);
    assert(endName != nullptr);

    uint32_t fileCopyAmount = 0;
    int32_t fileD = 0;

    sprintf(endName, "%s%s(%u)%s", path, beginningName, fileCopyAmount, format);
    while ((fileD = open(endName, O_RDONLY)) != -1) {
        fileCopyAmount++;
        sprintf(endName, "%s%s(%u)%s", path, beginningName, fileCopyAmount, format);
    }
    close(fileD);
    sprintf(endName, "%s%s(%u)", path, beginningName, fileCopyAmount);
}
