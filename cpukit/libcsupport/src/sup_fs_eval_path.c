/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/libio_.h>

static size_t get_parentpathlen(const char *path, size_t pathlen)
{
  while (pathlen > 0) {
    size_t i = pathlen - 1;

    if (rtems_filesystem_is_delimiter(path [i])) {
      return pathlen;
    }

    pathlen = i;
  }

  return 0;
}

static void set_startloc(
  rtems_filesystem_eval_path_context_t *ctx,
  rtems_filesystem_global_location_t *const *global_root_ptr,
  rtems_filesystem_global_location_t *const *global_current_ptr
)
{
  if (ctx->pathlen > 0) {
    char c = ctx->path [0];

    ctx->rootloc = rtems_filesystem_global_location_obtain(global_root_ptr);

    if (rtems_filesystem_is_delimiter(c)) {
      ++ctx->path;
      --ctx->pathlen;
      ctx->startloc = rtems_filesystem_global_location_obtain(
        &ctx->rootloc
      );
    } else {
      ctx->startloc = rtems_filesystem_global_location_obtain(
        global_current_ptr
      );
    }
  } else {
    ctx->rootloc = rtems_filesystem_global_location_obtain_null();
    ctx->startloc = rtems_filesystem_global_location_obtain_null();
    errno = ENOENT;
  }
}

static void check_access(
  rtems_filesystem_eval_path_context_t *ctx,
  int eval_flags
)
{
  const rtems_filesystem_location_info_t *currentloc = &ctx->currentloc;
  const rtems_filesystem_mount_table_entry_t *mt_entry = currentloc->mt_entry;

  if ((eval_flags & RTEMS_FS_PERMS_WRITE) == 0 || mt_entry->writeable) {
    struct stat st;
    int rv;

    st.st_mode = 0;
    st.st_uid = 0;
    st.st_gid = 0;
    rv = (*currentloc->handlers->fstat_h)(currentloc, &st);
    if (rv == 0) {
      bool access_ok = rtems_filesystem_check_access(
        eval_flags,
        st.st_mode,
        st.st_uid,
        st.st_gid
      );

      if (!access_ok) {
        rtems_filesystem_eval_path_error(ctx, EACCES);
      }
    } else {
      rtems_filesystem_eval_path_error(ctx, 0);
    }
  } else {
    rtems_filesystem_eval_path_error(ctx, EROFS);
  }
}

void rtems_filesystem_eval_path_continue(
  rtems_filesystem_eval_path_context_t *ctx
)
{
  int eval_flags;

  while (ctx->pathlen > 0) {
    (*ctx->currentloc.mt_entry->ops->eval_path_h)(ctx);
  }

  eval_flags = rtems_filesystem_eval_path_get_flags(ctx);
  if (rtems_filesystem_eval_path_has_token(ctx)) {
    bool make = (eval_flags & RTEMS_FS_MAKE) != 0;

    if (make) {
      check_access(ctx, RTEMS_FS_PERMS_WRITE);
    } else {
      rtems_filesystem_eval_path_error(ctx, ENOENT);
    }
  } else {
    bool exclusive = (eval_flags & RTEMS_FS_EXCLUSIVE) != 0;

    if (!exclusive) {
      check_access(ctx, ctx->flags);
    } else {
      rtems_filesystem_eval_path_error(ctx, EEXIST);
    }
  }
}

static rtems_filesystem_location_info_t *
eval_path_start(
  rtems_filesystem_eval_path_context_t *ctx,
  const char *path,
  size_t pathlen,
  int eval_flags,
  rtems_filesystem_global_location_t *const *global_root_ptr,
  rtems_filesystem_global_location_t *const *global_current_ptr
)
{
  memset(ctx, 0, sizeof(*ctx));

  ctx->path = path;
  ctx->pathlen = pathlen;
  ctx->flags = eval_flags;

  set_startloc(ctx, global_root_ptr, global_current_ptr);

  rtems_filesystem_instance_lock(&ctx->startloc->location);

  rtems_filesystem_location_clone(
    &ctx->currentloc,
    &ctx->startloc->location
  );

  rtems_filesystem_eval_path_continue(ctx);

  return &ctx->currentloc;
}

rtems_filesystem_location_info_t *
rtems_filesystem_eval_path_start_with_root_and_current(
  rtems_filesystem_eval_path_context_t *ctx,
  const char *path,
  int eval_flags,
  rtems_filesystem_global_location_t *const *global_root_ptr,
  rtems_filesystem_global_location_t *const *global_current_ptr
)
{
  return eval_path_start(
    ctx,
    path,
    strlen(path),
    eval_flags,
    global_root_ptr,
    global_current_ptr
  );
}

rtems_filesystem_location_info_t *
rtems_filesystem_eval_path_start(
  rtems_filesystem_eval_path_context_t *ctx,
  const char *path,
  int eval_flags
)
{
  return rtems_filesystem_eval_path_start_with_root_and_current(
    ctx,
    path,
    eval_flags,
    &rtems_filesystem_root,
    &rtems_filesystem_current
  );
}

rtems_filesystem_location_info_t *
rtems_filesystem_eval_path_start_with_parent(
  rtems_filesystem_eval_path_context_t *ctx,
  const char *path,
  int eval_flags,
  rtems_filesystem_location_info_t *parentloc,
  int parent_eval_flags
)
{
  size_t pathlen = strlen(path);
  const char *parentpath = path;
  size_t parentpathlen = get_parentpathlen(path, pathlen);
  const char *name = NULL;
  size_t namelen = 0;
  const rtems_filesystem_location_info_t *currentloc = NULL;

  if (pathlen > 0) {
    if (parentpathlen == 0) {
      parentpath = ".";
      parentpathlen = 1;
      name = path;
      namelen = pathlen;
    } else {
      name = path + parentpathlen;
      namelen = pathlen - parentpathlen;
    }
  }

  currentloc = eval_path_start(
    ctx,
    parentpath,
    parentpathlen,
    parent_eval_flags,
    &rtems_filesystem_root,
    &rtems_filesystem_current
  );

  rtems_filesystem_location_clone(parentloc, currentloc);

  ctx->path = name;
  ctx->pathlen = namelen;
  ctx->flags = eval_flags;

  rtems_filesystem_eval_path_continue(ctx);

  return &ctx->currentloc;
}

void rtems_filesystem_eval_path_recursive(
  rtems_filesystem_eval_path_context_t *ctx,
  const char *path,
  size_t pathlen
)
{
  if (pathlen > 0) {
    if (ctx->recursionlevel < RTEMS_FILESYSTEM_SYMLOOP_MAX) {
      const char *saved_path = ctx->path;
      size_t saved_pathlen = ctx->pathlen;

      if (rtems_filesystem_is_delimiter(path [0])) {
        rtems_filesystem_eval_path_restart(ctx, &ctx->rootloc);
      }

      ctx->path = path;
      ctx->pathlen = pathlen;

      ++ctx->recursionlevel;
      while (ctx->pathlen > 0) {
        (*ctx->currentloc.mt_entry->ops->eval_path_h)(ctx);
      }
      --ctx->recursionlevel;

      ctx->path = saved_path;
      ctx->pathlen = saved_pathlen;
    } else {
      rtems_filesystem_eval_path_error(ctx, ELOOP);
    }
  } else {
    rtems_filesystem_eval_path_error(ctx, ENOENT);
  }
}

void rtems_filesystem_eval_path_error(
  rtems_filesystem_eval_path_context_t *ctx,
  int eno
)
{
  ctx->path = NULL;
  ctx->pathlen = 0;
  ctx->token = NULL;
  ctx->tokenlen = 0;

  if (!rtems_filesystem_location_is_null(&ctx->currentloc)) {
    if (eno != 0) {
      errno = eno;
    }

    rtems_filesystem_location_detach(&ctx->currentloc);
  }
}

static void free_location(rtems_filesystem_location_info_t *loc)
{
  rtems_filesystem_mt_entry_declare_lock_context(lock_context);

  (*loc->mt_entry->ops->freenod_h)(loc);

  rtems_filesystem_mt_entry_lock(lock_context);
  rtems_chain_extract_unprotected(&loc->mt_entry_node);
  rtems_filesystem_mt_entry_unlock(lock_context);
}

void rtems_filesystem_eval_path_cleanup(
  rtems_filesystem_eval_path_context_t *ctx
)
{
  free_location(&ctx->currentloc);
  rtems_filesystem_instance_unlock(&ctx->startloc->location);
  rtems_filesystem_global_location_release(ctx->startloc);
  rtems_filesystem_global_location_release(ctx->rootloc);
}

void rtems_filesystem_eval_path_cleanup_with_parent(
  rtems_filesystem_eval_path_context_t *ctx,
  rtems_filesystem_location_info_t *parentloc
)
{
  free_location(parentloc);
  rtems_filesystem_eval_path_cleanup(ctx);
}

void rtems_filesystem_eval_path_restart(
  rtems_filesystem_eval_path_context_t *ctx,
  rtems_filesystem_global_location_t **newstartloc_ptr
)
{
  free_location(&ctx->currentloc);
  rtems_filesystem_instance_unlock(&ctx->startloc->location);
  rtems_filesystem_global_location_assign(
    &ctx->startloc,
    rtems_filesystem_global_location_obtain(newstartloc_ptr)
  );
  rtems_filesystem_instance_lock(&ctx->startloc->location);
  rtems_filesystem_location_clone(&ctx->currentloc, &ctx->startloc->location);
}
