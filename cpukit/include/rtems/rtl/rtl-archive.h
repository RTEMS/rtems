/*
 *  COPYRIGHT (c) 2018 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Archive
 *
 * The RTL Archive module manages dependent loading of object files from
 * archives. The archives need to have a `ranlib` generated symbol table.
 *
 * This module reads a configuration file called `rtl-libs.conf` from a default
 * directory of `/etc`. The file is a line per glob'ed path to archives to
 * search for symbols.
 *
 * The archive symbols are held in a per archive cache for searching.
 *
 * @note Errors in the reading of a config file, locating archives, reading
 *       symbol tables and loading object files are not considered RTL error
 *       reported to a user. The user error is undefined symbols.
 */

#if !defined (_RTEMS_RTL_ARCHIVE_H_)
#define _RTEMS_RTL_ARCHIVE_H_

#include <rtems.h>
#include <rtems/chain.h>
#include <rtems/printer.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Flags for archives.
 */
#define RTEMS_RTL_ARCHIVE_USER_LOAD (1 << 0) /**< User forced load. */
#define RTEMS_RTL_ARCHIVE_REMOVE    (1 << 1) /**< The achive is not found. */
#define RTEMS_RTL_ARCHIVE_LOAD      (1 << 2) /**< Load the achive. */

/**
 * Symbol search and loading results.
 */
typedef enum rtems_rtl_archive_search
{
  rtems_rtl_archive_search_not_found = 0, /**< The search failed to find the
                                               symbol. */
  rtems_rtl_archive_search_found = 1,     /**< The symbols was found. */
  rtems_rtl_archive_search_loaded = 2,    /**< The symbol was found and the
                                               object file has been loaded */
  rtems_rtl_archive_search_error = 3,     /**< There was an error searching or
                                               loading. */
  rtems_rtl_archive_search_no_config = 4 /**< There is no config or it is
                                          *   invalid. */
} rtems_rtl_archive_search;

/**
 * RTL Archive symbols.
 */
typedef struct rtems_rtl_archive_symbol
{
  size_t      entry;  /**< Index in the symbol offset table. */
  const char* label;  /**< The symbol's label. */
} rtems_rtl_archive_symbol;

/**
 * RTL Archive symbols.
 */
typedef struct rtems_rtl_archive_symbols
{
  void*                     base;     /**< Base of the symbol table. */
  size_t                    size;     /**< Size of the symbol table. */
  size_t                    entries;  /**< Entries in the symbol table. */
  const char*               names;    /**< Start of the symbol names. */
  rtems_rtl_archive_symbol* symbols;  /**< Sorted symbol table. */
} rtems_rtl_archive_symbols;

/**
 * RTL Archive data.
 */
typedef struct rtems_rtl_archive
{
  rtems_chain_node          node;     /**< Chain link. */
  const char*               name;     /**< Archive absolute path. */
  size_t                    size;     /**< Size of the archive. */
  time_t                    mtime;    /**< Archive's last modified time. */
  off_t                     enames;   /**< Extended file name offset, lazy load. */
  rtems_rtl_archive_symbols symbols;  /**< Ranlib symbol table. */
  size_t                    refs;     /**< Loaded object modules. */
  uint32_t                  flags;    /**< Some flags. */
} rtems_rtl_archive;

/**
 * RTL Archive data.
 */
typedef struct rtems_rtl_archives
{
  const char*         config_name;    /**< Config file name. */
  time_t              config_mtime;   /**< Config last modified time. */
  size_t              config_length;  /**< Length the config data. */
  char*               config;         /**< Config file contents. */
  rtems_chain_control archives;       /**< The located archives. */
} rtems_rtl_archives;

/**
 * Error handler call when finding an archive.
 */
typedef void (*rtems_rtl_archive_error)(int num, const char* text);

/**
 * Open the RTL archives support with the specified configration file.
 *
 * @param archives The archives data to open.
 * @param config The path to the configuration file.
 * @return bool Returns @true is the archives are open.
 */
void rtems_rtl_archives_open (rtems_rtl_archives* archives, const char* config);

/**
 * Close the RTL archives support.
 *
 * @param archives The archives data to close.
 */
void rtems_rtl_archives_close (rtems_rtl_archives* archives);

/**
 * Refresh the archives data. Check if the configuration has changes and if it
 * has reload it. Check each path in the configuration and creates archive
 * instances for new archives and remove archives not present any more.
 *
 * Refreshing is a development aid so reboots can be avoided as users trial
 * configurations that work.
 *
 * @param archives The archives data to refresh.
 * @retval false The refresh failed, an error will have been set.
 */
bool rtems_rtl_archives_refresh (rtems_rtl_archives* archives);

/**
 * Load an archive.
 *
 * @param archives The archives data to search.
 * @param name     The archive to load.
 * @retval true    The archive is loaded.
 */
bool rtems_rtl_archive_load (rtems_rtl_archives* archives, const char* name);

/**
 * Search for a symbol and load the first object file that has the symbol.
 *
 * @param archives The archives data to search.
 * @param symbol   The symbol name to search for.
 * @param load     If @true load the object file the symbol is found in
 *                 else return the found not found status.
 */
rtems_rtl_archive_search rtems_rtl_archive_obj_load (rtems_rtl_archives* archives,
                                                     const char*         symbol,
                                                     bool                load);

/**
 * Find a module in an archive returning the offset in the archive and
 * the size. If the name field is pointing to a string pointer and
 * that poniter is NULL and the offset is valid the name is extracted
 * from the archive and filled in. This is used when loading a file
 * from the archive after a symbol is found. The file name is not know
 * and could be extended which requires searching the extended string
 * table in the archive.
 *
 * @param fd Open file handle for the archive.
 * @param fsize Size of the archive.
 * @paarm name Pointer to the name string.
 * @param offset The offset of the file in the archive.
 * @param size The size of the file in the archive.
 * @param extended_names The offset in the archive for the extended names.
 * @param error The error handler called on an error.
 * @retval true The file was found in the archive.
 * @retval false The file was not found.
 */
bool rtems_rtl_obj_archive_find_obj (int                     fd,
                                     size_t                  fsize,
                                     const char**            name,
                                     off_t*                  offset,
                                     size_t*                 size,
                                     off_t*                  extended_names,
                                     rtems_rtl_archive_error error);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
