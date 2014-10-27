/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rap
 *
 * @brief RTEMS Application Loader
 *
 * This is the RAP implementation.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtems/libio_.h>

#include <dlfcn.h>
#include <rtems/rtl/rap.h>
#include <rtems/rtl/rtl.h>

#include "rtl-find-file.h"

/**
 * The global RAP data. This structure is allocated on the heap when the first
 * call to location an application and is never released.
 */
typedef struct rtems_rap_data_s
{
  rtems_id            lock;           /**< The RAP lock id */
  rtems_chain_control apps;           /**< List if loaded application. */
  int                 last_errno;     /**< Last error number. */
  char                last_error[64]; /**< Last error string. */
} rtems_rap_data_t;

/**
 * The RAP file data. This structure is allocated on the heap when a file is
 * loaded.
 */
typedef struct rtems_rap_app_s
{
  rtems_chain_node node;         /**< The node's link in the chain. */
  const char*      name;         /**< The file name */
  void*            handle;       /**< The dlopen handle. */
} rtems_rap_app_t;

/**
 * Semaphore configuration to create a mutex.
 */
#define RTEMS_MUTEX_ATTRIBS \
  (RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE | \
   RTEMS_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL)

/**
 * RTL entry.
 */
#if (RTL_GLUE(__USER_LABEL_PREFIX__, 1) == RTL_GLUE(_, 1))
  #define RTL_ENTRY_POINT "_rtems"
#else
  #define RTL_ENTRY_POINT "rtems"
#endif

/**
 * Static RAP data is returned to the user when the loader is locked.
 */
static rtems_rap_data_t rap_;

/**
 * Verbose level for the RAP loader.
 */
static bool rap_verbose;

/**
 * RAP entry call signature.
 */
typedef int (*rtems_rap_entry_t)(int argc, const char* argv[]);

/**
 * Forward decl.
 */
static bool rtems_rap_unlock (void);

static bool
rtems_rap_data_init (void)
{
  /*
   * Lock the RAP. We only create a lock if a call is made. First we test if a
   * lock is present. If one is present we lock it. If not the libio lock is
   * locked and we then test the lock again. If not present we create the lock
   * then release libio lock.
   */
  if (!rap_.lock)
  {
    rtems_libio_lock ();

    if (!rap_.lock)
    {
      rtems_status_code sc;
      rtems_id          lock;

      /*
       * Create the RAP lock.
       */
      sc = rtems_semaphore_create (rtems_build_name ('R', 'A', 'P', '_'),
                                   1, RTEMS_MUTEX_ATTRIBS,
                                   RTEMS_NO_PRIORITY, &lock);
      if (sc != RTEMS_SUCCESSFUL)
        return false;

      sc = rtems_semaphore_obtain (lock, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      if (sc != RTEMS_SUCCESSFUL)
      {
        rtems_semaphore_delete (lock);
        return false;
      }

      rap_.lock = lock;

      /*
       * Initialise the objects list and create any required services.
       */
      rtems_chain_initialize_empty (&rap_.apps);
    }

    rtems_libio_unlock ();

    rtems_rap_unlock ();
  }
  return true;
}

static rtems_rap_data_t*
rtems_rap_lock (void)
{
  rtems_status_code sc;

  if (!rtems_rap_data_init ())
    return NULL;

  sc = rtems_semaphore_obtain (rap_.lock,
                               RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
  {
    errno = EINVAL;
    return NULL;
  }

  return &rap_;
}

static bool
rtems_rap_unlock (void)
{
  /*
   * Not sure any error should be returned or an assert.
   */
  rtems_status_code sc;
  sc = rtems_semaphore_release (rap_.lock);
  if ((sc != RTEMS_SUCCESSFUL) && (errno == 0))
  {
    errno = EINVAL;
    return false;
  }
  return true;
}

static rtems_rap_app_t*
rtems_rap_check_handle (void* handle)
{
  rtems_rap_app_t*  app;
  rtems_chain_node* node;

  app = handle;
  node = rtems_chain_first (&rap_.apps);

  while (!rtems_chain_is_tail (&rap_.apps, node))
  {
    rtems_rap_app_t* check = (rtems_rap_app_t*) node;
    if (check == app)
      return app;
    node = rtems_chain_next (node);
  }

  return NULL;
}

static rtems_rap_app_t*
rtems_rap_app_alloc (void)
{
  rtems_rap_app_t* app = malloc (sizeof (rtems_rap_app_t));
  memset (app, 0, sizeof (rtems_rap_app_t));
  rtems_chain_append (&rap_.apps, &app->node);
  return app;
}

static void
rtems_rap_app_free (rtems_rap_app_t* app)
{
  if (app->handle)
  {
    dlclose (app->handle);
    app->handle = NULL;
  }

  if (!rtems_chain_is_node_off_chain (&app->node))
    rtems_chain_extract (&app->node);
}

static bool
rtems_rap_match_name (rtems_rap_app_t* app, const char* name)
{
  const char* a;

  /*
   * Assume the app name is absolute, ie points to the file on disk. This means
   * there is at least one delimiter in the name.
   */

  if (strncmp (app->name, name, strlen (name)) == 0)
    return true;

  a = app->name + strlen (app->name) - 1;

  while (a >= app->name)
  {
    if (rtems_filesystem_is_delimiter (*a))
    {
      const char* n = name;

      ++a;

      while (*a && *n)
      {
        if (*a == '.')
        {
          if (*n == '\0')
            return true;
        }

        ++a;
        ++n;
      }

      return false;
    }

    --a;
  }

  return false;
}

static void
rtems_rap_get_rtl_error (void)
{
  rap_.last_errno =
    rtems_rtl_get_error (rap_.last_error, sizeof (rap_.last_error));
}

static void
rtems_rap_set_error (int error, const char* format, ...)
{
  rtems_rap_data_t* rap = rtems_rap_lock ();
  va_list           ap;
  va_start (ap, format);
  rap->last_errno = error;
  vsnprintf (rap->last_error, sizeof (rap->last_error), format, ap);
  rtems_rap_unlock ();
  va_end (ap);
}

bool
rtems_rap_load (const char* name, int mode, int argc, const char* argv[])
{
  rtems_rap_data_t* rap = rtems_rap_lock ();

  if (!rap)
    return false;

  if (rap_verbose)
    printf ("rap: loading '%s'\n", name);

  /*
   * See if the app has already been loaded.
   */
  if (!rtems_rap_find (name))
  {
    rtems_rap_app_t*  app;
    rtems_rap_entry_t init;
    rtems_rap_entry_t fini;
    size_t            size = 0;
    int               r;

    /*
     * Allocate a new application descriptor and attempt to load it.
     */
    app = rtems_rap_app_alloc ();
    if (app == NULL)
    {
      rtems_rap_set_error (ENOMEM, "no memory for application");
      rtems_rap_unlock ();
      return false;
    }

    /*
     * Find the file in the file system using the search path.
     */
    if (!rtems_rtl_find_file (name, getenv ("PATH"), &app->name, &size))
    {
      rtems_rap_set_error (ENOENT, "file not found");
      rtems_rap_app_free (app);
      rtems_rap_unlock ();
      return false;
    }

    app->handle = dlopen (app->name, RTLD_NOW | mode);
    if (!app->handle)
    {
      rtems_rap_get_rtl_error ();
      rtems_rap_app_free (app);
      rtems_rap_unlock ();
      return false;
    }

    init = dlsym (app->handle, RTL_ENTRY_POINT);
    if (!init)
    {
      rtems_rap_get_rtl_error ();
      rtems_rap_app_free (app);
      rtems_rap_unlock ();
      return false;
    }

    fini = dlsym (app->handle, RTL_ENTRY_POINT);
    if (!fini)
    {
      rtems_rap_get_rtl_error ();
      rtems_rap_app_free (app);
      rtems_rap_unlock ();
      return false;
    }

    r = init (argc, argv);
    if (r != 0)
    {
      rtems_rap_set_error (r, "init call failure");
      rtems_rap_app_free (app);
      rtems_rap_unlock ();
      return false;
    }
  }

  rtems_rap_unlock ();

  return true;
}

bool
rtems_rap_unload (const char* name)
{
  rtems_rap_app_t*  app;
  rtems_rap_entry_t fini;
  int               r;

  rtems_rap_lock ();

  app = rtems_rap_find (name);

  if (rap_verbose)
    printf ("rap: unloading '%s'\n", name);

  if (!app)
  {
    rtems_rap_set_error (ENOENT, "invalid handle");
    rtems_rap_unlock ();
    return false;
  }

  fini = dlsym (app->handle, RTL_ENTRY_POINT);
  if (!fini)
  {
    rtems_rap_get_rtl_error ();
    rtems_rap_unlock ();
    return false;
  }

  r = fini (0, NULL);
  if (r != 0)
  {
    rtems_rap_set_error (r, "fini failure");
    rtems_rap_unlock ();
    return false;
  }

  rtems_rap_app_free (app);
  rtems_rap_unlock ();

  return true;
}

void*
rtems_rap_find (const char* name)
{
  rtems_rap_data_t* rap = rtems_rap_lock ();
  rtems_chain_node* node;

  node = rtems_chain_first (&rap->apps);

  while (!rtems_chain_is_tail (&rap->apps, node))
  {
    rtems_rap_app_t* app = (rtems_rap_app_t*) node;
    if (rtems_rap_match_name (app, name))
    {
      rtems_rap_unlock ();
      return app;
    }
    node = rtems_chain_next (node);
  }

  rtems_rap_unlock ();

  return NULL;
}

bool
rtems_rap_iterate (rtems_rap_iterator_t iterator)
{
  rtems_rap_data_t* rap = rtems_rap_lock ();
  rtems_chain_node* node;
  bool              result = true;

  node = rtems_chain_first (&rap->apps);

  while (!rtems_chain_is_tail (&rap->apps, node))
  {
    rtems_rap_app_t* app = (rtems_rap_app_t*) node;
    result = iterator (app);
    if (!result)
      break;
    node = rtems_chain_next (node);
  }

  rtems_rap_unlock ();

  return result;
}

const char*
rtems_rap_name (void* handle)
{
  rtems_rap_app_t* app = rtems_rap_check_handle (handle);
  if (app)
    return app->name;
  return NULL;
}

void*
rtems_rap_dl_handle (void* handle)
{
  rtems_rap_app_t* app = rtems_rap_check_handle (handle);
  if (app)
    return app->handle;
  return NULL;
}

int
rtems_rap_get_error (char* message, size_t max_message)
{
  rtems_rap_data_t* rap = rtems_rap_lock ();
  int               last_errno = rap->last_errno;
  strncpy (message, rap->last_error, sizeof (rap->last_error));
  rtems_rap_unlock ();
  return last_errno;
}
