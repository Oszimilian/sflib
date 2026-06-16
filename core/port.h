#ifndef PORT_H
#define PORT_H

#include "signal.h"


typedef Signal Port;

#define PORT_CREATE(name, ...)  SIGNAL_CREATE(_BUS, name, __VA_ARGS__)

#define PORT_DEF(name, ...)     enum { IMAP(name, __VA_ARGS__) }

#define PGET_1(p, n1)        ((p)->value.bus.items[ IDXNAME(p, n1) ])
#define PGET_2(p, n1, n2)    ((p)->value.bus.items[ IDXNAME(p, n1) ]->value.bus.items[ IDXNAME(n1, n2) ])
#define PGET_PICK(_1, _2, _3, NAME, ...) NAME
#define PORT_GET(...)  PGET_PICK(__VA_ARGS__, PGET_2, PGET_1, PGET_0)(__VA_ARGS__)

#define PSET_2(p, n1, val)       SIGNAL_STORE(PORT_GET(p, n1), val)
#define PSET_3(p, n1, n2, val)   SIGNAL_STORE(PORT_GET(p, n1, n2), val)
#define PSET_PICK(_1, _2, _3, _4, NAME, ...) NAME
#define PORT_SET(...)  PSET_PICK(__VA_ARGS__, PSET_3, PSET_2, PSET_1, PSET_0)(__VA_ARGS__)

#endif