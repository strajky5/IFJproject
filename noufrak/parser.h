#ifndef PAR_H
#define PAR_H

#include <stdio.h>
#include "scaner.h"
#include "str.h"
#include "errors.h"
#include "ial.h"
#include "expressions.h"

extern FILE*f;
extern tToken T;
extern tOperand res;
int parser();

#endif

