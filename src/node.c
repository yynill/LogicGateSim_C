#include "node.h"
#include "renderer.h"
#include "assert.h"

Pin create_pin(int x, int y, Node *parent_node) {
    Pin p;
    p.x = x;
    p.y = y;
    p.parent_node = parent_node;
    return p;
}

int insert_node(SimulationState *state, int num_inputs, int num_outputs, Operation op, SDL_Rect rect, const char *name) {
    assert(state != NULL);
    assert(num_inputs >= 0 && num_outputs >= 0);
    assert(rect.w > 0 && rect.h > 0);
    assert(name != NULL);

    Node *node = array_add_uninitialized(state->nodes);
    if (!node) {
        return 0;
    }

    node->inputs = array_create(2, sizeof(Pin));
    if (!node->inputs) {
        array_remove_last(state->nodes);
        return 0;
    }

    node->outputs = array_create(2, sizeof(Pin));
    if (!node->outputs) {
        array_free(node->inputs);
        array_remove_last(state->nodes);
        return 0;
    }

    float spacing = 0.5f * PIN_SIZE;
    float total_inputs_height = num_inputs * PIN_SIZE + (num_inputs - 1) * spacing;
    float start_y_inputs = (rect.h / 2.0f) - (total_inputs_height / 2.0f);

    for (int i = 0; i < num_inputs; i++) {
        Pin p = create_pin(-PIN_SIZE / 2, start_y_inputs + i * (PIN_SIZE + spacing), node);
        array_add(node->inputs, &p);
    }

    float total_outputs_height = num_outputs * PIN_SIZE + (num_outputs - 1) * spacing;
    float start_y_outputs = (rect.h / 2.0f) - (total_outputs_height / 2.0f);

    for (int i = 0; i < num_outputs; i++) {
        Pin p = create_pin(rect.w - PIN_SIZE / 2, start_y_outputs + i * (PIN_SIZE + spacing), node);
        array_add(node->outputs, &p);
    }

    node->operation = op;
    node->name = name;
    node->rect = rect;

    return 1;
}
