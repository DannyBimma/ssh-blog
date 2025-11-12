/*
 * main.c - SSH Blog Viewer entry point
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "blog.h"
#include "blog_loader.h"
#include "ui.h"
#include "navigation.h"

#define DEFAULT_BLOG_DIR "./blog_entries"

/* Print usage information */
static void print_usage(const char *prog_name) {
    fprintf(stderr, "SSH Blog Viewer\n");
    fprintf(stderr, "Usage: %s [blog_directory]\n", prog_name);
    fprintf(stderr, "\n");
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "  blog_directory  Directory containing blog entries (default: %s)\n",
            DEFAULT_BLOG_DIR);
    fprintf(stderr, "\n");
    fprintf(stderr, "Controls:\n");
    fprintf(stderr, "  K         - Next blog entry\n");
    fprintf(stderr, "  L         - Previous blog entry\n");
    fprintf(stderr, "  j/k       - Scroll down/up within entry\n");
    fprintf(stderr, "  :q        - Quit\n");
    fprintf(stderr, "  :b        - Show blog list\n");
    fprintf(stderr, "  :/query   - Search for 'query'\n");
    fprintf(stderr, "\n");
}

/* Main event loop */
static void run_blog_viewer(AppState *state) {
    int ch;

    while (!state->quit_requested) {
        /* Draw current view based on mode */
        switch (state->mode) {
            case MODE_NORMAL: {
                BlogEntry *current = blog_db_get_entry(state->db,
                                                       state->db->current_index);
                if (current) {
                    ui_draw_entry(current, state->scroll_offset);
                    ui_draw_status(state);
                }
                break;
            }

            case MODE_LIST:
                ui_draw_blog_list(state->db, state->db->current_index);
                break;

            case MODE_COMMAND:
                /* Command mode is handled separately */
                break;

            case MODE_SEARCH:
                /* After search, return to normal mode */
                state->mode = MODE_NORMAL;
                break;
        }

        /* Get user input */
        ch = getch();

        /* Handle input based on current mode */
        if (state->mode == MODE_LIST) {
            nav_handle_list_mode(state, ch);
        } else {
            /* Normal mode input handling */
            switch (ch) {
                case 'K':
                    /* Next entry */
                    nav_next_entry(state);
                    break;

                case 'L':
                    /* Previous entry */
                    nav_prev_entry(state);
                    break;

                case 'j':
                case KEY_DOWN:
                    /* Scroll down */
                    nav_scroll_down(state);
                    break;

                case 'k':
                case KEY_UP:
                    /* Scroll up */
                    nav_scroll_up(state);
                    break;

                case 6:  /* Ctrl+F */
                case KEY_NPAGE:  /* Page Down */
                    /* Page down */
                    nav_page_down(state);
                    break;

                case 2:  /* Ctrl+B */
                case KEY_PPAGE:  /* Page Up */
                    /* Page up */
                    nav_page_up(state);
                    break;

                case 'g':  /* gg to go to top */
                case KEY_HOME:
                    /* Jump to top */
                    nav_goto_top(state);
                    break;

                case 'G':  /* G to go to bottom */
                case KEY_END:
                    /* Jump to bottom */
                    nav_goto_bottom(state);
                    break;

                case ':':
                    /* Enter command mode */
                    nav_enter_command_mode(state);
                    break;

                case 'q':
                    /* Quick quit (alternative to :q) */
                    state->quit_requested = 1;
                    break;

                case 'b':
                    /* Quick blog list (alternative to :b) */
                    state->mode = MODE_LIST;
                    break;

                case 'n':
                    /* Next search result */
                    if (state->search_results.count > 0) {
                        nav_next_search_result(state);
                    }
                    break;

                case 'N':
                    /* Previous search result */
                    if (state->search_results.count > 0) {
                        nav_prev_search_result(state);
                    }
                    break;

                case KEY_RESIZE:
                    /* Handle terminal resize */
                    ui_clear();
                    break;

                default:
                    /* Ignore unknown keys */
                    break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    const char *blog_dir = DEFAULT_BLOG_DIR;

    /* Parse command line arguments */
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        blog_dir = argv[1];
    }

    /* Initialize blog database */
    BlogDatabase *db = blog_db_init();
    if (!db) {
        fprintf(stderr, "Error: Failed to initialize blog database\n");
        return 1;
    }

    /* Load blog entries */
    int loaded = blog_db_load_entries(db, blog_dir);
    if (loaded < 0) {
        fprintf(stderr, "Error: Failed to load blog entries from '%s'\n", blog_dir);
        fprintf(stderr, "Make sure the directory exists and contains .txt files\n");
        blog_db_free(db);
        return 1;
    }

    if (loaded == 0) {
        fprintf(stderr, "Warning: No blog entries found in '%s'\n", blog_dir);
        fprintf(stderr, "Creating a default entry...\n");

        /* Create a default entry so the viewer doesn't crash */
        db->entries[0].id = 0;
        snprintf(db->entries[0].title, MAX_TITLE_LEN, "Welcome to SSH Blog");
        snprintf(db->entries[0].date, MAX_DATE_LEN, "2025-01-12");
        db->entries[0].content = strdup(
            "Welcome to SSH Blog!\n\n"
            "No blog entries were found in the blog directory.\n\n"
            "To add blog entries:\n"
            "1. Create .txt files in the blog_entries/ directory\n"
            "2. Format them with TITLE: and DATE: headers\n"
            "3. Add your content after a --- separator\n\n"
            "Example:\n"
            "  TITLE: My First Post\n"
            "  DATE: 2025-01-12\n"
            "  ---\n"
            "  This is my first blog post!\n"
        );
        db->entries[0].content_length = strlen(db->entries[0].content);
        db->count = 1;
        db->current_index = 0;
    }

    /* Initialize UI */
    if (ui_init() != 0) {
        fprintf(stderr, "Error: Failed to initialize terminal UI\n");
        blog_db_free(db);
        return 1;
    }

    /* Initialize application state */
    AppState state = {
        .db = db,
        .mode = MODE_NORMAL,
        .scroll_offset = 0,
        .quit_requested = 0
    };
    state.search_query[0] = '\0';
    state.search_results.count = 0;
    state.search_results.current = 0;

    /* Start at the latest (first) entry */
    db->current_index = 0;

    /* Run the main event loop */
    run_blog_viewer(&state);

    /* Cleanup */
    ui_cleanup();
    blog_db_free(db);

    return 0;
}
