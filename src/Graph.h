#pragma once

#include <fcntl.h>
#include <sys\types.h>

#define GRAPHS_PATH "graphs\\"

const int32_t MAX_PATH_FILE_LENGTH = 100;
const int32_t MAX_COMMAND_NAME     = 100;

void MakeListGraph(List* list, char name[]);
void GenerateOutputGraphName(char beginningName[], char endName[]);
