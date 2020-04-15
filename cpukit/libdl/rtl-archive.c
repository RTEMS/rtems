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
 * @ingroup rtl
 *
 * @brief RTEMS Run-Time Linker Archive
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include <rtems/libio_.h>

#include <rtems/rtl/rtl.h>
#include "rtl-chain-iterator.h"
#include <rtems/rtl/rtl-trace.h>
#include "rtl-string.h"
#include "rtl-error.h"

/**
 * Archive headers.
 */
#define RTEMS_RTL_AR_IDENT      "!<arch>\n"
#define RTEMS_RTL_AR_IDENT_SIZE (sizeof (RTEMS_RTL_AR_IDENT) - 1)
#define RTEMS_RTL_AR_FHDR_BASE  RTEMS_RTL_AR_IDENT_SIZE
#define RTEMS_RTL_AR_FNAME      (0)
#define RTEMS_RTL_AR_FNAME_SIZE (16)
#define RTEMS_RTL_AR_SIZE       (48)
#define RTEMS_RTL_AR_SIZE_SIZE  (10)
#define RTEMS_RTL_AR_MAGIC      (58)
#define RTEMS_RTL_AR_MAGIC_SIZE (2)
#define RTEMS_RTL_AR_FHDR_SIZE  (60)

/**
 * Read a 32bit value from the symbol table.
 */
static unsigned int
rtems_rtl_archive_read_32 (void* data)
{
  uint8_t*     b = (uint8_t*) data;
  unsigned int v = b[0];
  v = (v << 8) | b[1];
  v = (v << 8) | b[2];
  v = (v << 8) | b[3];
  return v;
}

static void
rtems_rtl_archive_set_error (int num, const char* text)
{
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: error: %3d:  %s\n", num, text);
}

static uint64_t
rtems_rtl_scan_decimal (const uint8_t* string, size_t len)
{
  uint64_t value = 0;

  while (len && (*string != ' '))
  {
    value *= 10;
    value += *string - '0';
    ++string;
    --len;
  }

  return value;
}

static bool
rtems_rtl_seek_read (int fd, off_t off, size_t len, uint8_t* buffer)
{
  if (lseek (fd, off, SEEK_SET) < 0)
    return false;
  if (read (fd, buffer, len) != len)
    return false;
  return true;
}

/**
 * Archive iterator.
 */
typedef bool (*rtems_rtl_archive_iterator) (rtems_rtl_archive* archive,
					    void*              data);

/**
 * Chain iterator data.
 */
typedef struct rtems_rtl_archive_chain_data
{
  void*                      data;      /**< User's data. */
  rtems_rtl_archive_iterator iterator;  /**< The actual iterator. */
} rtems_rtl_archive_chain_data;

static bool
rtems_rtl_archive_node_iterator (rtems_chain_node* node, void* data)
{
  rtems_rtl_archive*            archive;
  rtems_rtl_archive_chain_data* chain_data;
  archive    = (rtems_rtl_archive*) node;
  chain_data = (rtems_rtl_archive_chain_data*) data;
  return chain_data->iterator (archive, chain_data->data);
}

static void
rtems_rtl_archive_iterate_archives (rtems_rtl_archives*        archives,
                                    rtems_rtl_archive_iterator iterator,
                                    void*                      data)
{
  rtems_rtl_archive_chain_data chain_data = {
    .data = data,
    .iterator = iterator
  };
  rtems_rtl_chain_iterate (&archives->archives,
                           rtems_rtl_archive_node_iterator,
                           &chain_data);
}

static bool
rtems_rtl_rchive_name_end (const char c)
{
  return c == '\0' || c == '\n' || c == '/';
}

static const char*
rtems_rtl_archive_dup_name (const char* name)
{
  size_t len = 0;
  char*  dup;
  while (!rtems_rtl_rchive_name_end (name[len]))
    ++len;
  dup = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT, len + 1, true);
  if (dup != NULL)
    memcpy (dup, name, len);
  return dup;
}

static bool
rtems_rtl_archive_match_name (const char* file_name, const char* name)
{
  if (name != NULL)
  {
    while (!rtems_rtl_rchive_name_end (*file_name) &&
           !rtems_rtl_rchive_name_end (*name) && *file_name == *name)
    {
      ++file_name;
      ++name;
    }
    if (((*file_name == '\0') || (*file_name == '\n') || (*file_name == '/')) &&
        ((*name == '\0') || (*name == '/')))
      return true;
  }
  return false;
}

static bool
rtems_rtl_archive_set_flags (rtems_rtl_archive* archive, void* data)
{
  uint32_t mask = *((uint32_t*) data);
  archive->flags |= mask;
  return true;
}

typedef struct rtems_rtl_archive_find_data
{
  rtems_rtl_archive* archive;
  const char*        path;
} rtems_rtl_archive_find_data;

static bool
rtems_rtl_archive_finder (rtems_rtl_archive* archive, void* data)
{
  rtems_rtl_archive_find_data* find;
  find = (rtems_rtl_archive_find_data*) data;
  if (strcmp (find->path, archive->name) == 0)
  {
    find->archive = archive;
    return false;
  }
  return true;
}

static rtems_rtl_archive*
rtems_rtl_archive_find (rtems_rtl_archives* archives,
                        const char*         path)
{
  rtems_rtl_archive_find_data find = {
    .archive = NULL,
    .path = path
  };
  rtems_rtl_archive_iterate_archives (archives,
                                      rtems_rtl_archive_finder,
                                      &find);
  return find.archive;
}

/*
 * Find an object file in archive that contains the symbol we are
 * searching for.
 *
 * The symbol search is performance sensitive. The archive's symbol table being
 * searched is the symbol table in the archive created by ranlib. This table is
 * not sorted so a sorted table of pointeres to the symbols is generated after
 * loading if there are enough symbols. For small symbol tables the search is
 * linear. The entire table is held in memory. At the time of writing this code
 * the symbol table for the SPARC architecture's libc is 16k.
 *
 * The ranlib table is:
 *
 *    [4]                - size of table in bytes
 *    [0..(entries x 4)] - 4 byte binary offsets into the archive
 *                         for each symbol
 *    [0..m]             - variable length table of strings, nul
 *                         separated and sorted
 *
 * Note: The loading of an object file from an archive uses an offset in the
 *       file name to speed the loading.
 */
typedef struct rtems_rtl_archive_obj_data
{
  const char*        symbol;   /**< The symbol to search for. */
  rtems_rtl_archive* archive;  /**< The archive the symbol is found
                                *   in. */
  off_t              offset;   /**< The offset in the archive if found
                                *   else 0 */
} rtems_rtl_archive_obj_data;

static int
rtems_rtl_archive_symbol_compare (const void* a, const void* b)
{
  const rtems_rtl_archive_symbol* sa;
  const rtems_rtl_archive_symbol* sb;
  sa = (const rtems_rtl_archive_symbol*) a;
  sb = (const rtems_rtl_archive_symbol*) b;
  return strcmp (sa->label, sb->label);
}

static bool
rtems_rtl_archive_obj_finder (rtems_rtl_archive* archive, void* data)
{
  const rtems_rtl_archive_symbols* symbols = &archive->symbols;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: finder: %s: entries: %zu\n",
            archive->name, symbols->entries);

  /*
   * Make sure there is a valid symbol table.
   */
  if (symbols->base != NULL)
  {
    /*
     * Perform a linear search if there is no sorted symbol table.
     */
    rtems_rtl_archive_obj_data* search = (rtems_rtl_archive_obj_data*) data;
    if (symbols->symbols == NULL)
    {
      const char* symbol = symbols->names;
      size_t      entry;
      for (entry = 0; entry < symbols->entries; ++entry)
      {
        if (strcmp (search->symbol, symbol) == 0)
        {
          search->archive = archive;
          search->offset =
            rtems_rtl_archive_read_32 (symbols->base + ((entry + 1) * 4));
          return false;
        }
        symbol += strlen (symbol) + 1;
      }
    }
    else
    {
      rtems_rtl_archive_symbol*      match;
      const rtems_rtl_archive_symbol key = {
        .entry = -1,
        .label = search->symbol
      };
      match = bsearch (&key,
                       symbols->symbols,
                       symbols->entries,
                       sizeof (symbols->symbols[0]),
                       rtems_rtl_archive_symbol_compare);
      if (match != NULL)
      {
          search->archive = archive;
          search->offset =
            rtems_rtl_archive_read_32 (symbols->base + (match->entry * 4));
          return false;
      }
    }
  }

  /*
   * Next archive.
   */
  return true;
}

static rtems_rtl_archive*
rtems_rtl_archive_new (rtems_rtl_archives* archives,
                       const char*         path,
                       const char*         name)
{
  rtems_rtl_archive* archive;
  size_t             path_size;
  size_t             size;
  /*
   * Handle the case of the path being just '/', do not create '//'.
   */
  path_size = strlen (path);
  size = sizeof(rtems_rtl_archive) + path_size + strlen (name) + 1;
  if (path_size > 1)
    ++size;
  archive = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT, size, true);
  if (archive == NULL)
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: new: %s: no memory\n", name);
  }
  else
  {
    char* aname;
    archive->name = ((const char*) archive) + sizeof(rtems_rtl_archive);
    aname = (char*) archive->name;
    strcpy (aname, path);
    if (path_size > 1)
      strcat (aname, "/");
    strcat (aname, name);
    rtems_chain_set_off_chain (&archive->node);
    archive->flags |= RTEMS_RTL_ARCHIVE_LOAD;
  }
  return archive;
}

static void
rtems_rtl_archive_del (rtems_rtl_archive* archive)
{
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: del: %s\n",  archive->name);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, archive->symbols.base);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, archive->symbols.symbols);
  if (!rtems_chain_is_node_off_chain (&archive->node))
    rtems_chain_extract (&archive->node);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, archive);
}

static rtems_rtl_archive*
rtems_rtl_archive_get (rtems_rtl_archives* archives,
                       const char*         path,
                       const char*         name)
{
  rtems_rtl_archive* archive;
  /*
   * Getting a new archive turns the path and name into a single path the stat
   * function can use. No matter how you try some memory is needed so it is
   * easier to get a new archive object and delete it if it exists.
   */
  archive = rtems_rtl_archive_new (archives, path, name);
  if (archive != NULL)
  {
    struct stat sb;
    if (stat (archive->name, &sb) == 0)
    {
      if (S_ISREG (sb.st_mode))
      {
        rtems_rtl_archive* find_archive;
        find_archive = rtems_rtl_archive_find (archives, archive->name);
        if (find_archive == NULL)
        {
          rtems_chain_append (&archives->archives, &archive->node);
        }
        else
        {
          rtems_rtl_archive_del (archive);
          archive = find_archive;
        }
        archive->flags &= ~RTEMS_RTL_ARCHIVE_REMOVE;
        if (archive->mtime != sb.st_mtime)
        {
          archive->flags |= RTEMS_RTL_ARCHIVE_LOAD;
          archive->size = sb.st_size;
          archive->mtime = sb.st_mtime;
        }
      }
    }
  }
  return archive;
}

static bool
rtems_rtl_archives_load_config (rtems_rtl_archives* archives)
{
  struct stat sb;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: config load: %s\n", archives->config_name);

  if (archives->config_name == NULL)
    return false;

  if (stat (archives->config_name, &sb) < 0)
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: no config: %s\n", archives->config_name);
    return false;
  }

  /*
   * If the configuration has change reload it.
   */
  if (sb.st_mtime != archives->config_mtime)
  {
    int     fd;
    ssize_t r;
    char*   s;
    bool    in_comment;

    archives->config_mtime = sb.st_mtime;
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) archives->config);
    archives->config_length = 0;
    archives->config =
      rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT, sb.st_size + 1, true);
    if (archives->config == NULL)
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
        printf ("rtl: archive: no memory for config\n");
      return false;
    }

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: config load: read %s\n", archives->config_name);

    fd = open (archives->config_name, O_RDONLY);
    if (fd < 0)
    {
      rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) archives->config);
      archives->config = NULL;
      archives->config_length = 0;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
        printf ("rtl: archive: config open error: %s\n", strerror (errno));
      return false;
    }

    r = read (fd, (void*) archives->config, sb.st_size);
    if (r < 0)
    {
      close (fd);
      rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) archives->config);
      archives->config = NULL;
      archives->config_length = 0;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
        printf ("rtl: archive: config read error: %s\n", strerror (errno));
      return false;
    }

    close (fd);
    archives->config_length = r;

    /*
     * Remove comments.
     */
    s = (char*) archives->config;
    in_comment = false;
    for (r = 0; r < archives->config_length; ++r, ++s)
    {
      if (*s == '#')
        in_comment = true;
      if (in_comment)
      {
        if (*s == '\n')
          in_comment = false;
        *s = '\0';
      }
    }

    /*
     * Create lines by turning '\r' and '\n' to '\0'.
     */
    s = (char*) archives->config;
    for (r = 0; r < archives->config_length; ++r, ++s)
    {
      if (*s == '\r' || *s == '\n')
        *s = '\0';
    }

    /*
     * Remove leading and trailing white space.
     */
    s = (char*) archives->config;
    r = 0;
    while (r < archives->config_length)
    {
      if (s[r] == '\0')
      {
        ++r;
      }
      else
      {
        size_t ls = strlen (&s[r]);
        size_t b = 0;
        while (b < ls && isspace (s[r + b]))
        {
          s[r + b] = '\0';
          ++b;
        }
        b = ls - 1;
        while (b > 0 && isspace (s[r + b]))
        {
          s[r + b] = '\0';
          --b;
        }
        r += ls;
      }
    }

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    {
      int line = 1;
      printf ("rtl: archive: config:\n");
      r = 0;
      while (r < archives->config_length)
      {
        const char* cs = &archives->config[r];
        size_t      len = strlen (cs);
        if (len > 0)
        {
          printf (" %3d: %s\n", line, cs);
          ++line;
        }
        r += len + 1;
      }
    }
  }

  return true;
}

void
rtems_rtl_archives_open (rtems_rtl_archives* archives, const char* config)
{
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: open: %s\n", config);
  memset (archives, 0, sizeof (rtems_rtl_archives));
  archives->config_name = rtems_rtl_strdup (config);
  rtems_chain_initialize_empty (&archives->archives);
}

void
rtems_rtl_archives_close (rtems_rtl_archives* archives)
{
  rtems_chain_node* node;
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: close: count=%zu\n",
            rtems_chain_node_count_unprotected (&archives->archives));
  node = rtems_chain_first (&archives->archives);
  while (!rtems_chain_is_tail (&archives->archives, node))
  {
    rtems_rtl_archive* archive = (rtems_rtl_archive*) node;
    rtems_chain_node*  next_node = rtems_chain_next (node);
    rtems_rtl_archive_del (archive);
    node = next_node;
  }
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) archives->config);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, archives);
}

static void
rtems_rtl_archives_remove (rtems_rtl_archives* archives)
{
  rtems_chain_node* node = rtems_chain_first (&archives->archives);
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: refresh: remove: checking %zu archive(s)\n",
            rtems_chain_node_count_unprotected (&archives->archives));
  while (!rtems_chain_is_tail (&archives->archives, node))
  {
    rtems_rtl_archive* archive = (rtems_rtl_archive*) node;
    rtems_chain_node*  next_node = rtems_chain_next (node);
    if ((archive->flags & RTEMS_RTL_ARCHIVE_REMOVE) != 0)
    {
      archive->flags &= ~RTEMS_RTL_ARCHIVE_REMOVE;
      if ((archive->flags & RTEMS_RTL_ARCHIVE_USER_LOAD) == 0)
        rtems_rtl_archive_del (archive);
    }
    node = next_node;
  }
}

static bool
rtems_rtl_archive_loader (rtems_rtl_archive* archive, void* data)
{
  int* loaded = (int*) data;

  if ((archive->flags & RTEMS_RTL_ARCHIVE_LOAD) != 0)
  {
    int         fd;
    off_t       offset = 0;
    size_t      size = 0;
    const char* name = "/";

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: loader: %s\n", archive->name);

    fd = open (archive->name, O_RDONLY);
    if (fd < 0)
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
        printf ("rtl: archive: loader: open error: %s: %s\n",
                archive->name, strerror (errno));
      rtems_rtl_archive_set_error (errno, "opening archive file");
      return true;
    }

    if (rtems_rtl_obj_archive_find_obj (fd,
                                        archive->size,
                                        &name,
                                        &offset,
                                        &size,
                                        &archive->enames,
                                        rtems_rtl_archive_set_error))
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
        printf ("rtl: archive: loader: symbols: off=0x%08jx size=%zu\n",
                offset, size);

      /*
       * Reallocate the symbol table memory if it has changed size.
       * Note, an updated library may have the same symbol table.
       */
      if (archive->symbols.size != size)
      {
        rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, archive->symbols.base);
        archive->symbols.base = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_SYMBOL,
                                                     size,
                                                     false);
        if (archive->symbols.base == NULL)
        {
          close (fd);
          memset (&archive->symbols, 0, sizeof (archive->symbols));
          rtems_rtl_archive_set_error (ENOMEM, "symbol table memory");
          return true;
        }
      }

      /*
       * Read the symbol table into memory and hold.
       */
      if (!rtems_rtl_seek_read (fd, offset, size, archive->symbols.base))
      {
        rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, archive->symbols.base);
        close (fd);
        memset (&archive->symbols, 0, sizeof (archive->symbols));
        rtems_rtl_archive_set_error (errno, "reading symbols");
        return true;
      }

      /*
       * The first 4 byte value is the number of entries. Range check the
       * value so the alloc size does not overflow (Coverity 1442636).
       */
      archive->symbols.entries =
        rtems_rtl_archive_read_32 (archive->symbols.base);
      if (archive->symbols.entries >= (SIZE_MAX / sizeof (rtems_rtl_archive_symbol)))
      {
        rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, archive->symbols.base);
        close (fd);
        memset (&archive->symbols, 0, sizeof (archive->symbols));
        rtems_rtl_archive_set_error (errno, "too many symbols");
        return true;
      }

      archive->symbols.size   = size;
      archive->symbols.names  = archive->symbols.base;
      archive->symbols.names += (archive->symbols.entries + 1) * 4;

      /*
       * Create a sorted symbol table.
       */
      size = archive->symbols.entries * sizeof (rtems_rtl_archive_symbol);
      archive->symbols.symbols =
        rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_SYMBOL, size, true);
      if (archive->symbols.symbols != NULL)
      {
        const char* symbol = archive->symbols.names;
        size_t      e;
        for (e = 0; e < archive->symbols.entries; ++e)
        {
          archive->symbols.symbols[e].entry = e + 1;
          archive->symbols.symbols[e].label = symbol;
          symbol += strlen (symbol) + 1;
        }
        qsort (archive->symbols.symbols,
               archive->symbols.entries,
               sizeof (rtems_rtl_archive_symbol),
               rtems_rtl_archive_symbol_compare);
      }

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
        printf ("rtl: archive: loader: symbols: " \
                "base=%p entries=%zu names=%p (0x%08x) symbols=%p\n",
                archive->symbols.base,
                archive->symbols.entries,
                archive->symbols.names,
                (unsigned int) (archive->symbols.entries + 1) * 4,
                archive->symbols.symbols);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVE_SYMS) &&
          archive->symbols.entries > 0)
      {
        size_t e;
        printf ("rtl: archive: symbols: %s\n", archive->name );
        for (e = 0; e < archive->symbols.entries; ++e)
        {
          printf(" %6zu: %6zu %s\n", e + 1,
                 archive->symbols.symbols[e].entry,
                 archive->symbols.symbols[e].label);
        }
      }
    }

    close (fd);

    archive->flags &= ~RTEMS_RTL_ARCHIVE_LOAD;

    ++(*loaded);
  }

  return true;
}

static bool
rtems_rtl_archives_load (rtems_rtl_archives* archives)
{
  int loaded = 0;
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: archive: load\n");
  rtems_rtl_archive_iterate_archives (archives,
                                      rtems_rtl_archive_loader,
                                      &loaded);
  return loaded > 0;
}

bool
rtems_rtl_archives_refresh (rtems_rtl_archives* archives)
{
  size_t   config_path = 0;
  uint32_t flags = RTEMS_RTL_ARCHIVE_REMOVE;

  /*
   * Reload the configuration file if it has not been loaded or has been
   * updated.
   */
  if (!rtems_rtl_archives_load_config (archives))
    return false;

  /*
   * Assume all existing archives are to be removed. If an existing archive
   * is ccnfigured and found teh remove flags is cleared. At the of the
   * refresh end remove any archives with this flag still set.
   */
  rtems_rtl_archive_iterate_archives (archives,
                                      rtems_rtl_archive_set_flags,
                                      &flags);

  while (config_path < archives->config_length)
  {
    const char* dirname = &archives->config[config_path];
    char*       basename;
    const char  root[2] = { '/', '\0' };
    DIR*        dir;

    if (*dirname == '\0')
    {
      ++config_path;
      continue;
    }

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: refresh: %s\n", dirname);

    config_path += strlen (dirname);

    /*
     * Relative paths do not work in the config. Must be absolute.
     */
    if (dirname[0] != '/')
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
        printf ("rtl: archive: refresh: relative paths ignored: %s\n", dirname);
      continue;
    }

    /*
     * Scan the parent directory of the glob path matching file names.
     */
    basename = strrchr (dirname, '/');
    if (basename == NULL)
      continue;

    if (basename == dirname)
      dirname = &root[0];

    *basename = '\0';
    ++basename;

    dir = opendir (dirname);

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: refresh: %s %sfound\n",
              dirname, dir == NULL ? ": not " : "");

    if (dir != NULL)
    {
      while (true)
      {
        struct dirent  entry;
        struct dirent* result = NULL;

        if (readdir_r (dir, &entry, &result) != 0)
        {
          if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
            printf ("rtl: archive: refresh: readdir error\n");
          break;
        }

        if (result == NULL)
          break;

        if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
          printf ("rtl: archive: refresh: checking: %s (pattern: %s)\n",
                  entry.d_name, basename);

        if (fnmatch (basename, entry.d_name, 0) == 0)
        {
          rtems_rtl_archive* archive;
          archive = rtems_rtl_archive_get (archives, dirname, entry.d_name);
          if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
            printf ("rtl: archive: refresh: %s: %sfound\n",
                    entry.d_name, archive == NULL ? ": not " : "");
        }
      }
      closedir (dir);
    }

    --basename;
    *basename = '/';
  }

  /*
   * Remove all archives flagged to be removed.
   */
  rtems_rtl_archives_remove (archives);

  /*
   * Load any new archives. If any are loaded set the archive search flag in
   * any unresolved external symbols so the archives are searched. The archive
   * search flag avoids searching for symbols we know are not in the known
   * archives,
   */
  if (rtems_rtl_archives_load (archives))
    rtems_rtl_unresolved_set_archive_search ();

  return true;
}

bool
rtems_rtl_archive_load (rtems_rtl_archives* archives, const char* name)
{
  if (archives != NULL)
  {
    rtems_rtl_archive* archive;
    int                loaded = 0;

    archive = rtems_rtl_archive_get (archives, "", name);
    if (archive == NULL)
    {
      rtems_rtl_set_error (ENOENT, "archive not found");
      return false;
    }

    archive->flags |= RTEMS_RTL_ARCHIVE_USER_LOAD;

    rtems_rtl_archive_loader (archive, &loaded);
    if (loaded == 0)
    {
      rtems_rtl_archive_del (archive);
      rtems_rtl_set_error (ENOENT, "archive load falied");
    }

    return true;
  }
  return false;
}

rtems_rtl_archive_search
rtems_rtl_archive_obj_load (rtems_rtl_archives* archives,
                            const char*         symbol,
                            bool                load)
{
  rtems_rtl_obj*       obj;
  rtems_chain_control* pending;
  int                  fd;
  size_t               archive_count;

  rtems_rtl_archive_obj_data search = {
    .symbol  = symbol,
    .archive = NULL,
    .offset  = 0
  };

  archive_count = rtems_chain_node_count_unprotected (&archives->archives);

  if (archive_count == 0)
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: load: no archives\n");
    return rtems_rtl_archive_search_no_config;
  }

  pending = rtems_rtl_pending_unprotected ();
  if (pending == NULL)
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: load: no pending list\n");
    return rtems_rtl_archive_search_not_found;
  }

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: load: searching %zu archives\n", archive_count);

  rtems_rtl_archive_iterate_archives (archives,
                                      rtems_rtl_archive_obj_finder,
                                      &search);

  if (search.archive == NULL)
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: load: not found: %s\n", symbol);
    return rtems_rtl_archive_search_not_found;
  }

  if (!load)
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: load: found (no load): %s\n", symbol);
    return rtems_rtl_archive_search_found;
  }

  obj = rtems_rtl_obj_alloc ();
  if (obj == NULL)
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: alloc: no memory: %s\n",
              search.archive->name);
    return rtems_rtl_archive_search_error;
  }

  obj->aname = rtems_rtl_strdup (search.archive->name);

  fd = open (obj->aname, O_RDONLY);
  if (fd < 0)
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: load: open error: %s: %s\n",
              obj->aname, strerror (errno));
    rtems_rtl_obj_free (obj);
    return rtems_rtl_archive_search_error;
  }

  obj->oname = NULL;
  obj->ooffset = search.offset;

  if (!rtems_rtl_obj_archive_find_obj (fd,
                                       search.archive->size,
                                       &obj->oname,
                                       &obj->ooffset,
                                       &obj->fsize,
                                       &search.archive->enames,
                                       rtems_rtl_archive_set_error))
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: load: load error: %s:%s\n",
              obj->aname, obj->oname);
    close (fd);
    rtems_rtl_obj_free (obj);
    return rtems_rtl_archive_search_error;
  }

  obj->fname = rtems_rtl_strdup (obj->aname);
  obj->ooffset -= RTEMS_RTL_AR_FHDR_SIZE;
  obj->fsize = search.archive->size;

  close (fd);

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: loading: %s:%s@0x%08jx size:%zu\n",
            obj->aname, obj->oname, obj->ooffset, obj->fsize);

  rtems_chain_append (pending, &obj->link);

  if (!rtems_rtl_obj_load (obj))
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
      printf ("rtl: archive: loading: error: %s:%s@0x%08jx: %s\n",
              obj->aname, obj->oname, obj->ooffset,
              rtems_rtl_last_error_unprotected ());
    rtems_chain_extract (&obj->link);
    rtems_rtl_obj_free (obj);
    rtems_rtl_obj_caches_flush ();
    return rtems_rtl_archive_search_error;
  }

  rtems_rtl_obj_caches_flush ();

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: loading: loaded: %s:%s@0x%08jx\n",
            obj->aname, obj->oname, obj->ooffset);

  return rtems_rtl_archive_search_loaded;
}

bool
rtems_rtl_obj_archive_find_obj (int                     fd,
                                size_t                  fsize,
                                const char**            name,
                                off_t*                  ooffset,
                                size_t*                 osize,
                                off_t*                  extended_file_names,
                                rtems_rtl_archive_error error)
{
  uint8_t header[RTEMS_RTL_AR_FHDR_SIZE];
  bool    scanning;

  if (name == NULL)
  {
    error (errno, "invalid object name");
    *ooffset = 0;
    *osize = 0;
    return false;
  }

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ARCHIVES))
    printf ("rtl: archive: find obj: %s @ 0x%08jx\n", *name, *ooffset);

  if (read (fd, &header[0], RTEMS_RTL_AR_IDENT_SIZE) !=  RTEMS_RTL_AR_IDENT_SIZE)
  {
    error (errno, "reading archive identifer");
    *ooffset = 0;
    *osize = 0;
    return false;
  }

  if (memcmp (header, RTEMS_RTL_AR_IDENT, RTEMS_RTL_AR_IDENT_SIZE) != 0)
  {
    error (EINVAL, "invalid archive identifer");
    *ooffset = 0;
    *osize = 0;
    return false;
  }

  /*
   * Seek to the current offset in the archive and if we have a valid archive
   * file header present check the file name for a match with the oname field
   * of the object descriptor. If the archive header is not valid and it is the
   * first pass reset the offset and start the search again in case the offset
   * provided is not valid any more.
   *
   * The archive can have a symbol table at the start. Ignore it. A symbol
   * table has the file name '/'.
   *
   * The archive can also have the GNU extended file name table. This
   * complicates the processing. If the object's file name starts with '/' the
   * remainder of the file name is an offset into the extended file name
   * table. To find the extended file name table we need to scan from start of
   * the archive for a file name of '//'. Once found we remeber the table's
   * start and can direct seek to file name location. In other words the scan
   * only happens once.
   *
   * If the name had the offset encoded we go straight to that location.
   */

  if (*ooffset != 0)
    scanning = false;
  else
  {
    if (*name == NULL)
    {
      error (errno, "invalid object name and archive offset");
      *ooffset = 0;
      *osize = 0;
      return false;
    }
    scanning = true;
    *ooffset = RTEMS_RTL_AR_FHDR_BASE;
    *osize = 0;
  }

  while (*ooffset < fsize)
  {
    /*
     * Clean up any existing data.
     */
    memset (header, 0, sizeof (header));

    if (!rtems_rtl_seek_read (fd, *ooffset, RTEMS_RTL_AR_FHDR_SIZE, &header[0]))
    {
      error (errno, "seek/read archive file header");
      *ooffset = 0;
      *osize = 0;
      return false;
    }

    if ((header[RTEMS_RTL_AR_MAGIC] != 0x60) ||
        (header[RTEMS_RTL_AR_MAGIC + 1] != 0x0a))
    {
      if (scanning)
      {
        error (EINVAL, "invalid archive file header");
        *ooffset = 0;
        *osize = 0;
        return false;
      }

      scanning = true;
      *ooffset = RTEMS_RTL_AR_FHDR_BASE;
      continue;
    }

    /*
     * The archive header is always aligned to an even address.
     */
    *osize = (rtems_rtl_scan_decimal (&header[RTEMS_RTL_AR_SIZE],
                                      RTEMS_RTL_AR_SIZE_SIZE) + 1) & ~1;

    /*
     * Check for the GNU extensions.
     */
    if (header[0] == '/')
    {
      off_t extended_off;

      switch (header[1])
      {
        case ' ':
          /*
           * SVR4/GNU Symbols table. Nothing more to do.
           */
          *ooffset += RTEMS_RTL_AR_FHDR_SIZE;
          return true;
        case '/':
          /*
           * Extended file names table. Remember. If asked to find this file
           * return the result.
           */
          *extended_file_names = *ooffset + RTEMS_RTL_AR_FHDR_SIZE;
          if (*name[0] == '/' && *name[1] == '/')
          {
            *ooffset = *ooffset + RTEMS_RTL_AR_FHDR_SIZE;
            return true;
          }
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          /*
           * Offset into the extended file name table. If we do not have the
           * offset to the extended file name table find it.
           */
          extended_off =
            rtems_rtl_scan_decimal (&header[1], RTEMS_RTL_AR_FNAME_SIZE);

          if (*extended_file_names == 0)
          {
            off_t off = RTEMS_RTL_AR_IDENT_SIZE;
            while (*extended_file_names == 0)
            {
              off_t esize;

              if (!rtems_rtl_seek_read (fd, off,
                                        RTEMS_RTL_AR_FHDR_SIZE, &header[0]))
              {
                error (errno, "seeking/reading archive ext file name header");
                *ooffset = 0;
                *osize = 0;
                return false;
              }

              if ((header[RTEMS_RTL_AR_MAGIC] != 0x60) ||
                  (header[RTEMS_RTL_AR_MAGIC + 1] != 0x0a))
              {
                error (errno, "invalid archive file header");
                *ooffset = 0;
                *osize = 0;
                return false;
              }

              if ((header[0] == '/') && (header[1] == '/'))
              {
                *extended_file_names = off + RTEMS_RTL_AR_FHDR_SIZE;
                break;
              }

              esize =
                (rtems_rtl_scan_decimal (&header[RTEMS_RTL_AR_SIZE],
                                         RTEMS_RTL_AR_SIZE_SIZE) + 1) & ~1;
              off += esize + RTEMS_RTL_AR_FHDR_SIZE;
            }
          }

          if (*extended_file_names != 0)
          {
            /*
             * We know the offset in the archive to the extended file. Read the
             * name from the table and compare with the name we are after.
             */
            #define RTEMS_RTL_MAX_FILE_SIZE (256)
            char ename[RTEMS_RTL_MAX_FILE_SIZE];

            if (!rtems_rtl_seek_read (fd, *extended_file_names + extended_off,
                                      RTEMS_RTL_MAX_FILE_SIZE, (uint8_t*) &ename[0]))
            {
              error (errno, "invalid archive ext file seek/read");
              *ooffset = 0;
              *osize = 0;
              return false;
            }

            /*
             * If there is no name memory the user is asking us to return the
             * name in the archive at the offset.
             */
            if (*name == NULL)
              *name = rtems_rtl_archive_dup_name (ename);
            if (rtems_rtl_archive_match_name (*name, ename))
            {
              *ooffset += RTEMS_RTL_AR_FHDR_SIZE;
              return true;
            }
          }
          break;
        default:
          /*
           * Ignore the file because we do not know what it it.
           */
          break;
      }
    }
    else
    {
      const char* ename = (const char*) &header[RTEMS_RTL_AR_FNAME];
      if (*name == NULL)
        *name = rtems_rtl_archive_dup_name (ename);
      if (rtems_rtl_archive_match_name (*name, ename))
      {
        *ooffset += RTEMS_RTL_AR_FHDR_SIZE;
        return true;
      }
    }

    *ooffset += *osize + RTEMS_RTL_AR_FHDR_SIZE;
  }

  error (ENOENT, "object file not found");
  *ooffset = 0;
  *osize = 0;
  return false;

}
