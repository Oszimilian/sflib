#include "sf.h"
#include "wiring.h"

MODEL_DEF(plant);

SIGNAL_CREATE(_INT, y_next, 0);

static void integrate(Signal* u, Signal* y, Signal* y_next) {
	y_next->value._int = y->value._int + u->value._int;
}

NODE(plant, integrate, IN(u, y), OUT(y_next));
NODE_DELAY(plant, y_next, y);
