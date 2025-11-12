/*
 * blog.h - Core data structures and constants for SSH blog viewer
 */

#ifndef BLOG_H
#define BLOG_H

#include <stddef.h>

/* Constants */
#define MAX_TITLE_LEN 256
#define MAX_DATE_LEN 11        /* YYYY-MM-DD format */
#define MAX_FILENAME_LEN 256
#define MAX_CONTENT_LEN 65536  /* 64KB per blog entry */
#define MAX_ENTRIES 1000
#define MAX_PATH_LEN 1024

/* Blog entry structure */
typedef struct {
    int id;
    char title[MAX_TITLE_LEN];
    char date[MAX_DATE_LEN];
    char filename[MAX_FILENAME_LEN];
    char *content;
    size_t content_length;
} BlogEntry;

/* Blog database structure */
typedef struct {
    BlogEntry *entries;
    int count;
    int current_index;
    int capacity;
} BlogDatabase;

/* Application state */
typedef enum {
    MODE_NORMAL,     /* Normal reading mode */
    MODE_COMMAND,    /* Command mode (: prefix) */
    MODE_LIST,       /* Blog list view (:b) */
    MODE_SEARCH      /* Search mode (:/) */
} ViewMode;

/* Search results tracking */
typedef struct {
    int entry_indices[MAX_ENTRIES];  /* Indices of matching entries */
    int count;                       /* Number of matches */
    int current;                     /* Current match being viewed */
} SearchResults;

typedef struct {
    BlogDatabase *db;
    ViewMode mode;
    int scroll_offset;
    char search_query[256];
    SearchResults search_results;
    int quit_requested;
} AppState;

#endif /* BLOG_H */
