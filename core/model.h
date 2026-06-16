#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include "function.h"

#ifndef MODEL_MAX
#define MODEL_MAX 64
#endif

typedef struct {
	Function* blocks[MODEL_MAX];
	int       n;
	Function* order[MODEL_MAX];
	int       n_order;
} Model;


#define BLOCKREF(name)  &name##_f
#define MODEL(...) (Model){ .blocks = { MAP(BLOCKREF, __VA_ARGS__) }, \
                            .n = VA_COUNT(__VA_ARGS__) }


static inline int sf_signal_in_port(Port* p, Signal* s) {
	for (int i = 0; i < p->value.bus.count; i++)
		if (p->value.bus.items[i] == s) return 1;
	return 0;
}


static inline int sf_depends(Function* b, Function* a) {
	Port* in = b->in;
	for (int i = 0; i < in->value.bus.count; i++)
		if (sf_signal_in_port(a->out, in->value.bus.items[i]))
			return 1;
	return 0;
}


static inline void calc_scheduling(Model* m) {
	int indegree[MODEL_MAX] = {0};
	int done[MODEL_MAX]     = {0};

	for (int b = 0; b < m->n; b++)
		for (int a = 0; a < m->n; a++)
			if (a != b && sf_depends(m->blocks[b], m->blocks[a]))
				indegree[b]++;

	m->n_order = 0;
	for (int iter = 0; iter < m->n; iter++) {
		int pick = -1;
		for (int b = 0; b < m->n; b++)
			if (!done[b] && indegree[b] == 0) { pick = b; break; }

		if (pick < 0) break;   /* kein In-Grad-0 mehr -> Zyklus */

		done[pick] = 1;
		m->order[m->n_order++] = m->blocks[pick];

		for (int b = 0; b < m->n; b++)
			if (!done[b] && sf_depends(m->blocks[b], m->blocks[pick]))
				indegree[b]--;
	}

	if (m->n_order < m->n)
		fprintf(stderr,
		        "calc_scheduling: Zyklus (algebraische Schleife) erkannt - "
		        "%d/%d Bloecke planbar.\n", m->n_order, m->n);
}


#define MODEL_RUN(m) \
	do { for (int _i = 0; _i < (m)->n_order; _i++) \
	         (m)->order[_i]->fn((m)->order[_i]->in, (m)->order[_i]->out); } while (0)

#endif
