#include <stdio.h>

#include "example_module.h"

/* z = 0, wenn enable == 0; sonst z = b + value. */
void my_function(Port* in, Port* out) {
	PORT_DEF(in, stream, b);          /* in-Slot 0 = stream (Bus), Slot 1 = b */
	PORT_DEF(stream, enable, value);  /* Schnittstelle des verschachtelten Bus */
	PORT_DEF(out, z);

	if (PORT_GET(in, stream, enable)->value._int == 0)
		PORT_SET(out, z, 0);
	else
		PORT_SET(out, z, PORT_GET(in, b)->value._int
		               + PORT_GET(in, stream, value)->value._int);
}

int main(void) {
	SIGNAL_CREATE(_INT, enable, 1);
	SIGNAL_CREATE(_INT, value, 5);
	SIGNAL_CREATE(_INT, b, 7);
	SIGNAL_CREATE(_INT, z, 0);

	SIGNAL_CREATE(_BUS, stream, enable, value);

	PORT_CREATE(pi, stream, b);
	PORT_CREATE(po, z);

	FUNCTION_CREATE(my_function, pi, po);

	FUNCTION_RUN(my_function);
	printf("enable=1 -> z = %d\n", SIGNAL_GET(po, z)->value._int);   /* 7 + 5 = 12 */

	SIGNAL_SET(enable, 0);
	FUNCTION_RUN(my_function);
	printf("enable=0 -> z = %d\n", SIGNAL_GET(po, z)->value._int);   /* 0 */

	return 0;
}
