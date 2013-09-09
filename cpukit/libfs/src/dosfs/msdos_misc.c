/**
 * @file
 *
 * @brief Miscellaneous Routines Implementation for MSDOS FileSystem
 * @ingroup libfs
 */

/*
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  Modifications to support UTF-8 in the file system are
 *  Copyright (c) 2013 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define MSDOS_TRACE 1

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"


#include <stdio.h>

#define MSDOS_LFN_ENTRY_SIZE \
  (MSDOS_LFN_LEN_PER_ENTRY * MSDOS_NAME_LFN_BYTES_PER_CHAR)

#define MSDOS_LFN_ENTRY_SIZE_UTF8 \
  ((MSDOS_LFN_LEN_PER_ENTRY + 1 ) * MSDOS_NAME_LFN_BYTES_PER_CHAR \
    * MSDOS_NAME_MAX_UTF8_BYTES_PER_CHAR)

/*
 * External strings. Saves space this way.
 */
const char *const MSDOS_DOT_NAME    = ".          ";
const char *const MSDOS_DOTDOT_NAME = "..         ";

/* msdos_is_valid_name_char --
 *     Routine to check the character in a file or directory name.
 *     The characters support in the short file name are letters,
 *     digits, or characters with code points values greater than
 *     127 (not sure what this last is) plus the following special
 *     characters "$%'-_@~`!(){}^#&". The must be uppercase.
 *
 *     The following 6 characters are allowed in a long names,
 *     " +,;=[]" including a space and lower case letters.
 *
 * PARAMETERS:
 *     ch        - character to check.
 *
 * RETURNS:
 *     MSDOS_NAME_INVALID - Not valid in a long or short name.
 *     MSDOS_NAME_SHORT   - Valid in a short name or long name.
 *     MSDOS_NAME_LONG    - Valid in a long name only.
 *
 */
static msdos_name_type_t
msdos_is_valid_name_char(const char ch)
{
    if (strchr(" +,;=[]", ch) != NULL)
        return MSDOS_NAME_LONG;

    if ((ch == '.') || isalnum((unsigned char)ch) ||
        (strchr("$%'-_@~`!(){}^#&", ch) != NULL) || (unsigned char) ch > 127)
        return MSDOS_NAME_SHORT;

    return MSDOS_NAME_INVALID;
}

/* msdos_short_hex_number --
 *     Routine to set the hex number in the SFN.
 *
 * PARAMETERS:
 *     name      - name to change
 *     num       - number to set
 *
 * RETURNS:
 *     nothing
 *
 */
static void
msdos_short_name_hex(char* sfn, int num)
{
    static const char* hex = "0123456789ABCDEF";
    char* c = MSDOS_DIR_NAME(sfn);
    int   i;
    for (i = 0; i < 2; i++, c++)
      if ((*c == ' ') || (*c == '.'))
        *c = '_';
    for (i = 0; i < 4; i++, c++)
      *c = hex[(num >> ((3 - i) * 4)) & 0xf];
    *c++ = '~';
    *c++ = '1';
}

/* msdos_name_type --
 *     Routine the type of file name.
 *
 * PARAMETERS:
 *     name      - name to check
 *
 * RETURNS:
 *     true the name is long, else the name is short.
 *
 */
#define MSDOS_NAME_TYPE_PRINT 0
static msdos_name_type_t
msdos_name_type(const char *name, int name_len)
{
    bool lowercase = false;
    bool uppercase = false;
    int  dot_at = -1;
    int  count = 0;

    while (*name && (count < name_len))
    {
        bool is_dot = *name == '.';
        msdos_name_type_t type = msdos_is_valid_name_char(*name);

#if MSDOS_NAME_TYPE_PRINT
        printf ("MSDOS_NAME_TYPE: c:%02x type:%d\n", *name, type);
#endif

        if ((type == MSDOS_NAME_INVALID) || (type == MSDOS_NAME_LONG))
            return type;

        if (dot_at >= 0)
        {
            if (is_dot || ((count - dot_at) > 3))
            {
#if MSDOS_NAME_TYPE_PRINT
                printf ("MSDOS_NAME_TYPE: LONG[1]: is_dot:%d, at:%d cnt\n",
                        is_dot, dot_at, count);
#endif
                return MSDOS_NAME_LONG;
            }
        }
        else
        {
            if (count == 8 && !is_dot)
            {
#if MSDOS_NAME_TYPE_PRINT
                printf ("MSDOS_NAME_TYPE: LONG[2]: is_dot:%d, at:%d cnt\n",
                        is_dot, dot_at, count);
#endif
                return MSDOS_NAME_LONG;
            }
        }

        if (is_dot)
            dot_at = count;
        else if ((*name >= 'A') && (*name <= 'Z'))
            uppercase = true;
        else if ((*name >= 'a') && (*name <= 'z'))
            lowercase = true;

        count++;
        name++;
    }

    if (lowercase && uppercase)
    {
#if MSDOS_NAME_TYPE_PRINT
        printf ("MSDOS_NAME_TYPE: LONG[3]\n");
#endif
        return MSDOS_NAME_LONG;
    }

#if MSDOS_NAME_TYPE_PRINT
    printf ("MSDOS_NAME_TYPE: SHORT[1]\n");
#endif
    return MSDOS_NAME_SHORT;
}

/* msdos_long_to_short --
 *     Routine to creates a short name from a long. Start the end of the
 *
 * PARAMETERS:
 *     name      - name to check
 *
 * RETURNS:
 *     true the name is long, else the name is short.
 *
 */
#define MSDOS_L2S_PRINT 0
msdos_name_type_t
msdos_long_to_short(rtems_dosfs_convert_control     *converter,
                    const char                      *lfn,
                    int                              lfn_len,
                    char                            *sfn,
                    int                              sfn_len)
{
    msdos_name_type_t type;
    int               eno = 0;
    int               i;
    ssize_t           short_filename_length = sfn_len;
    void             *buffer = converter->buffer.data;
    size_t            codepage_name_len = converter->buffer.size;

    /*
     * Fill with spaces. This is how a short directory entry is padded.
     */
    memset (sfn, ' ', sfn_len);

    /*
     * Handle '.' and '..' specially.
     */
    if ((lfn[0] == '.') && (lfn_len == 1))
    {
        sfn[0] = '.';
#if MSDOS_L2S_PRINT
        printf ("MSDOS_L2S: SHORT[1]: lfn:'%s' SFN:'%s'\n", lfn, sfn);
#endif
        return MSDOS_NAME_SHORT;
    }

    if ((lfn[0] == '.') && (lfn[1] == '.') && (lfn_len == 2))
    {
        sfn[0] = sfn[1] = '.';
#if MSDOS_L2S_PRINT
        printf ("MSDOS_L2S: SHORT[2]: lfn:'%s' SFN:'%s'\n", lfn, sfn);
#endif
        return MSDOS_NAME_SHORT;
    }

    /*
     * Filenames with only blanks and dots are not allowed!
     */
    for (i = 0; i < lfn_len; i++)
        if ((lfn[i] != ' ') && (lfn[i] != '.'))
            break;

    if (i == lfn_len)
    {
#if MSDOS_L2S_PRINT
        printf ("MSDOS_L2S: INVALID[1]: lfn:'%s' SFN:'%s'\n", lfn, sfn);
#endif
        return MSDOS_NAME_INVALID;
    }

    /*
     * Is this a short name ?
     */

    eno = (*converter->handler->utf8_to_codepage) (
        converter,
        (const uint8_t*)&lfn[0],
        lfn_len,
        buffer,
        &codepage_name_len);
    if (eno == EINVAL)
    {
        eno = 0;
        type = MSDOS_NAME_LONG;
    }
    else
    {
        type = msdos_name_type (
            buffer,
            codepage_name_len);
    }

    if (type != MSDOS_NAME_INVALID)
    {
        short_filename_length = msdos_filename_utf8_to_short_name_for_save (
            converter,
            (const uint8_t*)lfn,
            lfn_len,
            sfn,
            short_filename_length);
        if (short_filename_length < 0 ) {
            type = MSDOS_NAME_INVALID;
        }
#if MSDOS_L2S_PRINT
        printf ("MSDOS_L2S: TYPE:%d lfn:'%s' SFN:'%s'\n", type, lfn, sfn);
#endif
    }
    else
    {
#if MSDOS_L2S_PRINT
        printf ("MSDOS_L2S: INVALID[2]: lfn:'%s' SFN:'%s'\n", lfn, sfn);
#endif
    }

    return type;
}

/* msdos_find_name --
 *     Find the node which correspondes to the name, open fat-file which
 *     correspondes to the found node and close fat-file which correspondes
 *     to the node we searched in.
 *
 * PARAMETERS:
 *     parent_loc - parent node description
 *     name       - name to find
 *
 * RETURNS:
 *     RC_OK and updated 'parent_loc' on success, or -1 if error
 *     occured (errno set apropriately)
 *
 */
int
msdos_find_name(
    rtems_filesystem_location_info_t *parent_loc,
    const char                       *name,
    int                               name_len
    )
{
    int                rc = RC_OK;
    msdos_fs_info_t   *fs_info = parent_loc->mt_entry->fs_info;
    fat_file_fd_t     *fat_fd = NULL;
    msdos_name_type_t  name_type;
    fat_dir_pos_t      dir_pos;
    unsigned short     time_val = 0;
    unsigned short     date = 0;
    char               node_entry[MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE];

    memset(node_entry, 0, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);

    name_type = msdos_long_to_short (
        fs_info->converter,
        name,
        name_len,
        MSDOS_DIR_NAME(node_entry),
        MSDOS_NAME_MAX);

    /*
     * find the node which corresponds to the name in the directory pointed by
     * 'parent_loc'
     */
    rc = msdos_get_name_node(parent_loc, false, name, name_len, name_type,
                             &dir_pos, node_entry);
    if (rc != RC_OK)
        return rc;

    if (((*MSDOS_DIR_ATTR(node_entry)) & MSDOS_ATTR_VOLUME_ID) ||
        ((*MSDOS_DIR_ATTR(node_entry) & MSDOS_ATTR_LFN_MASK) == MSDOS_ATTR_LFN))
        return MSDOS_NAME_NOT_FOUND_ERR;

    /* open fat-file corresponded to the found node */
    rc = fat_file_open(&fs_info->fat, &dir_pos, &fat_fd);
    if (rc != RC_OK)
        return rc;

    fat_fd->dir_pos = dir_pos;

    /*
     * I don't like this if, but: we should do it, or should write new file
     * size and first cluster num to the disk after each write operation
     * (even if one byte is written  - that is TOO slow) because
     * otherwise real values of these fields stored in fat-file descriptor
     * may be accidentally rewritten with wrong values stored on the disk
     */
    if (fat_fd->links_num == 1)
    {
        fat_fd->cln = MSDOS_EXTRACT_CLUSTER_NUM(node_entry);

        time_val = *MSDOS_DIR_WRITE_TIME(node_entry);
        date = *MSDOS_DIR_WRITE_DATE(node_entry);

        fat_fd->mtime = msdos_date_dos2unix(CF_LE_W(date), CF_LE_W(time_val));

        if ((*MSDOS_DIR_ATTR(node_entry)) & MSDOS_ATTR_DIRECTORY)
        {
            fat_fd->fat_file_type = FAT_DIRECTORY;
            fat_fd->size_limit = MSDOS_MAX_DIR_LENGHT;

            rc = fat_file_size(&fs_info->fat, fat_fd);
            if (rc != RC_OK)
            {
                fat_file_close(&fs_info->fat, fat_fd);
                return rc;
            }
        }
        else
        {
            fat_fd->fat_file_size = CF_LE_L(*MSDOS_DIR_FILE_SIZE(node_entry));
            fat_fd->fat_file_type = FAT_FILE;
            fat_fd->size_limit = MSDOS_MAX_FILE_SIZE;
        }

        /* these data is not actual for zero-length fat-file */
        fat_fd->map.file_cln = 0;
        fat_fd->map.disk_cln = fat_fd->cln;

        if ((fat_fd->fat_file_size != 0) &&
            (fat_fd->fat_file_size <= fs_info->fat.vol.bpc))
        {
            fat_fd->map.last_cln = fat_fd->cln;
        }
        else
        {
            fat_fd->map.last_cln = FAT_UNDEFINED_VALUE;
        }
    }

    /* close fat-file corresponded to the node we searched in */
    rc = fat_file_close(&fs_info->fat, parent_loc->node_access);
    if (rc != RC_OK)
    {
        fat_file_close(&fs_info->fat, fat_fd);
        return rc;
    }

    /* update node_info_ptr field */
    parent_loc->node_access = fat_fd;

    return rc;
}

/* msdos_get_name_node --
 *     This routine is used in two ways: for a new node creation (a) or for
 *     search the node which correspondes to the name parameter (b).
 *     In case (a) 'name' should be set up to NULL and 'name_dir_entry' should
 *     point to initialized 32 bytes structure described a new node.
 *     In case (b) 'name' should contain a valid string.
 *
 *     (a): reading fat-file which correspondes to directory we are going to
 *          create node in. If free slot is found write contents of
 *          'name_dir_entry' into it. If reach end of fat-file and no free
 *          slot found, write 32 bytes to the end of fat-file.
 *
 *     (b): reading fat-file which correspondes to directory and trying to
 *          find slot with the name field == 'name' parameter
 *
 *
 * PARAMETERS:
 *     parent_loc     - node description to create node in or to find name in
 *     name           - NULL or name to find
 *     paux           - identify a node location on the disk -
 *                      cluster num and offset inside the cluster
 *     short_dir_entry - node to create/placeholder for found node (IN/OUT)
 *
 * RETURNS:
 *     RC_OK, filled aux_struct_ptr and name_dir_entry on success, or -1 if
 *     error occured (errno set apropriately)
 *
 */
int
msdos_get_name_node(
    const rtems_filesystem_location_info_t *parent_loc,
    bool                                    create_node,
    const char                             *name,
    int                                     name_len,
    msdos_name_type_t                       name_type,
    fat_dir_pos_t                          *dir_pos,
    char                                   *name_dir_entry
    )
{
    int              rc = RC_OK;
    fat_file_fd_t   *fat_fd = parent_loc->node_access;
    uint32_t         dotdot_cln = 0;

    /* find name in fat-file which corresponds to the directory */
    rc = msdos_find_name_in_fat_file(parent_loc->mt_entry, fat_fd,
                                     create_node, (const uint8_t*)name, name_len, name_type,
                                     dir_pos, name_dir_entry);
    if ((rc != RC_OK) && (rc != MSDOS_NAME_NOT_FOUND_ERR))
        return rc;

    if (!create_node)
    {
        /* if we search for valid name and name not found -> return */
        if (rc == MSDOS_NAME_NOT_FOUND_ERR)
            return rc;

        /*
         * if we have deal with ".." - it is a special case :(((
         *
         * Really, we should return cluster num and offset not of ".." slot, but
         * slot which correspondes to real directory name.
         */
        if (rc == RC_OK)
        {
            if (strncmp(name, "..", 2) == 0)
            {
                dotdot_cln = MSDOS_EXTRACT_CLUSTER_NUM((name_dir_entry));

                /* are we right under root dir ? */
                if (dotdot_cln == 0)
                {
                    /*
                     * we can relax about first_char field - it never should be
                     * used for root dir
                     */
                    fat_dir_pos_init(dir_pos);
                    dir_pos->sname.cln = FAT_ROOTDIR_CLUSTER_NUM;
                }
                else
                {
                    rc =
                        msdos_get_dotdot_dir_info_cluster_num_and_offset(parent_loc->mt_entry,
                                                                         dotdot_cln,
                                                                         dir_pos,
                                                                         name_dir_entry);
                    if (rc != RC_OK)
                        return rc;
                }
            }
        }
    }
    return rc;
}

/*
 * msdos_get_dotdot_dir_info_cluster_num_and_offset
 *
 * Unfortunately, in general, we cann't work here in fat-file ideologic
 * (open fat_file "..", get ".." and ".", open "..", find an entry ...)
 * because if we open
 * fat-file ".." it may happend that we have two different fat-file
 * descriptors ( for real name of directory and ".." name ) for a single
 * file  ( cluster num of both pointers to the same cluster )
 * But...we do it because we protected by semaphore
 *
 */

/* msdos_get_dotdot_dir_info_cluster_num_and_offset --
 *     Get cluster num and offset not of ".." slot, but slot which correspondes
 *     to real directory name.
 *
 * PARAMETERS:
 *     mt_entry       - mount table entry
 *     cln            - data cluster num extracted drom ".." slot
 *     paux           - identify a node location on the disk -
 *                      number of cluster and offset inside the cluster
 *     dir_entry      - placeholder for found node
 *
 * RETURNS:
 *     RC_OK, filled 'paux' and 'dir_entry' on success, or -1 if error occured
 *     (errno set apropriately)
 *
 */
int
msdos_get_dotdot_dir_info_cluster_num_and_offset(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    uint32_t                              cln,
    fat_dir_pos_t                        *dir_pos,
    char                                 *dir_entry
    )
{
    int              rc = RC_OK;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    fat_file_fd_t   *fat_fd = NULL;
    char             dot_node[MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE];
    char             dotdot_node[MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE];
    uint32_t         cl4find = 0;
    rtems_dosfs_convert_control *converter = fs_info->converter;

    /*
     * open fat-file corresponded to ".."
     */
    rc = fat_file_open(&fs_info->fat, dir_pos, &fat_fd);
    if (rc != RC_OK)
        return rc;

    fat_fd->cln = cln;
    fat_fd->fat_file_type = FAT_DIRECTORY;
    fat_fd->size_limit = MSDOS_MAX_DIR_LENGHT;

    fat_fd->map.file_cln = 0;
    fat_fd->map.disk_cln = fat_fd->cln;

    rc = fat_file_size(&fs_info->fat, fat_fd);
    if (rc != RC_OK)
    {
        fat_file_close(&fs_info->fat, fat_fd);
        return rc;
    }

    /* find "." node in opened directory */
    memset(dot_node, 0, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
    msdos_long_to_short(
        converter,
        ".", 1, dot_node, MSDOS_SHORT_NAME_LEN);
    rc = msdos_find_name_in_fat_file(mt_entry, fat_fd, false, (const uint8_t*)".", 1,
                                     MSDOS_NAME_SHORT, dir_pos, dot_node);

    if (rc != RC_OK)
    {
        fat_file_close(&fs_info->fat, fat_fd);
        return rc;
    }

    /* find ".." node in opened directory */
    memset(dotdot_node, 0, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
    msdos_long_to_short(
        converter,
        "..", 2, dotdot_node, MSDOS_SHORT_NAME_LEN);
    rc = msdos_find_name_in_fat_file(mt_entry, fat_fd, false, (const uint8_t*)"..", 2,
                                     MSDOS_NAME_SHORT, dir_pos,
                                     dotdot_node);

    if (rc != RC_OK)
    {
        fat_file_close(&fs_info->fat, fat_fd);
        return rc;
    }

    cl4find = MSDOS_EXTRACT_CLUSTER_NUM(dot_node);

    /* close fat-file corresponded to ".." directory */
    rc = fat_file_close(&fs_info->fat, fat_fd);
    if ( rc != RC_OK )
        return rc;

    if ( (MSDOS_EXTRACT_CLUSTER_NUM(dotdot_node)) == 0)
    {
        /*
         * we handle root dir for all FAT types in the same way with the
         * ordinary directories ( through fat_file_* calls )
         */
        fat_dir_pos_init(dir_pos);
        dir_pos->sname.cln = FAT_ROOTDIR_CLUSTER_NUM;
    }

    /* open fat-file corresponded to second ".." */
    rc = fat_file_open(&fs_info->fat, dir_pos, &fat_fd);
    if (rc != RC_OK)
        return rc;

    if ((MSDOS_EXTRACT_CLUSTER_NUM(dotdot_node)) == 0)
        fat_fd->cln = fs_info->fat.vol.rdir_cl;
    else
        fat_fd->cln = MSDOS_EXTRACT_CLUSTER_NUM(dotdot_node);

    fat_fd->fat_file_type = FAT_DIRECTORY;
    fat_fd->size_limit = MSDOS_MAX_DIR_LENGHT;

    fat_fd->map.file_cln = 0;
    fat_fd->map.disk_cln = fat_fd->cln;

    rc = fat_file_size(&fs_info->fat, fat_fd);
    if (rc != RC_OK)
    {
        fat_file_close(&fs_info->fat, fat_fd);
        return rc;
    }

    /* in this directory find slot with specified cluster num */
    rc = msdos_find_node_by_cluster_num_in_fat_file(mt_entry, fat_fd, cl4find,
                                                    dir_pos, dir_entry);
    if (rc != RC_OK)
    {
        fat_file_close(&fs_info->fat, fat_fd);
        return rc;
    }
    rc = fat_file_close(&fs_info->fat, fat_fd);
    return rc;
}


/* msdos_set_dir_wrt_time_and_date --
 *     Write last write date and time for a file to the disk (to corresponded
 *     32bytes node)
 *
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     fat_fd   - fat-file descriptor
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set apropriately).
 *
 */
int
msdos_set_dir_wrt_time_and_date(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd
    )
{
    ssize_t          ret1 = 0, ret2 = 0, ret3 = 0;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    uint16_t         time_val;
    uint16_t         date;
    uint32_t         sec = 0;
    uint32_t         byte = 0;

    msdos_date_unix2dos(fat_fd->mtime, &date, &time_val);

    /*
     * calculate input for fat_sector_write: convert (cluster num, offset) to
     * (sector num, new offset)
     */
    sec = fat_cluster_num_to_sector_num(&fs_info->fat, fat_fd->dir_pos.sname.cln);
    sec += (fat_fd->dir_pos.sname.ofs >> fs_info->fat.vol.sec_log2);
    /* byte points to start of 32bytes structure */
    byte = fat_fd->dir_pos.sname.ofs & (fs_info->fat.vol.bps - 1);

    time_val = CT_LE_W(time_val);
    ret1 = fat_sector_write(&fs_info->fat, sec, byte + MSDOS_FILE_WTIME_OFFSET,
                            2, (char *)(&time_val));
    date = CT_LE_W(date);
    ret2 = fat_sector_write(&fs_info->fat, sec, byte + MSDOS_FILE_WDATE_OFFSET,
                            2, (char *)(&date));
    ret3 = fat_sector_write(&fs_info->fat, sec, byte + MSDOS_FILE_ADATE_OFFSET,
                            2, (char *)(&date));

    if ( (ret1 < 0) || (ret2 < 0) || (ret3 < 0) )
        return -1;

    return RC_OK;
}

/* msdos_set_first_cluster_num --
 *     Write number of first cluster of the file to the disk (to corresponded
 *     32bytes slot)
 *
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     fat_fd   - fat-file descriptor
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured
 *
 */
int
msdos_set_first_cluster_num(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd
    )
{
    ssize_t          ret1 = 0, ret2 = 0;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    uint32_t         new_cln = fat_fd->cln;
    uint16_t         le_cl_low = 0;
    uint16_t         le_cl_hi = 0;
    uint32_t         sec = 0;
    uint32_t         byte = 0;

    /*
     * calculate input for fat_sector_write: convert (cluster num, offset) to
     * (sector num, new offset)
     */
    sec = fat_cluster_num_to_sector_num(&fs_info->fat, fat_fd->dir_pos.sname.cln);
    sec += (fat_fd->dir_pos.sname.ofs >> fs_info->fat.vol.sec_log2);
    /* byte from points to start of 32bytes structure */
    byte = fat_fd->dir_pos.sname.ofs & (fs_info->fat.vol.bps - 1);

    le_cl_low = CT_LE_W((uint16_t  )(new_cln & 0x0000FFFF));
    ret1 = fat_sector_write(&fs_info->fat, sec,
                            byte + MSDOS_FIRST_CLUSTER_LOW_OFFSET, 2,
                            (char *)(&le_cl_low));
    le_cl_hi = CT_LE_W((uint16_t  )((new_cln & 0xFFFF0000) >> 16));
    ret2 = fat_sector_write(&fs_info->fat, sec,
                            byte + MSDOS_FIRST_CLUSTER_HI_OFFSET, 2,
                            (char *)(&le_cl_hi));
    if ( (ret1 < 0) || (ret2 < 0) )
        return -1;

    return RC_OK;
}


/* msdos_set_file size --
 *     Write file size of the file to the disk (to corresponded 32bytes slot)
 *
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     fat_fd   - fat-file descriptor
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set apropriately).
 *
 */
int
msdos_set_file_size(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd
    )
{
    ssize_t          ret = 0;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    uint32_t         le_new_length = 0;
    uint32_t         sec = 0;
    uint32_t         byte = 0;

    sec = fat_cluster_num_to_sector_num(&fs_info->fat, fat_fd->dir_pos.sname.cln);
    sec += (fat_fd->dir_pos.sname.ofs >> fs_info->fat.vol.sec_log2);
    byte = (fat_fd->dir_pos.sname.ofs & (fs_info->fat.vol.bps - 1));

    le_new_length = CT_LE_L((fat_fd->fat_file_size));
    ret = fat_sector_write(&fs_info->fat, sec, byte + MSDOS_FILE_SIZE_OFFSET, 4,
                           (char *)(&le_new_length));
    if ( ret < 0 )
        return -1;

    return RC_OK;
}

/*
 * We should not check whether this routine is called for root dir - it
 * never can happend
 */

/* msdos_set_first_char4file_name --
 *     Write first character of the name of the file to the disk (to
 *     corresponded 32bytes slot)
 *
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     cl       - number of cluster
 *     ofs      - offset inside cluster
 *     fchar    - character to set up
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set apropriately)
 *
 */
int
msdos_set_first_char4file_name(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_dir_pos_t                        *dir_pos,
    unsigned char                         fchar
    )
{
    ssize_t          ret;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    uint32_t         dir_block_size;
    fat_pos_t        start = dir_pos->lname;
    fat_pos_t        end = dir_pos->sname;

    if ((end.cln == fs_info->fat.vol.rdir_cl) &&
        (fs_info->fat.vol.type & (FAT_FAT12 | FAT_FAT16)))
      dir_block_size = fs_info->fat.vol.rdir_size;
    else
      dir_block_size = fs_info->fat.vol.bpc;

    if (dir_pos->lname.cln == FAT_FILE_SHORT_NAME)
      start = dir_pos->sname;

    /*
     * We handle the changes directly due the way the short file
     * name code was written rather than use the fat_file_write
     * interface.
     */
    while (true)
    {
      uint32_t sec = (fat_cluster_num_to_sector_num(&fs_info->fat, start.cln) +
                      (start.ofs >> fs_info->fat.vol.sec_log2));
      uint32_t byte = (start.ofs & (fs_info->fat.vol.bps - 1));

      ret = fat_sector_write(&fs_info->fat, sec, byte + MSDOS_FILE_NAME_OFFSET,
                             1, &fchar);
      if (ret < 0)
        return -1;

      if ((start.cln == end.cln) && (start.ofs == end.ofs))
        break;

      start.ofs += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE;
      if (start.ofs >= dir_block_size)
      {
        int rc;
        if ((end.cln == fs_info->fat.vol.rdir_cl) &&
            (fs_info->fat.vol.type & (FAT_FAT12 | FAT_FAT16)))
          break;
        rc = fat_get_fat_cluster(&fs_info->fat, start.cln, &start.cln);
        if ( rc != RC_OK )
          return rc;
        start.ofs = 0;
      }
    }

    return  RC_OK;
}

/* msdos_dir_is_empty --
 *     Check whether directory which correspondes to the fat-file descriptor is
 *     empty.
 *
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     fat_fd   - fat-file descriptor
 *     ret_val  - placeholder for result
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured
 *
 */
int
msdos_dir_is_empty(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd,
    bool                                 *ret_val
    )
{
    ssize_t          ret = 0;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    uint32_t         j = 0, i = 0;

    /* dir is not empty */
    *ret_val = false;

    while ((ret = fat_file_read(&fs_info->fat, fat_fd, j * fs_info->fat.vol.bps,
                                  fs_info->fat.vol.bps,
                                  fs_info->cl_buf)) != FAT_EOF)
    {
        if (ret < MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
            return -1;

        assert(ret == fs_info->fat.vol.bps);

        /* have to look at the DIR_NAME as "raw" 8-bit data */
        for (i = 0;
             i < fs_info->fat.vol.bps;
             i += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            char* entry = (char*) fs_info->cl_buf + i;

            /*
             * If the entry is empty, a long file name entry, or '.' or '..'
             * then consider it as empty.
             *
             * Just ignore long file name entries. They must have a short entry to
             * be valid.
             */
            if (((*MSDOS_DIR_ENTRY_TYPE(entry)) ==
                 MSDOS_THIS_DIR_ENTRY_EMPTY) ||
                ((*MSDOS_DIR_ATTR(entry) & MSDOS_ATTR_LFN_MASK) ==
                 MSDOS_ATTR_LFN) ||
                (strncmp(MSDOS_DIR_NAME((entry)), MSDOS_DOT_NAME,
                         MSDOS_SHORT_NAME_LEN) == 0) ||
                (strncmp(MSDOS_DIR_NAME((entry)),
                         MSDOS_DOTDOT_NAME,
                         MSDOS_SHORT_NAME_LEN) == 0))
                continue;

            /*
             * Nothing more to look at.
             */
            if ((*MSDOS_DIR_NAME(entry)) ==
                MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY)
            {
                *ret_val = true;
                return RC_OK;
            }

            /*
             * Short file name entries mean not empty.
             */
            return RC_OK;
        }
        j++;
    }
    *ret_val = true;
    return RC_OK;
}

#define MSDOS_FIND_PRINT 0
static int
msdos_on_entry_found (
    msdos_fs_info_t                      *fs_info,
    fat_file_fd_t                        *fat_fd,
    const uint32_t                        bts2rd,
    char                                 *name_dir_entry,
    char                                 *entry,
    fat_dir_pos_t                        *dir_pos,
    uint32_t                             *dir_offset,
    const uint32_t                        dir_entry,
    const fat_pos_t                      *lfn_start
)
{
    int rc = RC_OK;
#if MSDOS_FIND_PRINT
    printf ("MSFS:[9.3] SNF found\n");
#endif
    /*
     * We get the entry we looked for - fill the position
     * structure and the 32 bytes of the short entry
     */
    rc = fat_file_ioctl(&fs_info->fat,
                        fat_fd,
                        F_CLU_NUM,
                        *dir_offset * bts2rd,
                        &dir_pos->sname.cln);
    if (rc == RC_OK) {
        dir_pos->sname.ofs = dir_entry;

        if (lfn_start->cln != FAT_FILE_SHORT_NAME)
        {
            rc = fat_file_ioctl (&fs_info->fat,
                                 fat_fd,
                                 F_CLU_NUM,
                                 lfn_start->cln * bts2rd,
                                 &lfn_start->cln);
        }
        if ( rc == RC_OK ) {
            dir_pos->lname.cln = lfn_start->cln;
            dir_pos->lname.ofs = lfn_start->ofs;

            memcpy(name_dir_entry, entry,
                   MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
        }
    }

    return rc;
}

ssize_t
msdos_get_utf16_string_from_long_entry (
    const char                 *entry,
    uint16_t                   *entry_string_buf,
    const size_t                buf_size,
    bool                        is_first_entry
)
{
    ssize_t chars_in_entry;

    if (buf_size >= MSDOS_LFN_ENTRY_SIZE) {
        memcpy (&entry_string_buf[0],  &entry[1],  10 );
        memcpy (&entry_string_buf[5],  &entry[14], 12 );
        memcpy (&entry_string_buf[11], &entry[28],  4 );

        if (is_first_entry) {
            for (chars_in_entry = 0;
                 (    entry_string_buf[chars_in_entry] != 0x0000
                  && chars_in_entry < MSDOS_LFN_LEN_PER_ENTRY );
                  ++chars_in_entry) {
                ;
            }
        }
        else
            chars_in_entry = MSDOS_LFN_LEN_PER_ENTRY;

        return chars_in_entry * MSDOS_NAME_LFN_BYTES_PER_CHAR;
    }
    else
        return ENOMEM;
}

/*  msdos_format_dirent_with_dot --
 *      This routine convert a (short) MSDOS filename as present on disk
 *      (fixed 8+3 characters, filled with blanks, without separator dot)
 *      to a "normal" format, with between 0 and 8 name chars,
 *      a separating dot and up to 3 extension characters
 *   Rules to work:
 *      - copy any (0-8) "name" part characters that are non-blank
 *      - if an extension exists, append a dot
 *      - copy any (0-3) non-blank extension characters
 *      - append a '\0' (dont count it for the rturn code
 *
 * PARAMETERS:
 *     dst: pointer to destination char array (must be big enough)
 *     src: pointer to source characters
 *
 *
 * RETURNS:
 *     the number of bytes (without trailing '\0'(written to destination
 */
ssize_t
msdos_format_dirent_with_dot(char *dst,const char *src)
{
  ssize_t len;
  int i;
  const char *src_tmp;

  /*
   * find last non-blank character of base name
   */
  for (i = MSDOS_SHORT_BASE_LEN, src_tmp = src + MSDOS_SHORT_BASE_LEN - 1;
       i > 0 && *src_tmp == ' ';
       --i,--src_tmp)
    {};
  /*
   * copy base name to destination
   */
  src_tmp = src;
  len = i;
  while (i-- > 0) {
    *dst++ = tolower((unsigned char)(*src_tmp++));
  }
  /*
   * find last non-blank character of extension
   */
  for (i = MSDOS_SHORT_EXT_LEN,
        src_tmp = src + MSDOS_SHORT_BASE_LEN+MSDOS_SHORT_EXT_LEN-1;
       i > 0 && *src_tmp == ' ';
       --i, --src_tmp)
    {};
  /*
   * extension is not empty
   */
  if (i > 0) {
    *dst++ = '.'; /* append dot */
    ++len;        /* dot */
    src_tmp = src + MSDOS_SHORT_BASE_LEN;
    while (i-- > 0) {
      *dst++ = tolower((unsigned char)(*src_tmp++));
      ++len;
    }
  }
  *dst = '\0'; /* terminate string */

  return len;
}

static ssize_t
msdos_long_entry_to_utf8_name (
    rtems_dosfs_convert_control *converter,
    const char                  *entry,
    const bool                   is_first_entry,
    uint8_t                     *entry_utf8_buf,
    const size_t                 buf_size)
{
    ssize_t      retval         = 0;
    int          eno            = 0;
    size_t       bytes_in_utf8  = buf_size;
    size_t       bytes_in_buf;
    uint16_t     entry_string[MSDOS_LFN_LEN_PER_ENTRY];

    retval = msdos_get_utf16_string_from_long_entry (
        entry,
        entry_string,
        sizeof (entry_string),
        is_first_entry
    );

    if (retval >= 0) {
        bytes_in_buf = retval;
        eno = (*converter->handler->utf16_to_utf8) (
            converter,
            &entry_string[0],
            bytes_in_buf,
            &entry_utf8_buf[0],
            &bytes_in_utf8);
        if ( eno == 0 ) {
            retval = bytes_in_utf8;
        }
    }

    if (eno != 0) {
        retval = -1;
        errno  = eno;
    }

    return retval;
}

static ssize_t msdos_short_entry_to_utf8_name (
  rtems_dosfs_convert_control     *converter,
  const char                      *entry,
  uint8_t                         *buf,
  const size_t                     buf_size)
{
  char         char_buf[MSDOS_NAME_MAX_WITH_DOT];
  int          eno             = 0;
  size_t       bytes_converted = buf_size;
  ssize_t      bytes_written   = msdos_format_dirent_with_dot(char_buf, entry);

  if (bytes_written > 0) {
    if (char_buf[0] == 0x05)
      char_buf[0] = 0xE5;

    eno = (*converter->handler->codepage_to_utf8) (
        converter,
        char_buf,
        bytes_written,
        buf,
        &bytes_converted);
    if (eno == 0)
        bytes_written = bytes_converted;
  } else {
      eno = EINVAL;
  }

  if (eno != 0) {
      bytes_written = -1;
      errno         = eno;
  }

  return bytes_written;
}

static ssize_t
msdos_compare_entry_against_filename (
    rtems_dosfs_convert_control *converter,
    const uint8_t               *entry,
    const size_t                 entry_size,
    const uint8_t               *filename,
    const size_t                 filename_size_remaining,
    bool                        *is_matching)
{
  ssize_t      size_remaining = filename_size_remaining;
  int          eno            = 0;
  uint8_t      entry_normalized[MSDOS_LFN_ENTRY_SIZE_UTF8];
  size_t       bytes_in_entry_normalized = sizeof ( entry_normalized );

  eno = (*converter->handler->utf8_normalize_and_fold) (
      converter,
      &entry[0],
      entry_size,
      &entry_normalized[0],
      &bytes_in_entry_normalized);
  if (eno == 0) {
#if MSDOS_FIND_PRINT > 1
        printf ( "MSFS:[6] entry_normalized:%s"
                 "name:%s\n",
                 entry,
                 filename );
#endif
        if (bytes_in_entry_normalized <= size_remaining) {
            size_remaining = size_remaining - bytes_in_entry_normalized;
            if (0 == memcmp ( &entry_normalized[0],
                              &filename[size_remaining],
                              bytes_in_entry_normalized)) {
                *is_matching = true;
            } else {
                *is_matching   = false;
                size_remaining = filename_size_remaining;
            }

        }
        else {
          *is_matching = false;
        }
    }

    if (eno != 0) {
      size_remaining = -1;
      errno          = eno;
    }

    return size_remaining;
}

static int
msdos_find_file_in_directory (
    const uint8_t                        *filename_converted,
    const size_t                          name_len_for_compare,
    const size_t                          name_len_for_save,
    const msdos_name_type_t               name_type,
    msdos_fs_info_t                      *fs_info,
    fat_file_fd_t                        *fat_fd,
    const uint32_t                        bts2rd,
    const bool                            create_node,
    const unsigned int                    fat_entries,
    char                                 *name_dir_entry,
    fat_dir_pos_t                        *dir_pos,
    uint32_t                             *dir_offset,
    uint32_t                             *empty_space_offset,
    uint32_t                             *empty_space_entry,
    uint32_t                             *empty_space_count)
{
    int               rc                = RC_OK;
    ssize_t           bytes_read;
    uint32_t          dir_entry;
    fat_pos_t         lfn_start;
    uint8_t           lfn_checksum      = 0;
    bool              entry_matched       = false;
    bool              empty_space_found = false;
    uint32_t          entries_per_block = bts2rd / MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE;
    int               lfn_entry         = 0;
    uint8_t           entry_utf8_normalized[MSDOS_LFN_ENTRY_SIZE_UTF8];
    size_t            bytes_in_entry;
    bool              filename_matched  = false;
    ssize_t           filename_size_remaining = name_len_for_compare;
    rtems_dosfs_convert_control *converter = fs_info->converter;

    /*
     * Scan the directory seeing if the file is present. While
     * doing this see if a suitable location can be found to
     * create the entry if the name is not found.
     */

    lfn_start.cln = lfn_start.ofs = FAT_FILE_SHORT_NAME;

    while (   (bytes_read = fat_file_read (&fs_info->fat, fat_fd, (*dir_offset * bts2rd),
                                             bts2rd, fs_info->cl_buf)) != FAT_EOF
           && rc == RC_OK)
    {
        bool remainder_empty = false;
#if MSDOS_FIND_PRINT
        printf ("MSFS:[2] dir_offset:%li\n", *dir_offset);
#endif

        if (bytes_read < MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
            rtems_set_errno_and_return_minus_one(EIO);

        assert(bytes_read == bts2rd);

        /* have to look at the DIR_NAME as "raw" 8-bit data */
        for (dir_entry = 0;
             dir_entry < bts2rd && rc == RC_OK && (! filename_matched);
             dir_entry += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            char* entry = (char*) fs_info->cl_buf + dir_entry;

            /*
             * See if the entry is empty or the remainder of the directory is
             * empty ? Localize to make the code read better.
             */
            bool entry_empty = (*MSDOS_DIR_ENTRY_TYPE(entry) ==
                                MSDOS_THIS_DIR_ENTRY_EMPTY);
            remainder_empty = (*MSDOS_DIR_ENTRY_TYPE(entry) ==
                               MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY);
#if MSDOS_FIND_PRINT
            printf ("MSFS:[3] re:%i ee:%i do:%li de:%li(%ld)\n",
                    remainder_empty, entry_empty, *dir_offset,
                    dir_entry, (dir_entry / MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE));
#endif
            /*
             * Remember where the we are, ie the start, so we can come back
             * to here and write the long file name if this is the start of
             * a series of empty entries. If empty_space_count is 0 then
             * we are currently not inside an empty series of entries. It
             * is a count of empty entries.
             */
            if (*empty_space_count == 0)
            {
                *empty_space_entry = dir_entry;
                *empty_space_offset = *dir_offset;
            }

            if (remainder_empty)
            {
#if MSDOS_FIND_PRINT
                printf ("MSFS:[3.1] cn:%i esf:%i\n", create_node, empty_space_found);
#endif
                /*
                 * If just looking and there is no more entries in the
                 * directory - return name-not-found
                 */
                if (!create_node)
                    rc = MSDOS_NAME_NOT_FOUND_ERR;

                /*
                 * Lets go and write the directory entries. If we have not found
                 * any available space add the remaining number of entries to any that
                 * we may have already found that are just before this entry. If more
                 * are needed FAT_EOF is returned by the read and we extend the file.
                 */
                if (   !empty_space_found
                    && rc == RC_OK )
                {
                    *empty_space_count +=
                    entries_per_block - (dir_entry / MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
                    empty_space_found = true;
#if MSDOS_FIND_PRINT
                    printf ( "MSFS:[3.2] esf:%i esc%"PRIu32"\n", empty_space_found, *empty_space_count );
#endif
                }
                break;
            }
            else if (entry_empty)
            {
                if (create_node)
                {
                    /*
                     * Remainder is not empty so is this entry empty ?
                     */
                    (*empty_space_count)++;

                    if (*empty_space_count == (fat_entries + 1))
                        empty_space_found = true;
                }
#if MSDOS_FIND_PRINT
                printf ("MSFS:[4.1] esc:%li esf:%i\n",
                        *empty_space_count, empty_space_found);
#endif
            }
            else
            {
                /*
                 * A valid entry so handle it.
                 *
                 * If empty space has not been found we need to start the
                 * count again.
                 */
                if (create_node && !empty_space_found)
                {
                    *empty_space_entry = 0;
                    *empty_space_count = 0;
                }

                /*
                 * Check the attribute to see if the entry is for a long
                 * file name.
                 */
                if ((*MSDOS_DIR_ATTR(entry) & MSDOS_ATTR_LFN_MASK) ==
                    MSDOS_ATTR_LFN)
                {
/*                    int   o;*/
#if MSDOS_FIND_PRINT
                    printf ("MSFS:[4.2] lfn:%c entry:%i checksum:%i\n",
                            lfn_start.cln == FAT_FILE_SHORT_NAME ? 'f' : 't',
                            *MSDOS_DIR_ENTRY_TYPE(entry) & MSDOS_LAST_LONG_ENTRY_MASK,
                            *MSDOS_DIR_LFN_CHECKSUM(entry));
#endif
                    /*
                     * If we are not already processing a LFN see if this is
                     * the first entry of a LFN ?
                     */
                    if (lfn_start.cln == FAT_FILE_SHORT_NAME)
                    {
                        entry_matched = false;

                        /*
                         * The first entry must have the last long entry
                         * flag set.
                         */
                        if ((*MSDOS_DIR_ENTRY_TYPE(entry) &
                             MSDOS_LAST_LONG_ENTRY) == 0)
                            continue;

                        /*
                         * Does the number of entries in the LFN directory
                         * entry match the number we expect for this
                         * file name. Note we do not know the number of
                         * characters in the entry so this is check further
                         * on when the characters are checked.
                         */
                        if (fat_entries != (*MSDOS_DIR_ENTRY_TYPE(entry) &
                                            MSDOS_LAST_LONG_ENTRY_MASK))
                            continue;

                        /*
                         * Get the checksum of the short entry.
                         */
                        lfn_start.cln = *dir_offset;
                        lfn_start.ofs = dir_entry;
                        lfn_entry = fat_entries;
                        lfn_checksum = *MSDOS_DIR_LFN_CHECKSUM(entry);

#if MSDOS_FIND_PRINT
                        printf ("MSFS:[4.3] lfn_checksum:%i\n",
                                lfn_checksum);
#endif
                    }

                    /*
                     * If the entry number or the check sum do not match
                     * forget this series of long directory entries. These
                     * could be orphaned entries depending on the history
                     * of the disk.
                     */
                    if ((lfn_entry != (*MSDOS_DIR_ENTRY_TYPE(entry) &
                                       MSDOS_LAST_LONG_ENTRY_MASK)) ||
                        (lfn_checksum != *MSDOS_DIR_LFN_CHECKSUM(entry)))
                    {
#if MSDOS_FIND_PRINT
                        printf ("MSFS:[4.4] no match\n");
#endif
                        lfn_start.cln = FAT_FILE_SHORT_NAME;
                        continue;
                    }
#if MSDOS_FIND_PRINT
                    printf ("MSFS:[5] lfne:%i\n", lfn_entry);
#endif
                    lfn_entry--;

                    bytes_in_entry = msdos_long_entry_to_utf8_name (
                        converter,
                        entry,
                        (lfn_entry + 1) == fat_entries,
                        &entry_utf8_normalized[0],
                        sizeof (entry_utf8_normalized));
                    if (bytes_in_entry > 0) {
                        filename_size_remaining = msdos_compare_entry_against_filename (
                            converter,
                            &entry_utf8_normalized[0],
                            bytes_in_entry,
                            &filename_converted[0],
                            filename_size_remaining,
                            &entry_matched);

                        if (filename_size_remaining < 0
                            || (! entry_matched)) {
                            filename_size_remaining = name_len_for_compare;
                            lfn_start.cln = FAT_FILE_SHORT_NAME;
                        }
                    } else {
                      lfn_start.cln = FAT_FILE_SHORT_NAME;
                      entry_matched   = false;
                    }
                }
                else
                {
#if MSDOS_FIND_PRINT
                    printf ("MSFS:[9.1] SFN entry, entry_matched:%i\n", entry_matched);
#endif
                    /*
                     * SFN entry found.
                     *
                     * If a LFN has been found and it matched check the
                     * entries have all been found and the checksum is
                     * correct. If this is the case return the short file
                     * name entry.
                     */
                    if (entry_matched)
                    {
                        uint8_t  cs = 0;
                        uint8_t* p = (uint8_t*) MSDOS_DIR_NAME(entry);
                        int      i;

                        for (i = 0; i < MSDOS_SHORT_NAME_LEN; i++, p++)
                            cs = ((cs & 1) ? 0x80 : 0) + (cs >> 1) + *p;

                        if (lfn_entry || (lfn_checksum != cs))
                            entry_matched = false;
                        else {
                            filename_matched = true;
                            rc = msdos_on_entry_found (
                                fs_info,
                                fat_fd,
                                bts2rd,
                                name_dir_entry,
                                entry,
                                dir_pos,
                                dir_offset,
                                dir_entry,
                                &lfn_start
                            );
                        }

#if MSDOS_FIND_PRINT
                        printf ("MSFS:[9.2] checksum, entry_matched:%i, lfn_entry:%i, lfn_checksum:%02x/%02x\n",
                                entry_matched, lfn_entry, lfn_checksum, cs);
#endif
                    } else {
                        bytes_in_entry = MSDOS_SHORT_NAME_LEN + 1;
                        bytes_in_entry = msdos_short_entry_to_utf8_name (
                            converter,
                            MSDOS_DIR_NAME (entry),
                            &entry_utf8_normalized[0],
                            bytes_in_entry);
                        if (bytes_in_entry > 0) {
                            filename_size_remaining = msdos_compare_entry_against_filename (
                                converter,
                                &entry_utf8_normalized[0],
                                bytes_in_entry,
                                &filename_converted[0],
                                name_len_for_compare,
                                &entry_matched);
                            if (entry_matched && filename_size_remaining == 0) {
                                filename_matched = true;
                                rc = msdos_on_entry_found (
                                    fs_info,
                                    fat_fd,
                                    bts2rd,
                                    name_dir_entry,
                                    entry,
                                    dir_pos,
                                    dir_offset,
                                    dir_entry,
                                    &lfn_start
                                );
                            }
                            if (rc == RC_OK && (! filename_matched)) {
                                lfn_start.cln           = FAT_FILE_SHORT_NAME;
                                entry_matched           = false;
                                filename_size_remaining = name_len_for_compare;
                            }
                        } else {
                          lfn_start.cln           = FAT_FILE_SHORT_NAME;
                          entry_matched           = false;
                          filename_size_remaining = name_len_for_compare;
                        }
                    }
                }
            }
        }

        if (filename_matched || remainder_empty)
            break;

        (*dir_offset)++;
    }
    if ( ! filename_matched ) {
        /*
         * If we are not to create the entry return a not found error.
         */
        if (!create_node)
            rc = MSDOS_NAME_NOT_FOUND_ERR;

#if MSDOS_FIND_PRINT
        printf ( "MSFS:[8.1] WRITE do:%"PRIu32" esc:%"PRIu32" eso:%"PRIu32" ese:%"PRIu32"\n",
                 *dir_offset, *empty_space_count, *empty_space_offset, *empty_space_entry );
#endif
    }

    return rc;
}

static int
msdos_add_file (
    const char                           *name_converted,
    const msdos_name_type_t               name_type,
    msdos_fs_info_t                      *fs_info,
    fat_file_fd_t                        *fat_fd,
    const uint32_t                        bts2rd,
    const unsigned int                    fat_entries,
    const char                           *name_dir_entry,
    fat_dir_pos_t                        *dir_pos,
    const uint32_t                        dir_offset,
    const uint32_t                        empty_space_offset_param,
    const uint32_t                        empty_space_entry_param,
    const uint32_t                        empty_space_count

)
{
    int              ret                = 0;
    ssize_t          bytes_written      = 0;
    uint8_t          lfn_checksum       = 0;
    uint32_t         empty_space_offset = empty_space_offset_param;
    uint32_t         empty_space_entry  = empty_space_entry_param;
    bool             read_cluster       = false;
    int              lfn_entry          = 0;
    fat_pos_t        lfn_start;
    uint32_t         dir_entry;

    /*
     * If a long file name calculate the checksum of the short file name
     * data to place in each long file name entry. First set the short
     * file name to the slot of the SFN entry. This will mean no clashes
     * in this directory.
     */
    if (name_type == MSDOS_NAME_LONG)
    {
        int      slot = (((empty_space_offset * bts2rd) + empty_space_entry) /
                        MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE) + fat_entries + 1;
        msdos_short_name_hex(MSDOS_DIR_NAME(name_dir_entry), slot);
    }

    if (fat_entries)
    {
        uint8_t* p = (uint8_t*) MSDOS_DIR_NAME(name_dir_entry);
        int      i;
        for (i = 0; i < 11; i++, p++)
            lfn_checksum =
                ((lfn_checksum & 1) ? 0x80 : 0) + (lfn_checksum >> 1) + *p;
    }

    /*
     * If there is no space available then extend the file. The
     * empty_space_count is a count of empty entries in the currently
     * read cluster so if 0 there is no space. Note, dir_offset will
     * be at the next cluster so we can just make empty_space_offset
     * that value.
     */
    if (empty_space_count == 0)
    {
        read_cluster = true;
        empty_space_offset = dir_offset;
        empty_space_entry = 0;
    }

    /*
     * Have we read past the empty block ? If so go back and read it again.
     */
    if (dir_offset != empty_space_offset)
        read_cluster = true;

    /*
     * Handle the entry writes.
     */
    lfn_start.cln = lfn_start.ofs = FAT_FILE_SHORT_NAME;
    lfn_entry = 0;

#if MSDOS_FIND_PRINT
    printf ("MSFS:[9] read_cluster:%d eso:%ld ese:%ld\n",
            read_cluster, empty_space_offset, empty_space_entry);
#endif

    /*
     * The one more is the short entry.
     */
    while (lfn_entry < (fat_entries + 1))
    {
        int length = 0;

        if (read_cluster)
        {
            uint32_t new_length;
#if MSDOS_FIND_PRINT
            printf ("MSFS:[9.1] eso:%li\n", empty_space_offset);
#endif
            ret = fat_file_read(&fs_info->fat, fat_fd,
                                (empty_space_offset * bts2rd), bts2rd,
                                fs_info->cl_buf);

            if (ret != bts2rd)
            {
                if (ret != FAT_EOF)
                    rtems_set_errno_and_return_minus_one(EIO);

#if MSDOS_FIND_PRINT
                printf ("MSFS:[9.2] extending file:%li\n", empty_space_offset);
#endif
                ret = fat_file_extend (&fs_info->fat, fat_fd, false,
                                       empty_space_offset * bts2rd, &new_length);

                if (ret != RC_OK)
                    return ret;

#if MSDOS_FIND_PRINT
                printf ("MSFS:[9.3] extended: %"PRIu32" <-> %"PRIu32"\n", new_length, empty_space_offset * bts2rd);
#endif
                if (new_length != (empty_space_offset * bts2rd))
                    rtems_set_errno_and_return_minus_one(EIO);

                memset(fs_info->cl_buf, 0, bts2rd);

                bytes_written = fat_file_write(&fs_info->fat, fat_fd,
                                               empty_space_offset * bts2rd,
                                               bts2rd, fs_info->cl_buf);
#if MSDOS_FIND_PRINT
                printf ("MSFS:[9.4] clear write: %d\n", ret);
#endif
                if (bytes_written == -1)
                    return -1;
                else if (bytes_written != bts2rd)
                    rtems_set_errno_and_return_minus_one(EIO);
            }
        }

#if MSDOS_FIND_PRINT
        printf ("MSFS:[10] eso:%li\n", empty_space_offset);
#endif

        for (dir_entry = empty_space_entry;
             dir_entry < bts2rd;
             dir_entry += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            char*       entry = (char*) fs_info->cl_buf + dir_entry;
            char*       p;
            const char* n;
            int         i;
            char        fill = 0;

            length += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE;
            lfn_entry++;

#if MSDOS_FIND_PRINT
            printf ("MSFS:[10] de:%li(%li) length:%i lfn_entry:%i\n",
                    dir_entry, (dir_entry / MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE),
                    length, lfn_entry);
#endif
            /*
             * Time to write the short file name entry.
             */
            if (lfn_entry == (fat_entries + 1))
            {
                /* get current cluster number */
                ret = fat_file_ioctl(&fs_info->fat, fat_fd, F_CLU_NUM,
                                    empty_space_offset * bts2rd,
                                    &dir_pos->sname.cln);
                if (ret != RC_OK)
                    return ret;

                dir_pos->sname.ofs = dir_entry;

                if (lfn_start.cln != FAT_FILE_SHORT_NAME)
                {
                    ret = fat_file_ioctl(&fs_info->fat, fat_fd, F_CLU_NUM,
                                        lfn_start.cln * bts2rd,
                                        &lfn_start.cln);
                    if (ret != RC_OK)
                        return ret;
                }

                dir_pos->lname.cln = lfn_start.cln;
                dir_pos->lname.ofs = lfn_start.ofs;

                /* write new node entry */
                memcpy (entry, (uint8_t *) name_dir_entry,
                        MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
                break;
            }

            /*
             * This is a long file name and we need to write
             * a long file name entry. See if this is the
             * first entry written and if so remember the
             * the location of the long file name.
             */
            if (lfn_start.cln == FAT_FILE_SHORT_NAME)
            {
                lfn_start.cln = empty_space_offset;
                lfn_start.ofs = dir_entry;
            }

            /*
             * Clear the entry before loading the data.
             */
            memset (entry, 0, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);

            *MSDOS_DIR_LFN_CHECKSUM(entry) = lfn_checksum;

            p = entry + 1;
            n = name_converted + (fat_entries - lfn_entry) * MSDOS_LFN_ENTRY_SIZE;

#if MSDOS_FIND_PRINT
            printf ("MSFS:[11] ");
#endif
            for (i = 0; i < MSDOS_LFN_LEN_PER_ENTRY; ++i)
            {
                if (!(*n == 0 && *(n+1) == 0))
                {
                    *p = *n;
                    *(p+1) = *(n+1);
                }
                else
                {
                    p [0] = fill;
                    p [1] = fill;
                    fill = 0xff;
                }
                n += MSDOS_NAME_LFN_BYTES_PER_CHAR;
#if MSDOS_FIND_PRINT
                printf ( "'%c''%c'", *p, *(p+1) );
#endif

                switch (i)
                {
                    case 4:
                        p += 5;
                        break;
                    case 10:
                        p += 4;
                        break;
                    default:
                        p += 2;
                        break;
                }
            }
#if MSDOS_FIND_PRINT
            printf ( "\n" );
#endif
            *MSDOS_DIR_ENTRY_TYPE(entry) = (fat_entries - lfn_entry) + 1;
            if (lfn_entry == 1)
                *MSDOS_DIR_ENTRY_TYPE(entry) |= MSDOS_LAST_LONG_ENTRY;
            *MSDOS_DIR_ATTR(entry) |= MSDOS_ATTR_LFN;
        }

        bytes_written = fat_file_write(&fs_info->fat, fat_fd,
                                       (empty_space_offset * bts2rd) + empty_space_entry,
                                       length, fs_info->cl_buf + empty_space_entry);
        if (bytes_written == -1)
            return -1;
        else if (bytes_written != length)
            rtems_set_errno_and_return_minus_one(EIO);

        empty_space_offset++;
        empty_space_entry = 0;
        read_cluster = true;
    }
    return ret;
}

int
msdos_find_name_in_fat_file (
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd,
    bool                                  create_node,
    const uint8_t                        *name_utf8,
    int                                   name_utf8_len,
    msdos_name_type_t                     name_type,
    fat_dir_pos_t                        *dir_pos,
    char                                 *name_dir_entry)
{
    int                                retval                     = 0;
    msdos_fs_info_t                   *fs_info                    = mt_entry->fs_info;
    ssize_t                            name_len_for_save;
    ssize_t                            name_len_for_compare;
    uint32_t                           bts2rd                     = 0;
    uint32_t                           empty_space_offset         = 0;
    uint32_t                           empty_space_entry          = 0;
    uint32_t                           empty_space_count          = 0;
    uint32_t                           dir_offset                 = 0;
    unsigned int                       fat_entries;
    rtems_dosfs_convert_control       *converter = fs_info->converter;
    void                              *buffer = converter->buffer.data;
    size_t                             buffer_size = converter->buffer.size;

    assert(name_utf8_len > 0);

    fat_dir_pos_init(dir_pos);



    if (FAT_FD_OF_ROOT_DIR(fat_fd) &&
        (fs_info->fat.vol.type & (FAT_FAT12 | FAT_FAT16)))
        bts2rd = fat_fd->fat_file_size;
    else
        bts2rd = fs_info->fat.vol.bpc;

    switch ( name_type ) {
        case MSDOS_NAME_SHORT:
            name_len_for_compare = msdos_filename_utf8_to_short_name_for_compare (
                converter,
                name_utf8,
                name_utf8_len,
                buffer,
                MSDOS_SHORT_NAME_LEN);
            if (name_len_for_compare > 0) {
                fat_entries = 0;
            }
            else
                retval = -1;
        break;
        case MSDOS_NAME_LONG:
            name_len_for_save = msdos_filename_utf8_to_long_name_for_save (
                converter,
                name_utf8,
                name_utf8_len,
                buffer,
                buffer_size);
            if (name_len_for_save > 0) {
                fat_entries = (name_len_for_save + MSDOS_LFN_ENTRY_SIZE - 1)
                    / MSDOS_LFN_ENTRY_SIZE;
                name_len_for_compare = msdos_filename_utf8_to_long_name_for_compare (
                  converter,
                  name_utf8,
                  name_utf8_len,
                  buffer,
                  buffer_size);
                if (0 >= name_len_for_compare) {
                    retval = -1;
                }
            }
            else
                retval = -1;
        break;
        case MSDOS_NAME_INVALID:
        default:
            errno = EINVAL;
            retval = -1;
        break;
    }
    if (retval == RC_OK) {
      /* See if the file/directory does already exist */
      retval = msdos_find_file_in_directory (
          buffer,
          name_len_for_compare,
          name_len_for_save,
          name_type,
          fs_info,
          fat_fd,
          bts2rd,
          create_node,
          fat_entries,
          name_dir_entry,
          dir_pos,
          &dir_offset,
          &empty_space_offset,
          &empty_space_entry,
          &empty_space_count);
    }
    /* Create a non-existing file/directory if requested */
    if (   retval == RC_OK
        && create_node) {
        switch (name_type) {
          case MSDOS_NAME_SHORT:
              name_len_for_save = msdos_filename_utf8_to_short_name_for_save (
                  converter,
                  name_utf8,
                  name_utf8_len,
                  buffer,
                  MSDOS_SHORT_NAME_LEN);
              if (name_len_for_save > 0 ) {
                  fat_entries = 0;
              }
              else
                  retval = -1;
          break;
          case MSDOS_NAME_LONG:
              name_len_for_save = msdos_filename_utf8_to_long_name_for_save (
                  converter,
                  name_utf8,
                  name_utf8_len,
                  buffer,
                  buffer_size);
              if (name_len_for_save > 0) {
                  fat_entries = (name_len_for_save + MSDOS_LFN_ENTRY_SIZE - 1)
                    / MSDOS_LFN_ENTRY_SIZE;
              }
              else
                  retval = -1;
          break;
          case MSDOS_NAME_INVALID:
          default:
              errno = EINVAL;
              retval = -1;
          break;
        }
        retval = msdos_add_file (
            buffer,
            name_type,
            fs_info,
            fat_fd,
            bts2rd,
            fat_entries,
            name_dir_entry,
            dir_pos,
            dir_offset,
            empty_space_offset,
            empty_space_entry,
            empty_space_count
        );
    }

    return retval;
}


/* msdos_find_node_by_cluster_num_in_fat_file --
 *     Find node with specified number of cluster in fat-file.
 *
 * Note, not updated in the LFN change because it is only used
 *       for . and .. entries and these are always short.
 *
 * PARAMETERS:
 *     mt_entry  - mount table entry
 *     fat_fd    - fat-file descriptor
 *     cl4find   - number of cluster to find
 *     paux      - identify a node location on the disk -
 *                 cluster num and offset inside the cluster
 *     dir_entry - placeholder for found node
 *
 * RETURNS:
 *     RC_OK on success, or error code if error occured
 *
 */
int msdos_find_node_by_cluster_num_in_fat_file(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd,
    uint32_t                              cl4find,
    fat_dir_pos_t                        *dir_pos,
    char                                 *dir_entry
    )
{
    int              rc = RC_OK;
    ssize_t          ret = 0;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    uint32_t         bts2rd = 0;
    uint32_t         i = 0, j = 0;

    if (FAT_FD_OF_ROOT_DIR(fat_fd) &&
       (fs_info->fat.vol.type & (FAT_FAT12 | FAT_FAT16)))
        bts2rd = fat_fd->fat_file_size;
    else
        bts2rd = fs_info->fat.vol.bpc;

    while ((ret = fat_file_read(&fs_info->fat, fat_fd, j * bts2rd, bts2rd,
                                  fs_info->cl_buf)) != FAT_EOF)
    {
        if ( ret < MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE )
            rtems_set_errno_and_return_minus_one( EIO );

        assert(ret == bts2rd);

        for (i = 0; i < bts2rd; i += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            char* entry = (char*) fs_info->cl_buf + i;

            /* if this and all rest entries are empty - return not-found */
            if ((*MSDOS_DIR_ENTRY_TYPE(entry)) ==
                MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY)
                return MSDOS_NAME_NOT_FOUND_ERR;

            /* if this entry is empty - skip it */
            if ((*MSDOS_DIR_ENTRY_TYPE(entry)) ==
                MSDOS_THIS_DIR_ENTRY_EMPTY)
                continue;

            /* if get a non-empty entry - compare clusters num */
            if (MSDOS_EXTRACT_CLUSTER_NUM(entry) == cl4find)
            {
                /* on success fill aux structure and copy all 32 bytes */
                rc = fat_file_ioctl(&fs_info->fat, fat_fd, F_CLU_NUM, j * bts2rd,
                                    &dir_pos->sname.cln);
                if (rc != RC_OK)
                    return rc;

                dir_pos->sname.ofs = i;
                dir_pos->lname.cln = FAT_FILE_SHORT_NAME;
                dir_pos->lname.ofs = FAT_FILE_SHORT_NAME;

                memcpy(dir_entry, entry,
                       MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
                return RC_OK;
            }
        }
        j++;
    }
    return MSDOS_NAME_NOT_FOUND_ERR;
}

int
msdos_sync(rtems_libio_t *iop)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_set_errno_and_return_minus_one(EIO);

    rc = fat_sync(&fs_info->fat);

    rtems_semaphore_release(fs_info->vol_sema);
    return rc;
}
