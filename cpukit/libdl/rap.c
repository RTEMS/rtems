/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <stdarg.h>
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
  pthread_once_t      once;
  rtems_mutex         lock;           /**< The RAP lock id */
  rtems_chain_control apps;           /**< List if loaded application. */
  int                 last_errno;     /**< Last error number. */
  char                last_error[64]; /**< Last error string. */
} rtems_rap_data;

/**
 * The RAP file data. This structure is allocated on the heap when a file is
 * loaded.
 */
typedef struct rtems_rap_app
{
  rtems_chain_node node;         /**< The node's link in the chain. */
  const char*      name;         /**< The file name */
  void*            handle;       /**< The dlopen handle. */
} rtems_rap_app;

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
static rtems_rap_data rap_ = { .once = PTHREAD_ONCE_INIT };

/**
 * Verbose level for the RAP loader.
 */
static bool rap_verbose;

/**
 * RAP entry call signature.
 */
typedef int (*rtems_rap_entry)(int argc, const char* argv[]);

/**
 * Forward decl.
 */
static void rtems_rap_unlock (void);

static void
rtems_rap_data_init (void)
{
  /*
   * Create the RAP lock.
   */
  rtems_mutex_init (&rap_.lock, "RAP");

  /*
   * Initialise the objects list and create any required services.
   */
  rtems_chain_initialize_empty (&rap_.apps);
}

static rtems_rap_data*
rtems_rap_lock (void)
{
  pthread_once (&rap_.once, rtems_rap_data_init);
  rtems_mutex_lock (&rap_.lock);

  return &rap_;
}

static void
rtems_rap_unlock (void)
{
  rtems_mutex_unlock (&rap_.lock);
}

static rtems_rap_app*
rtems_rap_check_handle (void* handle)
{
  rtems_rap_app*    app;
  rtems_chain_node* node;

  app = handle;
  node = rtems_chain_first (&rap_.apps);

  while (!rtems_chain_is_tail (&rap_.apps, node))
  {
    rtems_rap_app* check = (rtems_rap_app*) node;
    if (check == app)
      return app;
    node = rtems_chain_next (node);
  }

  return NULL;
}

static rtems_rap_app*
rtems_rap_app_alloc (void)
{
  rtems_rap_app* app = malloc (sizeof (rtems_rap_app));
  memset (app, 0, sizeof (rtems_rap_app));
  rtems_chain_append (&rap_.apps, &app->node);
  return app;
}

static void
rtems_rap_app_free (rtems_rap_app* app)
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
rtems_rap_match_name (rtems_rap_app* app, const char* name)
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
  rtems_rap_data* rap = rtems_rap_lock ();
  va_list         ap;
  va_start (ap, format);
  rap->last_errno = error;
  vsnprintf (rap->last_error, sizeof (rap->last_error), format, ap);
  rtems_rap_unlock ();
  va_end (ap);
}

bool
rtems_rap_load (const char* name, int mode, int argc, const char* argv[])
{
  rtems_rap_data* rap = rtems_rap_lock ();

  if (!rap)
    return false;

  if (rap_verbose)
    printf ("rap: loading '%s'\n", name);

  /*
   * See if the app has already been loaded.
   */
  if (!rtems_rap_find (name))
  {
    rtems_rap_app*  app;
    rtems_rap_entry init;
    rtems_rap_entry fini;
    size_t          size = 0;
    int             r;

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
  rtems_rap_app*  app;
  rtems_rap_entry fini;
  int             r;

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
  rtems_rap_data*   rap = rtems_rap_lock ();
  rtems_chain_node* node;

  node = rtems_chain_first (&rap->apps);

  while (!rtems_chain_is_tail (&rap->apps, node))
  {
    rtems_rap_app* app = (rtems_rap_app*) node;
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
rtems_rap_iterate (rtems_rap_iterator iterator)
{
  rtems_rap_data*   rap = rtems_rap_lock ();
  rtems_chain_node* node;
  bool              result = true;

  node = rtems_chain_first (&rap->apps);

  while (!rtems_chain_is_tail (&rap->apps, node))
  {
    rtems_rap_app* app = (rtems_rap_app*) node;
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
  rtems_rap_app* app = rtems_rap_check_handle (handle);
  if (app)
    return app->name;
  return NULL;
}

void*
rtems_rap_dl_handle (void* handle)
{
  rtems_rap_app* app = rtems_rap_check_handle (handle);
  if (app)
    return app->handle;
  return NULL;
}

int
rtems_rap_get_error (char* message, size_t max_message)
{
  rtems_rap_data* rap = rtems_rap_lock ();
  int             last_errno = rap->last_errno;
  strlcpy (message, rap->last_error, max_message);
  rtems_rap_unlock ();
  return last_errno;
}
