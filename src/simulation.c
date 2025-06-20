#include "simulation.h"

SimulationState *simulation_init(void) {
    SimulationState *state = malloc(sizeof(SimulationState));
    if (!state) return NULL;

    state->is_running = 1;

    state->popup_state = init_popupstate();

    state->nodes = array_create(16);
    assert(state->nodes != NULL);

    state->connections = array_create(16);
    assert(state->connections != NULL);

    state->buttons = array_create(16);
    assert(state->buttons != NULL);

    state->knife_stroke = array_create(16);
    assert(state->knife_stroke != NULL);

    state->selected_nodes = array_create(16);
    assert(state->selected_nodes != NULL);

    state->selected_connection_points = array_create(16);
    assert(state->selected_connection_points != NULL);

    init_buttons(state);

    state->left_mouse_down = 0;
    state->right_mouse_down = 0;
    state->middle_mouse_down = 0;

    state->mouse_x = 0;
    state->mouse_y = 0;
    state->mouse_wheel = 0;
    state->drag_offset_x = 0;
    state->drag_offset_y = 0;

    state->is_selection_box_drawing = 0;
    state->selection_box = (SDL_Rect){0, 0, 0, 0};

    state->is_node_dragging = 0;
    state->is_camera_dragging = 0;
    state->is_cable_dragging = 0;
    state->is_connection_point_dragging = 0;

    state->camera_x = 0;
    state->camera_y = 0;
    state->camera_zoom = 1;

    state->is_paused = 1;

    state->hovered_connection_point = NULL;
    state->dragging_connection_point = NULL;
    state->new_connection = NULL;

    state->dragged_node = NULL;

    state->last_node_x = 0;
    state->last_node_y = 0;

    state->hovered_pin = NULL;
    state->first_selected_pin = NULL;

    return state;
}

void init_buttons(SimulationState *state) {
    array_add(state->buttons, create_button((SDL_Rect){10, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "NOT", notGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){80, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "AND", andGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){150, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "OR", orGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){220, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "XOR", xorGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){290, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "XNOR", xnorGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){360, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "NOR", norGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){430, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "NAND", nandGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){550, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "LIGHT", nullGate, add_node));
    array_add(state->buttons, create_button((SDL_Rect){620, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "SWITCH", nullGate, add_node));

    array_add(state->buttons, create_button((SDL_Rect){WINDOW_WIDTH - 4 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/step_back.png", nullGate, null_function));
    array_add(state->buttons, create_button((SDL_Rect){WINDOW_WIDTH - 3 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/play.png", nullGate, toggle_play_pause));
    array_add(state->buttons, create_button((SDL_Rect){WINDOW_WIDTH - 2 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/step_forth.png", nullGate, one_step));
    array_add(state->buttons, create_button((SDL_Rect){WINDOW_WIDTH - 1 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/reload.png", nullGate, reset_sim));
}

void simulation_cleanup(SimulationState *state) {
    assert(state != NULL);
    
    if (state) {
        for (int i = 0; i < state->nodes->size; i++) {
            free_node((Node*)array_get(state->nodes, i));
        }
        free(state->nodes); 


        for (int i = 0; i < state->connections->size; i++) {
            free_connection((Connection*)array_get(state->connections, i));
        }
        free(state->connections);

        array_free(state->buttons);
        array_free(state->knife_stroke);
        free(state);
    }
}

void reset_sim(SimulationState *state, void *function_data) {
    assert(state != NULL);
    (void)function_data;
    state->should_reset = 1;
}

void simulation_update(SimulationState *state) {
    assert(state != NULL);

    if (!state->is_paused) {
        one_step(state, "");
    }
}

void null_function(SimulationState *state, void *function_data) {
    (void)state;
    (void)function_data;
    printf("// todo\n");
    return;
}

void add_node(SimulationState *state, void *function_data) {
    assert(state != NULL);
    assert(function_data != NULL);
    assert(state->nodes != NULL);

    Button *button = (Button *)function_data;
    Operation *op = (Operation *)button->function_data;
    SDL_Rect node_rect = {.x = 100, .y = 100, .w = NODE_WIDTH, .h = NODE_HEIGHT};

    int num_inputs = 2;
    int num_outputs = 1;
    if (strcmp(button->name, "NOT") == 0) {
        num_inputs = 1;
    }
    if (strcmp(button->name, "SWITCH") == 0) {
        num_inputs = 0;
        node_rect.w = SMALL_NODE_WIDTH;
    }
    if (strcmp(button->name, "LIGHT") == 0) {
        num_inputs = 1;
        num_outputs = 0;
        node_rect.w = SMALL_NODE_WIDTH;
    }

    array_add(state->nodes, create_node(num_inputs, num_outputs, op, node_rect, button->name));
}

void cut_connection(SimulationState *state) {
    assert(state != NULL);
    assert(state->connections != NULL);
    assert(state->knife_stroke != NULL);

    for (int i = 0; i < state->knife_stroke->size - 1; i++) {
        SDL_Point *knive_p1 = array_get(state->knife_stroke, i);
        SDL_Point *knive_p2 = array_get(state->knife_stroke, i + 1);
        for (int j = state->connections->size - 1; j >= 0; j--) {
            Connection *con = array_get(state->connections, j);
            if (con != NULL && con->points != NULL && con->points->size >= 2) {
                for (int k = 0; k < con->points->size - 1; k++) {
                    SDL_Point *con_p1 = array_get(con->points, k); 
                    SDL_Point *con_p2 = array_get(con->points, k + 1); 

                    SDL_Point out;
                    
                    if (segment_intersection(knive_p1, knive_p2, con_p1, con_p2, &out)) {
                        array_remove_at(state->connections, j);
                        free_connection(con);
                        break; 
                    }
                }
            }
        }
    }
}

void toggle_play_pause(SimulationState *state, void *function_data) {
    assert(state != NULL);
    (void)function_data;

    state->is_paused = !state->is_paused;
}

void one_step(SimulationState *state, void *function_data) {
    assert(state != NULL);
    (void)function_data;

    for (int i = 0; i < state->nodes->size; i++) {
        Node *node = array_get(state->nodes, i);
        run_node(node);
    }

    for (int i = 0; i < state->connections->size; i++) {
        Connection *con = array_get(state->connections, i);
        set_input_zero(con);
    }

    for (int i = 0; i < state->connections->size; i++) {
        Connection *con = array_get(state->connections, i);
        propagate_state(con);
    }
}

int try_handle_node_click(SimulationState *state, float world_x, float world_y) {
    Node *node = find_node_at_position(state, world_x, world_y);
    if (node == NULL) return 0;

    state->drag_offset_x = world_x - node->rect.x;
    state->drag_offset_y = world_y - node->rect.y;
    state->is_node_dragging = 1;
    state->dragged_node = node;

    return 1;
}

int try_handle_connection_point_click(SimulationState *state) {
    if (state->hovered_connection_point != NULL) {
        state->is_connection_point_dragging = 1;
        state->dragging_connection_point = state->hovered_connection_point;
        return 1;
    }
    return 0;
}

int try_handle_button_click(SimulationState *state) {
    Button *clicked_button = find_button_at_position(state->buttons, state->mouse_x, state->mouse_y);
    if (clicked_button) {
        clicked_button->on_press(state, clicked_button);
        return 1;
    }
    return 0;
}

int try_handle_pin_click(SimulationState *state) {
    if (state->hovered_pin != NULL) {
        state->first_selected_pin = state->hovered_pin;
        state->new_connection = start_connection(state->hovered_pin);
        add_connection_point(state->new_connection, state->hovered_pin->parent_node->rect.x + state->hovered_pin->x, state->hovered_pin->parent_node->rect.y + state->hovered_pin->y);
        state->is_cable_dragging = 1;
        return 1;
    }
    return 0;
}

void start_selection_box(SimulationState *state) {
    state->selected_nodes->size = 0;
    state->selected_connection_points->size = 0;

    state->is_selection_box_drawing = 1;
    state->selection_box.x = state->mouse_x;
    state->selection_box.y = state->mouse_y;
}

void update_all_connections(SimulationState *state) {
    for (int i = 0; i < state->connections->size; i++)
    {
        Connection *con = array_get(state->connections, i);
        update_connection_points(state, con);
        correct_connection_points(con);
    }
}

void handle_selection_box(SimulationState *state) {
    if (state->is_selection_box_drawing)
    {
        state->selection_box.w = state->mouse_x - state->selection_box.x;
        state->selection_box.h = state->mouse_y - state->selection_box.y;
    }
}

void handle_cable_dragging(SimulationState *state) {
    if (!state->is_cable_dragging) return;

    SDL_Point *last = (SDL_Point *)array_get(state->new_connection->points, state->new_connection->points->size - 1);

    if ((state->new_connection->points->size % 2) == 0) {
        if (state->mouse_x > last->x + NODE_HEIGHT ||
            state->mouse_x < last->x - NODE_HEIGHT) {
            add_connection_point(state->new_connection, last->x, state->mouse_y);
        }
    }
    else {
        if (state->mouse_y > last->y + NODE_HEIGHT ||
            state->mouse_y < last->y - NODE_HEIGHT) {
            add_connection_point(state->new_connection, state->mouse_x, last->y);
        }
    }
}

void handle_node_dragging(SimulationState *state, float world_x, float world_y) {
    if (state->is_node_dragging && state->dragged_node != NULL) {
        float new_x = world_x - state->drag_offset_x;
        float new_y = world_y - state->drag_offset_y;
        
        float offset_x = new_x - state->dragged_node->rect.x;
        float offset_y = new_y - state->dragged_node->rect.y;
        
        state->dragged_node->rect.x = new_x;
        state->dragged_node->rect.y = new_y;
        
        for (int i = 0; i < state->selected_nodes->size; i++) {
            Node *current = array_get(state->selected_nodes, i);
            if (current != state->dragged_node) { 
                current->rect.x += offset_x;
                current->rect.y += offset_y;
            }
        }

        for (int i = 0; i < state->selected_connection_points->size; i++) {
            SDL_Point *current = array_get(state->selected_connection_points, i);
            current->x += offset_x;
            current->y += offset_y;
        }
        update_all_connections(state);
    }
}

void handle_connection_point(SimulationState *state, float world_x, float world_y) {
    state->hovered_connection_point = NULL;
    for (int i = 0; i < state->connections->size; i++) {
        Connection *con = array_get(state->connections, i);
        for (int n = 0; n < con->points->size; n++) {
            SDL_Point *point = array_get(con->points, n);

            int dx = world_x - point->x;
            int dy = world_y - point->y;
            float distance_squared = (dx * dx) + (dy * dy);
            float hover_radius_squared = 25.0f;

            if (distance_squared <= hover_radius_squared) {
                state->hovered_connection_point = point;
            }

            if (state->is_connection_point_dragging && state->dragging_connection_point == point) {
                state->dragging_connection_point->x = world_x - state->drag_offset_x;
                state->dragging_connection_point->y = world_y - state->drag_offset_y;
            }
        }
    }
}

void update_pin_hover(SimulationState *state, float world_x, float world_y) {
    state->hovered_pin = NULL;
    for (int i = 0; i < state->nodes->size; i++)
    {
        Node *node = array_get(state->nodes, i);

        for (int n = 0; n < node->inputs->size; n++)
        {
            Pin *pin = (Pin *)array_get(node->inputs, n);

            if (world_x >= pin->x + node->rect.x && world_x <= pin->x + node->rect.x + PIN_SIZE &&
                world_y >= pin->y + node->rect.y && world_y <= pin->y + node->rect.y + PIN_SIZE)
            {
                state->hovered_pin = pin;
                return;
            }
        }

        for (int n = 0; n < node->outputs->size; n++)
        {
            Pin *pin = (Pin *)array_get(node->outputs, n);
            if (world_x >= pin->x + node->rect.x && world_x <= pin->x + node->rect.x + PIN_SIZE &&
                world_y >= pin->y + node->rect.y && world_y <= pin->y + node->rect.y + PIN_SIZE)
            {
                state->hovered_pin = pin;
                return;
            }
        }
    }
}

void handle_knife_stroke_motion(SimulationState *state, float world_x, float world_y) {
    if (state->right_mouse_down)
    {
        Uint32 now = SDL_GetTicks();
        if (now - state->last_knife_record_time > 20)
        {
            SDL_Point *point = malloc(sizeof(SDL_Point));
            if (!point)
            {
                printf("Out of memory!\n");
                exit(EXIT_FAILURE);
            }
            point->x = world_x;
            point->y = world_y;
            array_add(state->knife_stroke, point);
            state->last_knife_record_time = now;
        }
    }
}

void reset_knife_stroke(SimulationState *state) {
    array_free(state->knife_stroke);
    state->knife_stroke = array_create(16);
}

int point_in_rect(float x, float y, SDL_Rect rect) {
    return x >= rect.x && x <= rect.x + rect.w &&
           y >= rect.y && y <= rect.y + rect.h;
}

int point_in_node(float world_x, float world_y, Node *node) {
    return point_in_rect(world_x, world_y, node->rect);
}

Node *find_node_at_position(SimulationState *state, float x, float y) {
    for (int i = 0; i < state->nodes->size; i++) {
        Node *node = array_get(state->nodes, i);
        
        if (!point_in_node(x, y, node)) continue;
        
        return node;
    }
    return NULL;
}

Button *find_button_at_position(DynamicArray *buttons, int screen_x, int screen_y) {
    assert(buttons != NULL);

    for (int i = 0; i < buttons->size; i++) {
        Button *button = array_get(buttons, i);
        if (point_in_rect(screen_x, screen_y, button->rect)) {
            return button;
        }
    }
    return NULL;
}

void toggle_switch_outputs(Node *switch_node) {
    for (int i = 0; i < switch_node->outputs->size; i++) {
        Pin *pin = array_get(switch_node->outputs, i);
        pin->state = !pin->state;
    }
}

void reset_interaction_state(SimulationState *state) {
    state->is_connection_point_dragging = 0;
    state->is_selection_box_drawing = 0;
    state->selection_box = (SDL_Rect){0, 0, 0, 0};
}

int try_complete_pin_connection(SimulationState *state) {
    if (state->hovered_pin == NULL) return 0;
    if (state->first_selected_pin == NULL) return 0;
    if (state->first_selected_pin == state->hovered_pin) return 0;
    if (state->first_selected_pin->is_input == state->hovered_pin->is_input) return 0;

    add_connection_point(state->new_connection, state->hovered_pin->parent_node->rect.x + state->hovered_pin->x, state->hovered_pin->parent_node->rect.y + state->hovered_pin->y);
    finish_conection(state->new_connection, state->hovered_pin);
    array_add(state->connections, state->new_connection);

    state->first_selected_pin = NULL;
    state->is_cable_dragging = 0;
    return 1;
}

void delete_node_and_connections(SimulationState *state, Node *node) {
    for (int i = state->connections->size - 1; i >= 0; i--) {
        Connection *con = array_get(state->connections, i);
        if (con->p1->parent_node == node || con->p2->parent_node == node) {
            array_remove_at(state->connections, i);
            free_connection(con);
        }
    }
    
    for (int i = 0; i < state->nodes->size; i++) {
        Node *current = array_get(state->nodes, i);
        if (current == node) {
            array_remove_at(state->nodes, i);
            free_node(current);
            break;
        }
    }
}

void handle_selection(SimulationState *state) {
    assert(state != NULL);
    assert(state->nodes != NULL);

    SDL_Rect selection_box_world;
    screen_rect_to_world(state, &state->selection_box, &selection_box_world);

    for (int i = 0; i < state->nodes->size; i++) {
        Node *current = array_get(state->nodes, i);
        if (SDL_HasIntersection(&selection_box_world, &current->rect)) {
            array_add(state->selected_nodes, current);
        }
    }

    for (int i = 0; i < state->connections->size; i++) {
        Connection *con = array_get(state->connections, i);
        for (int j = 0; j < con->points->size; j++) {
            SDL_Point *current = array_get(con->points, j);
            if (point_in_rect(current->x, current->y, selection_box_world)) {
                array_add(state->selected_connection_points, current);
            }
        }
    }
}

int point_exists_in_array(DynamicArray *points_array, SDL_Point *target) {
    assert(points_array != NULL);
    assert(target != NULL);

    for (int i = 0; i < points_array->size; i++) {
        SDL_Point *point = (SDL_Point *)array_get(points_array, i); 
        if (point == target) {
            return 1;
        }
    }
    return 0;
}

void process_left_click(SimulationState *state) {
    assert(state != NULL);
    assert(state->nodes != NULL);
    assert(state->buttons != NULL);

    if (!state->left_mouse_down) return;

    if (state->popup_state->is_pop)
    {
       if(try_handle_popup(state))return;
    }
    

    float world_x, world_y;
    screen_to_world(state, state->mouse_x, state->mouse_y, &world_x, &world_y);

    if (try_handle_pin_click(state)) return;
    if (try_handle_node_click(state, world_x, world_y)) return;
    if (try_handle_connection_point_click(state)) return;
    if (try_handle_button_click(state)) return;
    
    start_selection_box(state);
}

void process_right_click(SimulationState *state) {
    assert(state != NULL);
    assert(state->nodes != NULL);

    if (!state->right_mouse_down) return;

    float world_x, world_y;
    screen_to_world(state, state->mouse_x, state->mouse_y, &world_x, &world_y);

    Node *clicked_node = find_node_at_position(state, world_x, world_y);
    if (clicked_node && strncmp(clicked_node->name, "SWITCH", 6) == 0) {
        toggle_switch_outputs(clicked_node);
    }
}

void process_mouse_motion(SimulationState *state) {
    assert(state != NULL);
    assert(state->nodes != NULL);
    assert(state->buttons != NULL);

    float world_x, world_y;
    screen_to_world(state, state->mouse_x, state->mouse_y, &world_x, &world_y);

    handle_knife_stroke_motion(state, world_x, world_y);
    update_pin_hover(state, world_x, world_y);
    handle_connection_point(state, world_x, world_y);
    handle_node_dragging(state, world_x, world_y);
    handle_cable_dragging(state);
    handle_selection_box(state);
    update_all_connections(state);
}

void process_right_mouse_up(SimulationState *state) {
    assert(state != NULL);
    cut_connection(state);
    reset_knife_stroke(state);
}

void process_left_mouse_up(SimulationState *state) {
    assert(state != NULL);
    assert(state->buttons != NULL);

    handle_selection(state);
    if(!try_complete_pin_connection(state)){
        state->first_selected_pin = NULL;
        state->is_cable_dragging = 0;
    }

    reset_interaction_state(state);
}

void handle_copy(SimulationState *state) {
    assert(state != NULL);

    state->clipboard_nodes = flat_copy(state->selected_nodes);
    state->clipboard_connection_points = flat_copy(state->selected_connection_points);
}

void paste_offset_rect(SimulationState *state, SDL_Rect *rect) {
    assert(state != NULL);
    assert(rect != NULL);

    rect->x += 50;
    rect->y += 50;
}

void paste_offset_point(SimulationState *state, SDL_Point *point) {
    assert(state != NULL);
    assert(point != NULL);

    point->x += 50;
    point->y += 50;
}

DynamicArray* find_fully_selected_connections(DynamicArray *connections, DynamicArray *selected_connection_points) {
    assert(connections != NULL);
    assert(selected_connection_points != NULL);

    DynamicArray *fully_selected = array_create(16);
    
    for (int i = 0; i < connections->size; i++) {
        Connection *conn = (Connection *)array_get(connections, i);
        int all_points_selected = 1;
        
        for (int j = 0; j < conn->points->size; j++) {
            SDL_Point *point = (SDL_Point *)array_get(conn->points, j);
            
            if (!point_exists_in_array(selected_connection_points, point)) {
                all_points_selected = 0;
                break;
            }
        }
        
        if (all_points_selected) {
            array_add(fully_selected, conn);
        }
    }
    
    return fully_selected;
}

Pin* find_corresponding_pin(Pin *original_pin, DynamicArray *original_nodes, DynamicArray *pasted_nodes) {
    for (int i = 0; i < original_nodes->size; i++) {
        Node *original_node = array_get(original_nodes, i);
        
        for (int j = 0; j < original_node->inputs->size; j++) {
            Pin *pin = array_get(original_node->inputs, j);
            if (pin == original_pin) {
                Node *pasted_node = array_get(pasted_nodes, i);
                return array_get(pasted_node->inputs, j);
            }
        }
        
        for (int j = 0; j < original_node->outputs->size; j++) {
            Pin *pin = array_get(original_node->outputs, j);
            if (pin == original_pin) {
                Node *pasted_node = array_get(pasted_nodes, i);
                return array_get(pasted_node->outputs, j);
            }
        }
    }
    return NULL; 
}

void handle_paste(SimulationState *state) {
    assert(state != NULL);

    state->selected_nodes->size = 0;
    state->selected_connection_points->size = 0;

    for (int i = 0; i < state->clipboard_nodes->size; i++) {
        Node *current = array_get(state->clipboard_nodes, i);
        Node *new = create_node(current->inputs->size, current->outputs->size, current->operation, current->rect, current->name);
        paste_offset_rect(state, &new->rect);

        array_add(state->nodes, new);
        array_add(state->selected_nodes, new);
    }

    DynamicArray *matching_connections = find_fully_selected_connections(state->connections, state->clipboard_connection_points);
    for (int i = 0; i < matching_connections->size; i++) {
        Connection *original_conn = array_get(matching_connections, i);

        Pin *new_p1 = find_corresponding_pin(original_conn->p1, state->clipboard_nodes, state->selected_nodes);
        Pin *new_p2 = find_corresponding_pin(original_conn->p2, state->clipboard_nodes, state->selected_nodes);
        if (new_p1 == NULL || new_p2 == NULL)  continue;
        
        Connection *new_conn = malloc(sizeof(Connection));
        if (new_conn == NULL) continue;
        
        new_conn->p1 = new_p1;
        new_conn->p2 = new_p2;
        new_conn->state = original_conn->state;
        new_conn->points = array_create(4);
        if (new_conn->points == NULL) {
            free(new_conn);
            continue;
        }

        for (int j = 0; j < original_conn->points->size; j++) {
            SDL_Point *original_point = array_get(original_conn->points, j);
            
            SDL_Point *new_point = malloc(sizeof(SDL_Point));
            if (new_point == NULL) continue;
            
            new_point->x = original_point->x;
            new_point->y = original_point->y;
            paste_offset_point(state, new_point);
            
            array_add(new_conn->points, new_point);
            array_add(state->selected_connection_points, new_point);
        }

        propagate_state(new_conn);
        correct_connection_points(new_conn);
        update_connection_points(state, new_conn);

        array_add(state->connections, new_conn);
    }  
    free(matching_connections);
} 

void handle_backspace(SimulationState *state) {
    if(state->popup_state->is_pop) {
        size_t len = strlen(state->popup_state->name_input.text);
        if (len > 0) {
            state->popup_state->name_input.text[len - 1] = '\0';
        }
        return;
    }

    for (int i = 0; i < state->selected_nodes->size; i++) {
        Node *node = array_get(state->selected_nodes, i);
        delete_node_and_connections(state, node);
        state->last_dragged_node = NULL;
    }
    state->selected_nodes->size = 0;
    state->selected_connection_points->size = 0;
}

void handle_group_nodes(SimulationState *state) {
    printf("group nodes\n");

    // make a input field, where i can type the name in for this node / color? 
    
    // delete selcted nodes from array with connections. 
    // craete component 
    // count number of switches / lights  --> make so many in / out puts
    // make a node with the num inputs outputs
    // paste deleted nodes into tht copmonent 
    // add internal step function for components that runs them

    // right click coponnet to step into it.
}

void handle_g_pressed(SimulationState *state) {
    assert(state != NULL);
    printf("g presed\n");
    state->popup_state->is_pop = 1;
}

void handle_escape(SimulationState *state, void *function_data) {
    assert(state != NULL);
    (void)function_data;

    if (state->popup_state->is_pop == 1) state->popup_state->is_pop = 0;
}

void handle_enter(SimulationState *state, void *function_data) {
    assert(state != NULL);
    (void)function_data;

    if (state->popup_state->is_pop == 1) {
        handle_group_nodes(state);
        state->popup_state->is_pop = 0;
    }
}
