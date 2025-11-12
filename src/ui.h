/*
 * ui.h - Terminal UI rendering functions using ncurses
 */

#ifndef UI_H
#define UI_H

#include "blog.h"

/*
 * Initialize ncurses and terminal settings
 * Returns 0 on success, -1 on failure
 */
int ui_init(void);

/*
 * Cleanup and restore terminal settings
 */
void ui_cleanup(void);

/*
 * Draw a blog entry to the screen
 */
void ui_draw_entry(const BlogEntry *entry, int scroll_offset);

/*
 * Draw the status line at bottom of screen
 */
void ui_draw_status(const AppState *state);

/*
 * Draw the blog list view (:b command)
 */
void ui_draw_blog_list(const BlogDatabase *db, int selected_index);

/*
 * Draw command prompt at bottom
 */
void ui_draw_command_prompt(const char *prompt);

/*
 * Get user input for command mode
 * Returns number of characters read
 */
int ui_get_command_input(char *buffer, size_t buffer_size);

/*
 * Show an error message
 */
void ui_show_error(const char *message);

/*
 * Clear the screen and refresh
 */
void ui_clear(void);

#endif /* UI_H */
