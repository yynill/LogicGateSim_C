#include "node.h"
#include "renderer.h"

Node *create_node(int num_inputs, int num_outputs, Operation op, SDL_Rect rect, const char *name)
{
    Node *node = malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Failed to allocate Node.\n");
        exit(EXIT_FAILURE);
    }

    node->inputs = array_create(2, sizeof(Pin));
    node->outputs = array_create(2, sizeof(Pin));

    float spacing = 0.5f * PIN_SIZE;
    float total_inputs_height = num_inputs * PIN_SIZE + (num_inputs - 1) * spacing;
    float start_y_inputs = (rect.h / 2.0f) - (total_inputs_height / 2.0f);

    for (int i = 0; i < num_inputs; i++) {
        Pin p;
        p.x = -PIN_SIZE/2; // left edge
        p.y = start_y_inputs + i * (PIN_SIZE + spacing);
        array_add(node->inputs, &p);
    }

    float total_outputs_height = num_outputs * PIN_SIZE + (num_outputs - 1) * spacing;
    float start_y_outputs = (rect.h / 2.0f) - (total_outputs_height / 2.0f);

    for (int i = 0; i < num_outputs; i++) {
        Pin p;
        p.x = rect.w - PIN_SIZE / 2; // right edge
        p.y = start_y_outputs + i * (PIN_SIZE + spacing);
        array_add(node->outputs, &p);
    }

    node->operation = op;
    node->name = name;
    node->rect = rect;

    return node;
}
