/*
 * blog_loader.h - Functions for loading blog entries from disk
 */

#ifndef BLOG_LOADER_H
#define BLOG_LOADER_H

#include "blog.h"

/*
 * Initialize a new blog database
 * Returns pointer to database or NULL on failure
 */
BlogDatabase *blog_db_init(void);

/*
 * Free blog database and all associated memory
 */
void blog_db_free(BlogDatabase *db);

/*
 * Load all blog entries from the specified directory
 * Returns number of entries loaded, or -1 on error
 */
int blog_db_load_entries(BlogDatabase *db, const char *directory);

/*
 * Get the latest (most recent) blog entry
 * Returns pointer to entry or NULL if database is empty
 */
BlogEntry *blog_db_get_latest(BlogDatabase *db);

/*
 * Get blog entry by index
 * Returns pointer to entry or NULL if index is out of bounds
 */
BlogEntry *blog_db_get_entry(BlogDatabase *db, int index);

/*
 * Sort entries by date (newest first)
 */
void blog_db_sort_by_date(BlogDatabase *db);

#endif /* BLOG_LOADER_H */
