#ifndef SIGNAL_H
#define SIGNAL_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	_INT,
	_FLOAT,
	_BUS
} SignalType;

typedef struct Signal {
	SignalType type;
	bool dirty;
	union {
		int _int;
		float _float;
		struct { struct Signal **items; int count; } bus;
	} value;
} Signal;




#define VA_COUNT(...) VA_COUNT_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define VA_COUNT_(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N


#define ADDR(x) &(x)
#define MAP_1(f, a)       f(a)
#define MAP_2(f, a, ...)  f(a), MAP_1(f, __VA_ARGS__)
#define MAP_3(f, a, ...)  f(a), MAP_2(f, __VA_ARGS__)
#define MAP_4(f, a, ...)  f(a), MAP_3(f, __VA_ARGS__)
#define MAP_5(f, a, ...)  f(a), MAP_4(f, __VA_ARGS__)
#define MAP_6(f, a, ...)  f(a), MAP_5(f, __VA_ARGS__)
#define MAP_7(f, a, ...)  f(a), MAP_6(f, __VA_ARGS__)
#define MAP_8(f, a, ...)  f(a), MAP_7(f, __VA_ARGS__)
#define MAP_PICK(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME
#define MAP(f, ...) \
    MAP_PICK(__VA_ARGS__, MAP_8, MAP_7, MAP_6, MAP_5, MAP_4, MAP_3, MAP_2, MAP_1)(f, __VA_ARGS__)




#define SIGNAL_INT(name, val)   Signal name = (Signal){.type = _INT,   .value._int   = (val)}
#define SIGNAL_FLOAT(name, val) Signal name = (Signal){.type = _FLOAT, .value._float = (val)}
#define SIGNAL_BUS(name, ...)   Signal name = (Signal){.type = _BUS, .value.bus = { \
        .items = (Signal*[]){ MAP(ADDR, __VA_ARGS__) }, \
        .count = VA_COUNT(__VA_ARGS__) }}; \
    enum { IMAP(name, __VA_ARGS__) }





#define SIGNAL_CREATE(tag, ...) SIGNAL##tag(__VA_ARGS__)


#define IDXNAME(pre, name)   pre##_##name
#define IMAP_1(pre, a)       IDXNAME(pre, a)
#define IMAP_2(pre, a, ...)  IDXNAME(pre, a), IMAP_1(pre, __VA_ARGS__)
#define IMAP_3(pre, a, ...)  IDXNAME(pre, a), IMAP_2(pre, __VA_ARGS__)
#define IMAP_4(pre, a, ...)  IDXNAME(pre, a), IMAP_3(pre, __VA_ARGS__)
#define IMAP_5(pre, a, ...)  IDXNAME(pre, a), IMAP_4(pre, __VA_ARGS__)
#define IMAP_6(pre, a, ...)  IDXNAME(pre, a), IMAP_5(pre, __VA_ARGS__)
#define IMAP_7(pre, a, ...)  IDXNAME(pre, a), IMAP_6(pre, __VA_ARGS__)
#define IMAP_8(pre, a, ...)  IDXNAME(pre, a), IMAP_7(pre, __VA_ARGS__)
#define IMAP_PICK(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME
#define IMAP(pre, ...) \
    IMAP_PICK(__VA_ARGS__, IMAP_8, IMAP_7, IMAP_6, IMAP_5, IMAP_4, IMAP_3, IMAP_2, IMAP_1)(pre, __VA_ARGS__)


#define GET_2(b, n1)         ((b).value.bus.items[ IDXNAME(b, n1) ])
#define GET_3(b, n1, n2)     ((b).value.bus.items[ IDXNAME(b, n1) ]->value.bus.items[ IDXNAME(n1, n2) ])
#define GET_4(b, n1, n2, n3) ((b).value.bus.items[ IDXNAME(b, n1) ]->value.bus.items[ IDXNAME(n1, n2) ]->value.bus.items[ IDXNAME(n2, n3) ])
#define GET_PICK(_1, _2, _3, _4, NAME, ...) NAME
#define SIGNAL_GET(...) GET_PICK(__VA_ARGS__, GET_4, GET_3, GET_2, GET_1)(__VA_ARGS__)



#define SIGNAL_STORE(sptr, val) \
    ( (sptr)->dirty = true, \
      (sptr)->type == _FLOAT ? (void)((sptr)->value._float = (val)) \
                             : (void)((sptr)->value._int   = (val)) )


#define SET_2(name, val)          SIGNAL_STORE(&(name), val)
#define SET_3(b, n1, val)         SIGNAL_STORE(SIGNAL_GET(b, n1), val)
#define SET_4(b, n1, n2, val)     SIGNAL_STORE(SIGNAL_GET(b, n1, n2), val)
#define SET_5(b, n1, n2, n3, val) SIGNAL_STORE(SIGNAL_GET(b, n1, n2, n3), val)
#define SET_PICK(_1, _2, _3, _4, _5, NAME, ...) NAME
#define SIGNAL_SET(...) SET_PICK(__VA_ARGS__, SET_5, SET_4, SET_3, SET_2, SET_1)(__VA_ARGS__)

#endif
