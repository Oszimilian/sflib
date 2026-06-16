#include <stdio.h>

#include "sf.h"

void inc(Signal* count, Signal* count_next) {
	count_next->value._int = count->value._int + 1;
}

typedef struct { int sum; } AccState;

void accumulate(void* st, Signal* in, Signal* out) {
	AccState* s = st;
	s->sum += in->value._int;
	out->value._int = s->sum;
}

MODEL_DEF(m);

SIGNAL_CREATE(_INT, count, 0);
SIGNAL_CREATE(_INT, count_next, 0);
SIGNAL_CREATE(_INT, x, 2);
SIGNAL_CREATE(_INT, acc_out, 0);

NODE(m, inc, IN(count), OUT(count_next));
NODE_DELAY(m, count_next, count);
NODE_S(m, accumulate, AccState, IN(x), OUT(acc_out));

int main(void) {
	calc_scheduling(&m);

	for (int i = 0; i < 6; i++) {
		MODEL_RUN(&m);
		printf("tick %d: count=%d  acc_out=%d\n",
		       i, count.value._int, acc_out.value._int);
	}
	return 0;
}
