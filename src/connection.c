

#include "connection.h"

Connection *start_connection(Pin *pin1) {
    assert(pin1 != NULL);
    Connection *con = malloc(sizeof(Connection));
    con->p1 = pin1;
    con->p2 = NULL;
    con->state = 0;
    con->points = array_create(4);
    if (con->points == NULL) {
        free(con);
        return NULL;
    }

    return con;
}

void finish_conection(Connection *con, Pin *pin2) {
    assert(con != NULL);
    assert(pin2 != NULL);

    con->p2 = pin2;

    while (con->points->size < 4) {
        add_connection_point(con, pin2->x + pin2->parent_node->rect.x, pin2->y + pin2->parent_node->rect.y);
    }

    propagate_state(con);
    correct_connection_points(con);
}

void correct_connection_points(Connection *con) {
    assert(con != NULL);
    assert(con->points != NULL);

    int size = con->points->size;
    if (size < 2) return;
    
    int p1_y = con->p1->y + con->p1->parent_node->rect.y + (PIN_SIZE / 2);
    int p2_y = con->p2->y + con->p2->parent_node->rect.y + (PIN_SIZE / 2);
    
    for (int i = 0; i < size && i < 2; i++) {
        SDL_Point *point = array_get(con->points, i);
        point->y = p1_y;
    }

    for (int i = size - 2; i < size; i++) {
        if (i >= 0) {
            SDL_Point *point = array_get(con->points, i);
            point->y = p2_y;
        }
    }
}

void update_connection_points(SimulationState *state, Connection *con) {
    assert(con != NULL);
    assert(con->points != NULL);

    int size = con->points->size;

    Node *parent1 = con->p1->parent_node;
    Node *parent2 = con->p2->parent_node;

    SDL_Point *first = array_get(con->points, 0);
    first->x = parent1->rect.x + con->p1->x + (PIN_SIZE / 2);
    first->y = parent1->rect.y + con->p1->y + (PIN_SIZE / 2);

    SDL_Point *second = array_get(con->points, 1);
    second->y = parent1->rect.y + con->p1->y + (PIN_SIZE / 2);
    
    SDL_Point *last = array_get(con->points, size - 1);
    last->x = parent2->rect.x + con->p2->x + (PIN_SIZE / 2);
    last->y = parent2->rect.y + con->p2->y + (PIN_SIZE / 2);

    SDL_Point *second_to_last = array_get(con->points, size - 2);
    second_to_last->y = parent2->rect.y + con->p2->y + (PIN_SIZE / 2);

    for (int i = 0; i < size; i++) {
        SDL_Point *current_point = array_get(con->points, i);

        if (i == 0 || i == size - 1) {
            continue;
        }

        SDL_Point *before = array_get(con->points, i - 1);
        SDL_Point *after = array_get(con->points, i + 1);

        if (current_point == state->dragging_connection_point) {

            if (i % 2 == 0) {
                before->x = state->dragging_connection_point->x;
                after->y = state->dragging_connection_point->y;
            }
            else {
                before->y = state->dragging_connection_point->y;
                after->x = state->dragging_connection_point->x;
            }
        }
    }
}

void add_connection_point(Connection *con, int x, int y) {
    assert(con != NULL);
    assert(con->points != NULL);

    SDL_Point *point = malloc(sizeof(SDL_Point));
    if (point == NULL) {
        return;
    }

    point->x = x;
    point->y = y;

    array_add(con->points, point);
}

void pop_connection_point(Connection *con) {
    assert(con != NULL);

    array_remove_at(con->points, con->points->size);
}

void set_input_zero(Connection *con) {
    assert(con != NULL);
    assert(con->p1 != NULL && con->p2 != NULL);
    assert(con->p1->is_input != con->p2->is_input);

    Pin *in_pin = con->p1->is_input ? con->p1 : con->p2;

    in_pin->state = 0;
}

void propagate_state(Connection *con) {
    assert(con != NULL);
    assert(con->p1 != NULL && con->p2 != NULL);
    assert(con->p1->is_input != con->p2->is_input);

    Pin *out_pin = con->p1->is_input ? con->p2 : con->p1;
    Pin *in_pin  = con->p1->is_input ? con->p1 : con->p2;

    in_pin->state |= out_pin->state;
    con->state = out_pin->state;
}

void free_connection(Connection *con) {
    assert(con != NULL);

    if (con->points != NULL) {
        array_free(con->points);
        con->points = NULL;
    }
    free(con); 
}

void print_connection(Connection *con) {
    assert(con != NULL);

    if (!con || !con->p1 || !con->p2) {
        printf("Invalid connection (NULL pointer)\n");
        return;
    }

    Node *from_node = con->p1->parent_node;
    Node *to_node   = con->p2->parent_node;

    printf("Connection (%p):\n", (void *)con);
    printf("  Node: %s (%p) | Pin (%p) State: %d | Pin Type: %s\n",
           from_node ? from_node->name : "NULL",
           (void *)from_node,
           (void *)con->p1,
           con->p1->state,
           con->p1->is_input ? "INPUT" : "OUTPUT");

    printf("  Node:   %s (%p) | Pin (%p) State: %d | Pin Type: %s\n",
           to_node ? to_node->name : "NULL",
           (void *)to_node,
           (void *)con->p2,
           con->p2->state,
           con->p2->is_input ? "INPUT" : "OUTPUT");

    printf("  Cable State: %d\n", con->state);

    printf("  SDL_Points:\n");
    for (int i = 0; i < con->points->size; i++)
    {
        SDL_Point *point = (SDL_Point *)array_get(con->points, i);
        printf("    SDL_Point %d (x: %d, y: %d)\n", i, point->x, point->y);
    }
    
    printf("--------------------------\n");
}
