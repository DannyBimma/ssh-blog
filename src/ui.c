/*
 * ui.c - Terminal UI rendering functions using ncurses
 */

#include "ui.h"
#include <ncurses.h>
#include <string.h>
#include <stdio.h>

/* Terminal dimensions */
static int term_height = 0;
static int term_width = 0;

int ui_init(void) {
    /* Initialize ncurses */
    if (initscr() == NULL) {
        return -1;
    }

    /* Configure terminal */
    cbreak();              /* Disable line buffering */
    noecho();              /* Don't echo input */
    keypad(stdscr, TRUE);  /* Enable special keys */
    curs_set(0);           /* Hide cursor */

    /* Get terminal dimensions */
    getmaxyx(stdscr, term_height, term_width);

    /* Enable colors if supported */
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);    /* Status line */
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);  /* Titles */
        init_pair(3, COLOR_RED, COLOR_BLACK);     /* Errors */
    }

    return 0;
}

void ui_cleanup(void) {
    endwin();
}

void ui_draw_entry(const BlogEntry *entry, int scroll_offset) {
    if (!entry) {
        return;
    }

    clear();

    /* Update terminal dimensions in case of resize */
    getmaxyx(stdscr, term_height, term_width);

    /* Calculate content area (leave room for status line) */
    int content_height = term_height - 2;
    int line_num = 0;

    /* Draw title */
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(line_num++, 0, "%s", entry->title);
    attroff(COLOR_PAIR(2) | A_BOLD);

    /* Draw date */
    mvprintw(line_num++, 0, "Date: %s", entry->date);
    line_num++;  /* Blank line */

    /* Draw content with word wrapping */
    if (entry->content) {
        const char *content = entry->content;
        size_t content_len = entry->content_length;
        int current_line = 0;
        int display_line = line_num;

        /* Simple line-by-line display with scroll support */
        const char *line_start = content;
        const char *line_end;

        while (line_start < content + content_len && display_line < term_height - 1) {
            /* Find end of line */
            line_end = strchr(line_start, '\n');
            if (!line_end) {
                line_end = content + content_len;
            }

            /* Check if we should display this line (scrolling) */
            if (current_line >= scroll_offset) {
                int line_len = line_end - line_start;

                /* Word wrap long lines */
                if (line_len > term_width) {
                    int printed = 0;
                    while (printed < line_len && display_line < term_height - 1) {
                        int to_print = (line_len - printed > term_width) ?
                                       term_width : line_len - printed;
                        mvprintw(display_line++, 0, "%.*s", to_print,
                                line_start + printed);
                        printed += to_print;
                    }
                } else {
                    mvprintw(display_line++, 0, "%.*s", line_len, line_start);
                }
            }

            current_line++;
            line_start = line_end + 1;
        }
    }

    refresh();
}

void ui_draw_status(const AppState *state) {
    if (!state || !state->db) {
        return;
    }

    /* Draw status line at bottom */
    int status_y = term_height - 1;

    attron(COLOR_PAIR(1) | A_REVERSE);
    mvhline(status_y, 0, ' ', term_width);

    /* Show current entry number and total */
    char status_text[256];
    snprintf(status_text, sizeof(status_text),
             " Entry %d/%d | %s | K:next L:prev :q:quit :b:list :/search ",
             state->db->current_index + 1,
             state->db->count,
             state->db->count > 0 ?
                state->db->entries[state->db->current_index].date : "");

    mvprintw(status_y, 0, "%.*s", term_width, status_text);
    attroff(COLOR_PAIR(1) | A_REVERSE);

    refresh();
}

void ui_draw_blog_list(const BlogDatabase *db, int selected_index) {
    if (!db) {
        return;
    }

    clear();

    /* Draw header */
    attron(A_BOLD);
    mvprintw(0, 0, "Blog Entries");
    mvprintw(1, 0, "------------");
    attroff(A_BOLD);

    /* Draw list of entries */
    int start_line = 3;
    int max_display = term_height - 5;  /* Leave room for header and footer */

    for (int i = 0; i < db->count && i < max_display; i++) {
        int line = start_line + i;

        if (i == selected_index) {
            attron(A_REVERSE);
        }

        mvprintw(line, 0, "%2d. [%s] %s",
                i + 1,
                db->entries[i].date,
                db->entries[i].title);

        if (i == selected_index) {
            attroff(A_REVERSE);
        }
    }

    /* Draw footer instructions */
    attron(COLOR_PAIR(1));
    mvprintw(term_height - 2, 0, "Use K/L or arrow keys to navigate, Enter to select, :q to cancel");
    attroff(COLOR_PAIR(1));

    refresh();
}

void ui_draw_command_prompt(const char *prompt) {
    int prompt_y = term_height - 1;

    /* Clear the line */
    move(prompt_y, 0);
    clrtoeol();

    /* Draw prompt */
    mvprintw(prompt_y, 0, "%s", prompt);
    refresh();
}

int ui_get_command_input(char *buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return -1;
    }

    int prompt_y = term_height - 1;
    move(prompt_y, 1);  /* Position after the ':' */

    echo();
    curs_set(1);  /* Show cursor */

    int result = getnstr(buffer, buffer_size - 1);

    noecho();
    curs_set(0);  /* Hide cursor */

    return (result == OK) ? strlen(buffer) : -1;
}

void ui_show_error(const char *message) {
    if (!message) {
        return;
    }

    int msg_y = term_height - 1;

    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(msg_y, 0, "Error: %s", message);
    clrtoeol();
    attroff(COLOR_PAIR(3) | A_BOLD);

    refresh();
    napms(2000);  /* Display for 2 seconds */
}

void ui_clear(void) {
    clear();
    refresh();
}
