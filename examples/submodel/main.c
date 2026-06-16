#include <stdio.h>

#include "sf.h"
#include "wiring.h"

SIGNAL_CREATE(_INT, target, 100);
SIGNAL_CREATE(_INT, y, 0);
SIGNAL_CREATE(_INT, u, 0);

int main(void) {
	calc_scheduling(&ctrl);
	calc_scheduling(&plant);

	for (int i = 0; i < 12; i++) {
		MODEL_RUN(&ctrl);
		MODEL_RUN(&plant);
		printf("tick %2d: target=%d  y=%d  u=%d\n",
		       i, target.value._int, y.value._int, u.value._int);
	}
	return 0;
}
