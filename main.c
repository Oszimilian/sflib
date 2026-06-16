#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "signal.h"
#include "port.h"
#include "function.h"


void my_function(Port* in, Port* out) {
	PORT_DEF(in, stream, b);
	PORT_DEF(stream, enable, value);
	PORT_DEF(out, z);

	if (PORT_GET(in, stream, enable)->value._int == 0) {
		PORT_SET(out, z, 0);
	} else {
		int sum = PORT_GET(in, b)->value._int
		        + PORT_GET(in, stream, value)->value._int;
		PORT_SET(out, z, sum);
	}
}

int main(int arg, char** argv) {
	SIGNAL_CREATE(_INT, enable, 1);
	SIGNAL_CREATE(_INT, value, 5);
	SIGNAL_CREATE(_BUS, stream, enable, value);


	SIGNAL_CREATE(_INT, b, 7);
	SIGNAL_CREATE(_INT, z, 0);


	PORT_CREATE(pi, stream, b);
	PORT_CREATE(po, z);

	FUNCTION_CREATE(my_function, pi, po);

	FUNCTION_RUN(my_function);

	printf("z = %d\n", SIGNAL_GET(po, z)->value._int);

	return 0;
}
