#ifndef EXAMPLE_MODULE_H
#define EXAMPLE_MODULE_H

#include "../signal.h"
#include "../port.h"
#include "../function.h"

/* Modul-Funktion: liest stream(enable,value) + b, schreibt z. */
void my_function(Port* in, Port* out);

#endif /* EXAMPLE_MODULE_H */
