/*
 * navigation.h - Navigation and input handling functions
 */

#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "blog.h"

/*
 * Navigate to the next blog entry (K key)
 */
void nav_next_entry(AppState *state);

/*
 * Navigate to the previous blog entry (L key)
 */
void nav_prev_entry(AppState *state);

/*
 * Navigate to a specific entry by index
 */
void nav_goto_entry(AppState *state, int index);

/*
 * Scroll down in current entry
 */
void nav_scroll_down(AppState *state);

/*
 * Scroll up in current entry
 */
void nav_scroll_up(AppState *state);

/*
 * Handle command mode input
 * Returns 0 to continue, 1 to quit
 */
int nav_handle_command(AppState *state, const char *command);

/*
 * Enter command mode and get command from user
 */
void nav_enter_command_mode(AppState *state);

/*
 * Handle blog list selection
 */
void nav_handle_list_mode(AppState *state, int key);

#endif /* NAVIGATION_H */
