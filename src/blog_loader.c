/*
 * blog_loader.c - Functions for loading blog entries from disk
 */

#include "blog_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

/* Helper function to check if a file is a blog entry */
static int is_blog_file(const char *filename) {
    size_t len = strlen(filename);
    return (len > 4 && strcmp(filename + len - 4, ".txt") == 0);
}

/* Helper function to parse a blog entry file */
static int parse_blog_entry(BlogEntry *entry, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return -1;
    }

    char line[MAX_TITLE_LEN];
    int in_header = 1;
    size_t content_size = 0;
    char *content_buffer = malloc(MAX_CONTENT_LEN);

    if (!content_buffer) {
        fclose(fp);
        return -1;
    }

    content_buffer[0] = '\0';

    /* Parse header and content */
    while (fgets(line, sizeof(line), fp)) {
        if (in_header) {
            /* Check for separator */
            if (strncmp(line, "---", 3) == 0) {
                in_header = 0;
                continue;
            }

            /* Parse TITLE: */
            if (strncmp(line, "TITLE:", 6) == 0) {
                char *title = line + 6;
                /* Skip leading whitespace */
                while (*title == ' ' || *title == '\t') title++;
                /* Remove newline */
                size_t title_len = strlen(title);
                if (title_len > 0 && title[title_len - 1] == '\n') {
                    title[title_len - 1] = '\0';
                }
                strncpy(entry->title, title, MAX_TITLE_LEN - 1);
                entry->title[MAX_TITLE_LEN - 1] = '\0';
            }
            /* Parse DATE: */
            else if (strncmp(line, "DATE:", 5) == 0) {
                char *date = line + 5;
                while (*date == ' ' || *date == '\t') date++;
                size_t date_len = strlen(date);
                if (date_len > 0 && date[date_len - 1] == '\n') {
                    date[date_len - 1] = '\0';
                }
                strncpy(entry->date, date, MAX_DATE_LEN - 1);
                entry->date[MAX_DATE_LEN - 1] = '\0';
            }
        } else {
            /* Append to content */
            size_t line_len = strlen(line);
            if (content_size + line_len < MAX_CONTENT_LEN - 1) {
                strcat(content_buffer, line);
                content_size += line_len;
            }
        }
    }

    fclose(fp);

    /* Store content */
    entry->content = content_buffer;
    entry->content_length = content_size;

    /* If title is empty, use filename */
    if (entry->title[0] == '\0') {
        const char *basename = strrchr(filepath, '/');
        basename = basename ? basename + 1 : filepath;
        strncpy(entry->title, basename, MAX_TITLE_LEN - 1);
        entry->title[MAX_TITLE_LEN - 1] = '\0';
    }

    /* If date is empty, use file modification time */
    if (entry->date[0] == '\0') {
        struct stat st;
        if (stat(filepath, &st) == 0) {
            struct tm *tm = localtime(&st.st_mtime);
            strftime(entry->date, MAX_DATE_LEN, "%Y-%m-%d", tm);
        }
    }

    return 0;
}

/* Comparison function for sorting entries by date (newest first) */
static int compare_dates(const void *a, const void *b) {
    const BlogEntry *entry_a = (const BlogEntry *)a;
    const BlogEntry *entry_b = (const BlogEntry *)b;
    return strcmp(entry_b->date, entry_a->date);  /* Reverse order */
}

BlogDatabase *blog_db_init(void) {
    BlogDatabase *db = malloc(sizeof(BlogDatabase));
    if (!db) {
        return NULL;
    }

    db->capacity = MAX_ENTRIES;
    db->entries = malloc(sizeof(BlogEntry) * db->capacity);
    if (!db->entries) {
        free(db);
        return NULL;
    }

    db->count = 0;
    db->current_index = 0;

    return db;
}

void blog_db_free(BlogDatabase *db) {
    if (!db) {
        return;
    }

    /* Free all entry content */
    for (int i = 0; i < db->count; i++) {
        if (db->entries[i].content) {
            free(db->entries[i].content);
        }
    }

    free(db->entries);
    free(db);
}

int blog_db_load_entries(BlogDatabase *db, const char *directory) {
    if (!db || !directory) {
        return -1;
    }

    DIR *dir = opendir(directory);
    if (!dir) {
        return -1;
    }

    struct dirent *entry;
    int loaded_count = 0;

    while ((entry = readdir(dir)) != NULL && db->count < db->capacity) {
        if (!is_blog_file(entry->d_name)) {
            continue;
        }

        /* Build full file path */
        char filepath[MAX_PATH_LEN];
        snprintf(filepath, MAX_PATH_LEN, "%s/%s", directory, entry->d_name);

        /* Parse the blog entry */
        BlogEntry *blog_entry = &db->entries[db->count];
        blog_entry->id = db->count;
        strncpy(blog_entry->filename, entry->d_name, MAX_FILENAME_LEN - 1);
        blog_entry->filename[MAX_FILENAME_LEN - 1] = '\0';

        if (parse_blog_entry(blog_entry, filepath) == 0) {
            db->count++;
            loaded_count++;
        }
    }

    closedir(dir);

    /* Sort entries by date (newest first) */
    if (db->count > 0) {
        blog_db_sort_by_date(db);
    }

    return loaded_count;
}

BlogEntry *blog_db_get_latest(BlogDatabase *db) {
    if (!db || db->count == 0) {
        return NULL;
    }
    return &db->entries[0];
}

BlogEntry *blog_db_get_entry(BlogDatabase *db, int index) {
    if (!db || index < 0 || index >= db->count) {
        return NULL;
    }
    return &db->entries[index];
}

void blog_db_sort_by_date(BlogDatabase *db) {
    if (!db || db->count <= 1) {
        return;
    }
    qsort(db->entries, db->count, sizeof(BlogEntry), compare_dates);
}
