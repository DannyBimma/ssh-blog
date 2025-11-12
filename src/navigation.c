/*
 * navigation.c - Navigation and input handling functions
 */

#include "navigation.h"
#include "ui.h"
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

void nav_next_entry(AppState *state) {
    if (!state || !state->db) {
        return;
    }

    if (state->db->count == 0) {
        return;
    }

    /* Move to next entry (wrap around) */
    state->db->current_index++;
    if (state->db->current_index >= state->db->count) {
        state->db->current_index = 0;
    }

    /* Reset scroll offset */
    state->scroll_offset = 0;
}

void nav_prev_entry(AppState *state) {
    if (!state || !state->db) {
        return;
    }

    if (state->db->count == 0) {
        return;
    }

    /* Move to previous entry (wrap around) */
    state->db->current_index--;
    if (state->db->current_index < 0) {
        state->db->current_index = state->db->count - 1;
    }

    /* Reset scroll offset */
    state->scroll_offset = 0;
}

void nav_goto_entry(AppState *state, int index) {
    if (!state || !state->db) {
        return;
    }

    if (index >= 0 && index < state->db->count) {
        state->db->current_index = index;
        state->scroll_offset = 0;
    }
}

void nav_scroll_down(AppState *state) {
    if (!state) {
        return;
    }

    state->scroll_offset++;
    /* TODO: Add bounds checking based on content length */
}

void nav_scroll_up(AppState *state) {
    if (!state) {
        return;
    }

    if (state->scroll_offset > 0) {
        state->scroll_offset--;
    }
}

void nav_page_down(AppState *state) {
    if (!state) {
        return;
    }

    /* Scroll by a full page (roughly 20 lines) */
    state->scroll_offset += 20;
    /* TODO: Add max bounds checking */
}

void nav_page_up(AppState *state) {
    if (!state) {
        return;
    }

    /* Scroll up by a full page */
    state->scroll_offset -= 20;
    if (state->scroll_offset < 0) {
        state->scroll_offset = 0;
    }
}

void nav_goto_top(AppState *state) {
    if (!state) {
        return;
    }

    state->scroll_offset = 0;
}

void nav_goto_bottom(AppState *state) {
    if (!state || !state->db) {
        return;
    }

    /* Set scroll offset to a very large number */
    /* The UI rendering will handle not going past the end */
    state->scroll_offset = 10000;
}

int nav_handle_command(AppState *state, const char *command) {
    if (!state || !command) {
        return 0;
    }

    /* Remove leading/trailing whitespace */
    while (*command == ' ' || *command == '\t') {
        command++;
    }

    if (strlen(command) == 0) {
        return 0;
    }

    /* Handle quit command */
    if (strcmp(command, "q") == 0 || strcmp(command, "quit") == 0) {
        return 1;  /* Signal to quit */
    }

    /* Handle blog list command */
    if (strcmp(command, "b") == 0 || strcmp(command, "list") == 0) {
        state->mode = MODE_LIST;
        return 0;
    }

    /* Handle search command */
    if (command[0] == '/') {
        const char *query = command + 1;
        /* Skip leading whitespace in query */
        while (*query == ' ' || *query == '\t') {
            query++;
        }

        if (strlen(query) > 0) {
            strncpy(state->search_query, query, sizeof(state->search_query) - 1);
            state->search_query[sizeof(state->search_query) - 1] = '\0';

            /* Perform search and collect all matches */
            int matches = nav_search(state, query);

            if (matches > 0) {
                state->mode = MODE_SEARCH;
                /* Show first result */
                nav_goto_entry(state, state->search_results.entry_indices[0]);

                /* Show match count */
                char msg[256];
                snprintf(msg, sizeof(msg), "Found %d match%s (press 'n' for next, 'N' for prev)",
                        matches, matches == 1 ? "" : "es");
                ui_show_error(msg);  /* Reusing error display for info */
            } else {
                ui_show_error("No results found");
            }
        }
        return 0;
    }

    /* Handle numeric entry selection */
    if (command[0] >= '0' && command[0] <= '9') {
        int entry_num = atoi(command);
        if (entry_num > 0 && entry_num <= state->db->count) {
            nav_goto_entry(state, entry_num - 1);
            return 0;
        } else {
            ui_show_error("Invalid entry number");
            return 0;
        }
    }

    /* Unknown command */
    ui_show_error("Unknown command");
    return 0;
}

void nav_enter_command_mode(AppState *state) {
    if (!state) {
        return;
    }

    char command[256];
    ui_draw_command_prompt(":");

    if (ui_get_command_input(command, sizeof(command)) > 0) {
        if (nav_handle_command(state, command)) {
            state->quit_requested = 1;
        }
    }

    /* Return to normal mode */
    state->mode = MODE_NORMAL;
}

void nav_handle_list_mode(AppState *state, int key) {
    if (!state || !state->db) {
        return;
    }

    static int selected = 0;

    switch (key) {
        case 'K':
        case KEY_DOWN:
            if (selected < state->db->count - 1) {
                selected++;
            }
            break;

        case 'L':
        case KEY_UP:
            if (selected > 0) {
                selected--;
            }
            break;

        case '\n':
        case '\r':
        case KEY_ENTER:
            /* Select the entry */
            nav_goto_entry(state, selected);
            state->mode = MODE_NORMAL;
            selected = 0;
            return;

        case ':':
            /* Enter command mode from list */
            nav_enter_command_mode(state);
            if (state->quit_requested) {
                return;
            }
            break;

        case 'q':
            /* Quick quit from list view */
            state->mode = MODE_NORMAL;
            selected = 0;
            return;
    }

    /* Redraw list with new selection */
    ui_draw_blog_list(state->db, selected);
}

int nav_search(AppState *state, const char *query) {
    if (!state || !state->db || !query) {
        return 0;
    }

    /* Reset search results */
    state->search_results.count = 0;
    state->search_results.current = 0;

    /* Search through all entries */
    for (int i = 0; i < state->db->count; i++) {
        BlogEntry *entry = &state->db->entries[i];

        /* Check if query matches title or content */
        if (strstr(entry->title, query) != NULL ||
            (entry->content && strstr(entry->content, query) != NULL)) {
            /* Add to search results */
            if (state->search_results.count < MAX_ENTRIES) {
                state->search_results.entry_indices[state->search_results.count] = i;
                state->search_results.count++;
            }
        }
    }

    return state->search_results.count;
}

void nav_next_search_result(AppState *state) {
    if (!state || state->search_results.count == 0) {
        return;
    }

    /* Move to next result (wrap around) */
    state->search_results.current++;
    if (state->search_results.current >= state->search_results.count) {
        state->search_results.current = 0;
    }

    /* Navigate to the entry */
    int entry_idx = state->search_results.entry_indices[state->search_results.current];
    nav_goto_entry(state, entry_idx);

    /* Show progress */
    char msg[256];
    snprintf(msg, sizeof(msg), "Match %d of %d",
            state->search_results.current + 1,
            state->search_results.count);
    ui_show_error(msg);
}

void nav_prev_search_result(AppState *state) {
    if (!state || state->search_results.count == 0) {
        return;
    }

    /* Move to previous result (wrap around) */
    state->search_results.current--;
    if (state->search_results.current < 0) {
        state->search_results.current = state->search_results.count - 1;
    }

    /* Navigate to the entry */
    int entry_idx = state->search_results.entry_indices[state->search_results.current];
    nav_goto_entry(state, entry_idx);

    /* Show progress */
    char msg[256];
    snprintf(msg, sizeof(msg), "Match %d of %d",
            state->search_results.current + 1,
            state->search_results.count);
    ui_show_error(msg);
}
