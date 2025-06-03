#include "node.h"
#include "renderer.h"
#include <assert.h>

Connection *create_connection(Pin *pin1, Pin *pin2) {
    Connection *con = malloc(sizeof(Connection));
    con->p1 = pin1;
    con->p2 = pin2;
    con->state = 0;
    propagate_state(con);

    return con;
}


Pin *create_pin(int x, int y, int ii, Node *parent_node) {
    Pin *p = malloc(sizeof(Pin));
    assert(p != NULL);
    p->x = x;
    p->y = y;
    p->is_input = ii;
    p->parent_node = parent_node;
    p->state = 0;
    return p;
}

Node *create_node(int num_inputs, int num_outputs, Operation op, SDL_Rect rect, const char *name) {
    assert(num_inputs >= 0 && num_outputs >= 0);
    assert(rect.w > 0 && rect.h > 0);
    assert(name != NULL);

    Node *node = malloc(sizeof(Node));
    if (!node) {
        return 0;
    }

    node->inputs = array_create(2);
    if (!node->inputs) {
        return 0;
    }

    node->outputs = array_create(2);
    if (!node->outputs) {
        array_free_with_elements(node->inputs);
        return 0;
    }

    float spacing = 0.5f * PIN_SIZE;
    float total_inputs_height = num_inputs * PIN_SIZE + (num_inputs - 1) * spacing;
    float start_y_inputs = (rect.h / 2.0f) - (total_inputs_height / 2.0f);

    for (int i = 0; i < num_inputs; i++) {
        Pin *p = create_pin(-PIN_SIZE / 2, start_y_inputs + i * (PIN_SIZE + spacing), 1, node);
        array_add(node->inputs, p);
    }

    float total_outputs_height = num_outputs * PIN_SIZE + (num_outputs - 1) * spacing;
    float start_y_outputs = (rect.h / 2.0f) - (total_outputs_height / 2.0f);

    for (int i = 0; i < num_outputs; i++) {
        Pin *p = create_pin(rect.w - PIN_SIZE / 2, start_y_outputs + i * (PIN_SIZE + spacing), 0, node);
        array_add(node->outputs, p);
    }

    node->operation = op;
    node->name = name;
    node->rect = rect;

    run_node(node);

    return node;
}

void run_node(Node *node)
{
    assert(node != NULL);
    assert(node->inputs != NULL);
    assert(node->outputs != NULL);
    assert(node->operation != NULL);

    if (node->inputs->size == 0 || node->outputs->size == 0) return;

    assert(node->inputs->size >= 1);

    Pin *p1 = (Pin *)array_get(node->inputs, 0);
    assert(p1 != NULL);
    assert(p1->is_input);
    int a = p1->state;

    int b = 0;
    if (node->inputs->size > 1) {
        Pin *p2 = (Pin *)array_get(node->inputs, 1);
        assert(p2 != NULL);
        assert(p2->is_input);
        b = p2->state;
    }

    int result = node->operation(a, b);


    for (int i = 0; i < node->outputs->size; i++) {
        Pin *out_pin = (Pin *)array_get(node->outputs, i);
        assert(out_pin != NULL);
        assert(!out_pin->is_input);
        out_pin->state = result;
    }
}


void propagate_state(Connection *con) {
    assert(con != NULL);
    assert(con->p1 != NULL && con->p2 != NULL);
    assert(con->p1->is_input != con->p2->is_input);

    Pin *out_pin = con->p1->is_input ? con->p2 : con->p1;
    Pin *in_pin  = con->p1->is_input ? con->p1 : con->p2;

    in_pin->state = out_pin->state;
    con->state = out_pin->state;
}

void print_node(Node *node) {
    if (!node) {
        printf("Invalid Node (NULL pointer)\n");
        return;
    }

    printf("Node (%p): %s\n", (void *)node, node->name);
    printf("  Position: (%d, %d) Size: (%d x %d)\n",
           node->rect.x, node->rect.y, node->rect.w, node->rect.h);

    // Inputs
    printf("  Inputs (%d):\n", node->inputs->size);
    for (int i = 0; i < node->inputs->size; i++) {
        Pin *pin = array_get(node->inputs, i);
        printf("    Input %d (%p) -> State: %d | Pos: (%d, %d)\n",
               i, (void *)pin, pin->state, pin->x, pin->y);
    }

    // Outputs
    printf("  Outputs (%d):\n", node->outputs->size);
    for (int i = 0; i < node->outputs->size; i++) {
        Pin *pin = array_get(node->outputs, i);
        printf("    Output %d (%p) -> State: %d | Pos: (%d, %d)\n",
               i, (void *)pin, pin->state, pin->x, pin->y);
    }

    printf("--------------------------------\n");
}

void print_connection(Connection *con) {
    if (!con || !con->p1 || !con->p2) {
        printf("Invalid connection (NULL pointer)\n");
        return;
    }

    Node *from_node = con->p1->parent_node;
    Node *to_node   = con->p2->parent_node;

    printf("Connection (%p):\n", (void *)con);
    printf("  From Node: %s (%p) | Pin (%p) State: %d | Pin Type: %s\n",
           from_node ? from_node->name : "NULL",
           (void *)from_node,
           (void *)con->p1,
           con->p1->state,
           con->p1->is_input ? "INPUT" : "OUTPUT");

    printf("  To Node:   %s (%p) | Pin (%p) State: %d | Pin Type: %s\n",
           to_node ? to_node->name : "NULL",
           (void *)to_node,
           (void *)con->p2,
           con->p2->state,
           con->p2->is_input ? "INPUT" : "OUTPUT");

    printf("  Cable State: %d\n", con->state);
    printf("--------------------------\n");
}
