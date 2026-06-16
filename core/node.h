#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include "signal.h"



typedef struct Node {
	void (*run)(void);
	void (*latch)(void);
	Signal** ins;
	int      n_in;
	Signal** outs;
	int      n_out;
	void*    state;
} Node;


#define IN(...)  (__VA_ARGS__)
#define OUT(...) (__VA_ARGS__)

#define SF_STRIP(...) __VA_ARGS__

#define SF_CAT(a, b)  SF_CAT_(a, b)
#define SF_CAT_(a, b) a##b

#ifndef SF_MAX_NODES
#define SF_MAX_NODES 256
#endif

typedef struct Model {
	Node* nodes[SF_MAX_NODES];
	int   count;
} Model;

#define MODEL_DEF(name) Model name = { .count = 0 }

static inline void sf_register(Model* m, Node* n) { m->nodes[m->count++] = n; }

#define NODE(model, fn, ins, outs)          SF_NODE_E(model, SF_CAT(sfnode_, __COUNTER__), fn, ins, outs)
#define SF_NODE_E(model, id, fn, ins, outs) SF_NODE(model, id, fn, ins, outs)
#define SF_NODE(model, id, fn, ins, outs) \
	static Signal* id##_in[]  = { MAP(ADDR, SF_STRIP ins) }; \
	static Signal* id##_out[] = { MAP(ADDR, SF_STRIP outs) }; \
	static void id##_run(void) { fn( MAP(ADDR, SF_STRIP ins), MAP(ADDR, SF_STRIP outs) ); } \
	static Node id = { id##_run, NULL, id##_in, VA_COUNT(SF_STRIP ins), id##_out, VA_COUNT(SF_STRIP outs), NULL }; \
	__attribute__((constructor)) static void id##_reg(void) { sf_register(&model, &id); }

#define NODE_S(model, fn, StateT, ins, outs)          SF_NODES_E(model, SF_CAT(sfnode_, __COUNTER__), fn, StateT, ins, outs)
#define SF_NODES_E(model, id, fn, StateT, ins, outs)  SF_NODES(model, id, fn, StateT, ins, outs)
#define SF_NODES(model, id, fn, StateT, ins, outs) \
	static StateT id##_state; \
	static Signal* id##_in[]  = { MAP(ADDR, SF_STRIP ins) }; \
	static Signal* id##_out[] = { MAP(ADDR, SF_STRIP outs) }; \
	static void id##_run(void) { fn( &id##_state, MAP(ADDR, SF_STRIP ins), MAP(ADDR, SF_STRIP outs) ); } \
	static Node id = { id##_run, NULL, id##_in, VA_COUNT(SF_STRIP ins), id##_out, VA_COUNT(SF_STRIP outs), &id##_state }; \
	__attribute__((constructor)) static void id##_reg(void) { sf_register(&model, &id); }

typedef struct { int value; } DelayState;

static inline void sf_delay_out(void* st, Signal* out)  { out->value._int = ((DelayState*)st)->value; }
static inline void sf_delay_latch(void* st, Signal* in) { ((DelayState*)st)->value = in->value._int; }

#define NODE_DELAY(model, in_sig, out_sig)         SF_NODED_E(model, SF_CAT(sfnode_, __COUNTER__), in_sig, out_sig)
#define SF_NODED_E(model, id, in_sig, out_sig)     SF_NODED(model, id, in_sig, out_sig)
#define SF_NODED(model, id, in_sig, out_sig) \
	static DelayState id##_state; \
	static Signal* id##_in[]  = { &(in_sig) }; \
	static Signal* id##_out[] = { &(out_sig) }; \
	static void id##_run(void)   { sf_delay_out(&id##_state, &(out_sig)); } \
	static void id##_latch(void) { sf_delay_latch(&id##_state, &(in_sig)); } \
	static Node id = { id##_run, id##_latch, id##_in, 1, id##_out, 1, &id##_state }; \
	__attribute__((constructor)) static void id##_reg(void) { sf_register(&model, &id); }


#define MODEL_RUN(m) \
	do { \
		for (int _i = 0; _i < (m)->count; _i++) (m)->nodes[_i]->run(); \
		for (int _i = 0; _i < (m)->count; _i++) if ((m)->nodes[_i]->latch) (m)->nodes[_i]->latch(); \
	} while (0)


static inline int sf_any_input_dirty(Node* nd) {
	for (int i = 0; i < nd->n_in; i++)
		if (nd->ins[i]->dirty) return 1;
	return 0;
}

static inline void model_step(Model* m) {
	for (int i = 0; i < m->count; i++) {
		Node* nd = m->nodes[i];
		if (nd->latch || sf_any_input_dirty(nd)) {
			nd->run();
			for (int k = 0; k < nd->n_out; k++)
				nd->outs[k]->dirty = true;
		}
	}

	for (int i = 0; i < m->count; i++)
		if (m->nodes[i]->latch) m->nodes[i]->latch();

	for (int i = 0; i < m->count; i++) {
		Node* nd = m->nodes[i];
		for (int k = 0; k < nd->n_in;  k++) nd->ins[k]->dirty  = false;
		for (int k = 0; k < nd->n_out; k++) nd->outs[k]->dirty = false;
	}
}

#define MODEL_STEP(m)  model_step(m)


static inline int sf_produces(Node* p, Signal* s) {
	for (int k = 0; k < p->n_out; k++)
		if (p->outs[k] == s) return 1;
	return 0;
}


static inline int sf_depends(Node* a, Node* b) {
	if (a->latch) return 0;
	for (int i = 0; i < a->n_in; i++)
		if (sf_produces(b, a->ins[i])) return 1;
	return 0;
}

static inline void calc_scheduling(Model* m) {
	int n = m->count;
	Node* result[n];
	int   placed[n];
	for (int i = 0; i < n; i++) placed[i] = 0;

	for (int step = 0; step < n; step++) {
		int pick = -1;
		for (int i = 0; i < n && pick < 0; i++) {
			if (placed[i]) continue;
			int ready = 1;
			for (int j = 0; j < n; j++) {
				if (j == i || placed[j]) continue;
				if (sf_depends(m->nodes[i], m->nodes[j])) { ready = 0; break; }
			}
			if (ready) pick = i;
		}
		if (pick < 0) {
			fprintf(stderr, "calc_scheduling: Zyklus (algebraische Schleife) erkannt!\n");
			return;
		}
		result[step] = m->nodes[pick];
		placed[pick] = 1;
	}
	for (int i = 0; i < n; i++) m->nodes[i] = result[i];
}

#endif
