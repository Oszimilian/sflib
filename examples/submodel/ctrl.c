#include "sf.h"
#include "wiring.h"

MODEL_DEF(ctrl);

SIGNAL_CREATE(_INT, err, 0);

static void compute_error(Signal* target, Signal* y, Signal* err) {
	err->value._int = target->value._int - y->value._int;
}

static void apply_gain(Signal* err, Signal* u) {
	u->value._int = err->value._int / 4;
}

NODE(ctrl, compute_error, IN(target, y), OUT(err));
NODE(ctrl, apply_gain,    IN(err),       OUT(u));
