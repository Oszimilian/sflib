#ifndef FUNCTION_H
#define FUNCTION_H

#include "port.h"


typedef struct {
	void (*fn)(Port* in, Port* out);
	Port* in;
	Port* out;
} Function;


#define FUNCTION_CREATE(fn, in, out) \
	Function fn##_f = { (fn), &(in), &(out) }


#define FUNCTION_RUN(name)  ((name##_f).fn((name##_f).in, (name##_f).out))

#endif
