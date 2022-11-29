#ifndef RECURSIVE_DESCENT_H_INCLUDED
#define RECURSIVE_DESCENT_H_INCLUDED

#include "differ.h"

int GetG(tree_t* tree, const char* str);

size_t TypeOfTrigF(const char** s);

elem_s* GetDeg();

elem_s* GetX();

elem_s* GetN();

elem_s* GetE();

elem_s* GetT();

elem_s* GetP();

#endif   //guard
