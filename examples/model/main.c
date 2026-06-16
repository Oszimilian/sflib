#include <stdio.h>

#include "sf.h"


void f1(Signal* a1, Signal* o1) {            /* o1 = a1 * 2  */
	printf("[f1] ");
	o1->value._int = a1->value._int * 2;
}

void f2(Signal* a2, Signal* o2) {            /* o2 = a2 + 10 */
	printf("[f2] ");
	o2->value._int = a2->value._int + 10;
}

void f3(Signal* o1, Signal* o2, Signal* o3) {/* o3 = o1 + o2 */
	printf("[f3] ");
	o3->value._int = o1->value._int + o2->value._int;
}

void f4(Signal* a4, Signal* o4) {            /* o4 = a4 * 4  */
	printf("[f4] ");
	o4->value._int = a4->value._int * 4;
}

SIGNAL_CREATE(_INT, a1, 3);
SIGNAL_CREATE(_INT, a2, 4);
SIGNAL_CREATE(_INT, a4, 0);
SIGNAL_CREATE(_INT, o1, 0);
SIGNAL_CREATE(_INT, o2, 0);
SIGNAL_CREATE(_INT, o3, 0);
SIGNAL_CREATE(_INT, o4, 0);


MODEL_DEF(m);

NODE(m, f1, IN(a1),     OUT(o1));
NODE(m, f2, IN(a2),     OUT(o2));
NODE(m, f3, IN(o1, o2), OUT(o3));
NODE(m, f4, IN(a4),     OUT(o4));

static void show(void) {
	printf("=> o1=%d o2=%d o3=%d o4=%d\n",
	       o1.value._int, o2.value._int, o3.value._int, o4.value._int);
}

int main(void) {
	calc_scheduling(&m);

	/* 1) Kaltstart: alle Eingaenge setzen -> alles rechnet einmal durch. */
	printf("set a1,a2,a4:   ");  SIGNAL_SET(a1, 3); SIGNAL_SET(a2, 4); SIGNAL_SET(a4, 1);
	MODEL_STEP(&m);  show();

	/* 2) Nur a4 neu -> NUR f4 laeuft (o1/o2/o3 bleiben). */
	printf("set a4=5:       ");  SIGNAL_SET(a4, 5);
	MODEL_STEP(&m);  show();

	/* 3) Nur a1 neu -> f1 laeuft, o1 aendert sich -> f3 laeuft nach.
	 *    f2 und f4 bleiben aus. */
	printf("set a1=10:      ");  SIGNAL_SET(a1, 10);
	MODEL_STEP(&m);  show();

	/* 4) Nichts gesetzt -> kein Knoten laeuft. */
	printf("nichts gesetzt: ");
	MODEL_STEP(&m);  show();

	return 0;
}
