/*
 *  Miscellaneous routines implementation for MSDOS filesystem
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  @(#) $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* This copied from Linux */
static int day_n[] = { 0,31,59,90,120,151,181,212,243,273,304,334,0,0,0,0 };
		  /* JanFebMarApr May Jun Jul Aug Sep Oct Nov Dec */

#undef CONFIG_ATARI

/* MS-DOS "device special files" */
static const char *reserved_names[] = {
#ifndef CONFIG_ATARI /* GEMDOS is less stupid */
    "CON     ","PRN     ","NUL     ","AUX     ",
    "LPT1    ","LPT2    ","LPT3    ","LPT4    ",
    "COM1    ","COM2    ","COM3    ","COM4    ",
#endif
    NULL };

static char bad_chars[] = "*?<>|\"";
#ifdef CONFIG_ATARI
/* GEMDOS is less restrictive */
static char bad_if_strict[] = " ";
#else
static char bad_if_strict[] = "+=,; ";
#endif

/* The following three functions copied from Linux */
/*
 * Formats an MS-DOS file name. Rejects invalid names
 * 
 * conv is relaxed/normal/strict, name is proposed name,
 * len is the length of the proposed name, res is the result name,
 * dotsOK is if hidden files get dots.
 */
int 
msdos_format_name(char conv, const char *name, int len, char *res, 
                  char dotsOK)
{
	char *walk;
	const char **reserved;
	unsigned char c;
	int space;
	if (name[0] == '.') {  /* dotfile because . and .. already done */
		if (!dotsOK) return -EINVAL;
		/* Get rid of dot - test for it elsewhere */
		name++; len--;
	}
#ifndef CONFIG_ATARI
	space = 1; /* disallow names that _really_ start with a dot */
#else
	space = 0; /* GEMDOS does not care */
#endif
	c = 0;
	for (walk = res; len && walk-res < 8; walk++) {
	    	c = *name++;
		len--;
		if (conv != 'r' && strchr(bad_chars,c)) return -EINVAL;
		if (conv == 's' && strchr(bad_if_strict,c)) return -EINVAL;
  		if (c >= 'A' && c <= 'Z' && conv == 's') return -EINVAL;
		if (c < ' ' || c == ':' || c == '\\') return -EINVAL;
/*  0xE5 is legal as a first character, but we must substitute 0x05     */
/*  because 0xE5 marks deleted files.  Yes, DOS really does this.       */
/*  It seems that Microsoft hacked DOS to support non-US characters     */
/*  after the 0xE5 character was already in use to mark deleted files.  */
		if((res==walk) && (c==0xE5)) c=0x05;
		if (c == '.') break;
		space = (c == ' ');
		*walk = (c >= 'a' && c <= 'z') ? c-32 : c;
	}
	if (space) return -EINVAL;
	if (conv == 's' && len && c != '.') {
		c = *name++;
		len--;
		if (c != '.') return -EINVAL;
	}
	while (c != '.' && len--) c = *name++;
	if (c == '.') {
		while (walk-res < 8) *walk++ = ' ';
	 	while (len > 0 && walk-res < MSDOS_NAME_MAX) {
			c = *name++;
			len--;
			if (conv != 'r' && strchr(bad_chars,c)) return -EINVAL;
			if (conv == 's' && strchr(bad_if_strict,c))
				return -EINVAL;
			if (c < ' ' || c == ':' || c == '\\')
				return -EINVAL;
			if (c == '.') {
				if (conv == 's')
					return -EINVAL;
				break;
			}
			if (c >= 'A' && c <= 'Z' && conv == 's') return -EINVAL;
			space = c == ' ';
			*walk++ = c >= 'a' && c <= 'z' ? c-32 : c;
		}
		if (space) return -EINVAL;
		if (conv == 's' && len) return -EINVAL;
	}
	while (walk-res < MSDOS_NAME_MAX) *walk++ = ' ';
	for (reserved = reserved_names; *reserved; reserved++)
		if (!strncmp(res,*reserved,8)) return -EINVAL;
	return 0;
}

/* Convert a MS-DOS time/date pair to a UNIX date (seconds since 1 1 70) */
unsigned int 
msdos_date_dos2unix(unsigned short time_val,unsigned short date)
{
    int month,year,secs;

    month = ((date >> 5) & 15)-1;
    year = date >> 9;
    secs = (time_val & 31)*2+60*((time_val >> 5) & 63)+
           (time_val >> 11)*3600+86400*
        ((date & 31)-1+day_n[month]+(year/4)+year*365-((year & 3) == 0 &&
        month < 2 ? 1 : 0)+3653);
            /* days since 1.1.70 plus 80's leap day */

    return secs;
}


/* Convert linear UNIX date to a MS-DOS time/date pair */
void msdos_date_unix2dos(int unix_date,
                         unsigned short *time_val, 
                         unsigned short *date)
{
	int day,year,nl_day,month;

	*time_val = (unix_date % 60)/2+(((unix_date/60) % 60) << 5)+
	    (((unix_date/3600) % 24) << 11);
	day = unix_date/86400-3652;
	year = day/365;
	if ((year+3)/4+365*year > day) year--;
	day -= (year+3)/4+365*year;
	if (day == 59 && !(year & 3)) {
		nl_day = day;
		month = 2;
	}
	else {
		nl_day = (year & 3) || day <= 59 ? day : day-1;
		for (month = 0; month < 12; month++)
			if (day_n[month] > nl_day) break;
	}
	*date = nl_day-day_n[month-1]+1+(month << 5)+(year << 9);
}


/* msdos_get_token --
 *     Routine to get a token (name or separator) from the path.
 *
 * PARAMETERS:
 *     path      - path to get token from
 *     ret_token - returned token
 *     token_len - length of returned token
 *
 * RETURNS:
 *     token type, token and token length 
 *
 */
msdos_token_types_t 
msdos_get_token(const char *path, char *ret_token, int *token_len)
{
    int                 rc = RC_OK;
    register int        i = 0;
    msdos_token_types_t type = MSDOS_NAME;
    char                token[MSDOS_NAME_MAX_WITH_DOT+1];  
    register char       c;

    /* 
     *  Copy a name into token.  (Remember NULL is a token.)
     */
    c = path[i];
    while ( (!msdos_is_separator(c)) && (i <= MSDOS_NAME_MAX_WITH_DOT) ) 
    {
        token[i] = c;
        if ( i == MSDOS_NAME_MAX_WITH_DOT )
            return MSDOS_INVALID_TOKEN;
        if ( !msdos_is_valid_name_char(c) )
            return MSDOS_INVALID_TOKEN;   
        c = path [++i];
    }

    /*
     *  Copy a seperator into token.
     */
    if ( i == 0 ) 
    {
        token[i] = c;
        if ( token[i] != '\0' ) 
        {
            i++;
            type = MSDOS_CURRENT_DIR;
        } 
        else  
            type = MSDOS_NO_MORE_PATH;
    } 
    else if (token[ i-1 ] != '\0') 
        token[i] = '\0';

    /*
     *  Set token_len to the number of characters copied.
     */
    *token_len = i;

    /*
     *  If we copied something that was not a seperator see if
     *  it was a special name.
     */
    if ( type == MSDOS_NAME ) 
    {
        if ( strcmp( token, "..") == 0 )
        {
            strcpy(ret_token, MSDOS_DOTDOT_NAME);
            type = MSDOS_UP_DIR;
            return type;
        }  

        if ( strcmp( token, "." ) == 0 )
        {
            strcpy(ret_token, MSDOS_DOT_NAME);
            type = MSDOS_CURRENT_DIR;
            return type;        
        }

        rc = msdos_format_name('r', token, *token_len, ret_token, 0);
        if ( rc != RC_OK )
            return MSDOS_INVALID_TOKEN;     
    }
    ret_token[MSDOS_NAME_MAX] = '\0';
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
    char                             *name
    )
{
    int              rc = RC_OK;
    msdos_fs_info_t *fs_info = parent_loc->mt_entry->fs_info;
    fat_file_fd_t   *fat_fd = NULL;
    fat_auxiliary_t  aux;
    unsigned short   time_val = 0;
    unsigned short   date = 0;
    unsigned char    node_entry[MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE];
  
    memset(node_entry, 0, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
  
    /* 
     * find the node which correspondes to the name in the directory pointed by 
     * 'parent_loc'
     */
    rc = msdos_get_name_node(parent_loc, name, &aux, node_entry);
    if (rc != RC_OK)
        return rc;

    /* open fat-file corresponded to the found node */
    rc = fat_file_open(parent_loc->mt_entry, aux.cln, aux.ofs, &fat_fd);
    if (rc != RC_OK)
        return rc;
  
    /*
     * I don't like this if, but: we should do it , or should write new file 
     * size and first cluster num to the disk after each write operation 
     * (even if one byte is written  - that is TOO non-optimize) because 
     * otherwise real values of these fields stored in fat-file descriptor 
     * may be accidentely rewritten with wrong values stored on the disk
     */
    if (fat_fd->links_num == 1)
    {
        fat_fd->info_cln = aux.cln;
        fat_fd->info_ofs = aux.ofs;
        fat_fd->cln = MSDOS_EXTRACT_CLUSTER_NUM(node_entry);
        fat_fd->first_char = *MSDOS_DIR_NAME(node_entry);
    
        time_val = *MSDOS_DIR_WRITE_TIME(node_entry);
        date = *MSDOS_DIR_WRITE_DATE(node_entry);
    
        fat_fd->mtime = msdos_date_dos2unix(CF_LE_W(time_val), CF_LE_W(date));
 
        if ((*MSDOS_DIR_ATTR(node_entry)) & MSDOS_ATTR_DIRECTORY)
        {
            fat_fd->fat_file_type = FAT_DIRECTORY;
            fat_fd->size_limit = MSDOS_MAX_DIR_LENGHT;                                    
  
            rc = fat_file_size(parent_loc->mt_entry, fat_fd);
            if (rc != RC_OK)
            {
                fat_file_close(parent_loc->mt_entry, fat_fd);
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
    rc = fat_file_close(parent_loc->mt_entry, parent_loc->node_access);
    if (rc != RC_OK)
    {
        fat_file_close(parent_loc->mt_entry, fat_fd);
        return rc;
    }

    /* update node_info_ptr field */
    parent_loc->node_access = fat_fd;
  
    return rc;
}  

/* msdos_get_name_node --
 *     This routine is used in two ways: for a new mode creation (a) or for
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
 *     name_dir_entry - node to create/placeholder for found node (IN/OUT)
 *
 * RETURNS:
 *     RC_OK, filled aux_struct_ptr and name_dir_entry on success, or -1 if 
 *     error occured (errno set apropriately)
 *
 */
int
msdos_get_name_node(
    rtems_filesystem_location_info_t *parent_loc, 
    char                             *name, 
    fat_auxiliary_t                  *paux,
    char                             *name_dir_entry
    )
{
    int              rc = RC_OK;
    ssize_t          ret = 0;
    msdos_fs_info_t *fs_info = parent_loc->mt_entry->fs_info;
    fat_file_fd_t   *fat_fd = parent_loc->node_access;
    unsigned32       dotdot_cln = 0;

    /* find name in fat-file which correspondes to the directory */
    rc = msdos_find_name_in_fat_file(parent_loc->mt_entry, fat_fd, name, paux,
                                     name_dir_entry);
    if ((rc != RC_OK) && (rc != MSDOS_NAME_NOT_FOUND_ERR))
        return rc;
  
    /* if we search for valid name and name not found -> return */
    if ((rc == MSDOS_NAME_NOT_FOUND_ERR) && (name != NULL))
        return rc;
  
    /* 
     * if we try to create new entry and the directory is not big enough 
     * currently - try to enlarge directory   
     */
    if ((rc == MSDOS_NAME_NOT_FOUND_ERR) && (name == NULL))
    {
        ret = fat_file_write(parent_loc->mt_entry, fat_fd, 
                             fat_fd->fat_file_size, 
                             MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE, 
                             name_dir_entry);
        if (ret == -1)
            return -1;  

        /* on success directory is enlarged by a new cluster */
        fat_fd->fat_file_size += fs_info->fat.vol.bpc;
    
        /* get cluster num where a new node located */
        rc = fat_file_ioctl(parent_loc->mt_entry, fat_fd, F_CLU_NUM,
                            fat_fd->fat_file_size - 1, &paux->cln);
                        
        if (rc != RC_OK)
            return rc;

        /* 
         * if new cluster allocated succesfully then new node is at very 
         * beginning of the cluster (offset is computed in bytes) 
         */
        paux->ofs = 0;
        return RC_OK;                                                 
    }                                   
                                   
    /* 
     * if we have deal with ".." - it is a special case :((( 
     *
     * Really, we should return cluster num and offset not of ".." slot, but
     * slot which correspondes to real directory name.
     */
    if ((rc == RC_OK) && (name != NULL))
    {
        if (strncmp(name, MSDOS_DOTDOT_NAME, MSDOS_SHORT_NAME_LEN) == 0)
        {
            dotdot_cln = MSDOS_EXTRACT_CLUSTER_NUM((name_dir_entry));

            /* are we right under root dir ? */
            if (dotdot_cln == 0)
            { 
                /* 
                 * we can relax about first_char field - it never should be 
                 * used for root dir
                 */
                paux->cln = FAT_ROOTDIR_CLUSTER_NUM;
                paux->ofs = 0;
            }  
            else
            {
                rc = msdos_get_dotdot_dir_info_cluster_num_and_offset(
                        parent_loc->mt_entry,
                        dotdot_cln,
                        paux,
                        name_dir_entry
                        );
                if (rc != RC_OK)
                    return rc;
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
    unsigned32                            cln,
    fat_auxiliary_t                      *paux,
    char                                 *dir_entry
    )
{
    int              rc = RC_OK;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    fat_file_fd_t   *fat_fd = NULL;
    unsigned char    dot_node[MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE];
    unsigned char    dotdot_node[MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE];
    unsigned char    cur_node[MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE];
    unsigned32       cl4find = 0;
  
    memset(dot_node, 0, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);  
    memset(dotdot_node, 0, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
    memset(cur_node, 0, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
  
    /*
     * open fat-file corresponded to ".."
     */
    rc = fat_file_open(mt_entry, paux->cln, paux->ofs, &fat_fd);
    if (rc != RC_OK)
        return rc;
  
    fat_fd->info_cln = paux->cln;
    fat_fd->info_ofs = paux->ofs;
    fat_fd->cln = cln;
    fat_fd->fat_file_type = FAT_DIRECTORY;
    fat_fd->size_limit = MSDOS_MAX_DIR_LENGHT;
    
    fat_fd->map.file_cln = 0;
    fat_fd->map.disk_cln = fat_fd->cln;

    rc = fat_file_size(mt_entry, fat_fd);
    if (rc != RC_OK)
    {
        fat_file_close(mt_entry, fat_fd);
        return rc;
    }  
  
    /* find "." node in opened directory */
    rc = msdos_find_name_in_fat_file(mt_entry, fat_fd, MSDOS_DOT_NAME, paux, 
                                     dot_node);
                                   
    if (rc != RC_OK)
    {
        fat_file_close(mt_entry, fat_fd);
        return rc;
    }
  
    /* find ".." node in opened directory */
    rc = msdos_find_name_in_fat_file(mt_entry, fat_fd, MSDOS_DOTDOT_NAME, paux, 
                                     dotdot_node);
                                   
    if (rc != RC_OK) 
    {
        fat_file_close(mt_entry, fat_fd);
        return rc;
    }

    cl4find = MSDOS_EXTRACT_CLUSTER_NUM(dot_node);
  
    /* close fat-file corresponded to ".." directory */
    rc = fat_file_close(mt_entry, fat_fd);
    if ( rc != RC_OK )
        return rc;

    if ( (MSDOS_EXTRACT_CLUSTER_NUM(dotdot_node)) == 0)
    {
        /* 
         * we handle root dir for all FAT types in the same way with the 
         * ordinary directories ( through fat_file_* calls )
         */
        paux->cln = FAT_ROOTDIR_CLUSTER_NUM;
        paux->ofs = 0;
    }

    /* open fat-file corresponded to second ".." */
    rc = fat_file_open(mt_entry, paux->cln, paux->ofs, &fat_fd);
    if (rc != RC_OK)
        return rc;

    fat_fd->info_cln = paux->cln;
    fat_fd->info_ofs = paux->ofs;

    if ((MSDOS_EXTRACT_CLUSTER_NUM(dotdot_node)) == 0)
        fat_fd->cln = fs_info->fat.vol.rdir_cl;
    else
        fat_fd->cln = MSDOS_EXTRACT_CLUSTER_NUM(dotdot_node);

    fat_fd->fat_file_type = FAT_DIRECTORY;
    fat_fd->size_limit = MSDOS_MAX_DIR_LENGHT;
    
    fat_fd->map.file_cln = 0;
    fat_fd->map.disk_cln = fat_fd->cln;

    rc = fat_file_size(mt_entry, fat_fd);
    if (rc != RC_OK)
    {
        fat_file_close(mt_entry, fat_fd);
        return rc;
    }  
  
    /* in this directory find slot with specified cluster num */
    rc = msdos_find_node_by_cluster_num_in_fat_file(mt_entry, fat_fd, cl4find, 
                                                    paux, dir_entry);
    if (rc != RC_OK)
    {
        fat_file_close(mt_entry, fat_fd);
        return rc;
    }
    rc = fat_file_close(mt_entry, fat_fd);
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
    ssize_t          ret1 = 0, ret2 = 0;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned short   time_val;
    unsigned short   date;
    unsigned32       sec = 0;
    unsigned32       byte = 0;
  
    msdos_date_unix2dos(fat_fd->mtime, &time_val, &date);
  
    /* 
     * calculate input for _fat_block_write: convert (cluster num, offset) to
     * (sector num, new offset)
     */
    sec = fat_cluster_num_to_sector_num(mt_entry, fat_fd->info_cln);
    sec += (fat_fd->info_ofs >> fs_info->fat.vol.sec_log2);
    /* byte points to start of 32bytes structure */
    byte = fat_fd->info_ofs & (fs_info->fat.vol.bps - 1);
   
    time_val = CT_LE_W(time_val);
    ret1 = _fat_block_write(mt_entry, sec, byte + MSDOS_FILE_WTIME_OFFSET,
                            2, (char *)(&time_val));
    date = CT_LE_W(date);
    ret2 = _fat_block_write(mt_entry, sec, byte + MSDOS_FILE_WDATE_OFFSET,
                            2, (char *)(&date));

    if ( (ret1 < 0) || (ret2 < 0) )
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
    unsigned32       new_cln = fat_fd->cln;
    unsigned16       le_cl_low = 0;
    unsigned16       le_cl_hi = 0;  
    unsigned32       sec = 0;
    unsigned32       byte = 0;

    /* 
     * calculate input for _fat_block_write: convert (cluster num, offset) to
     * (sector num, new offset)
     */
    sec = fat_cluster_num_to_sector_num(mt_entry, fat_fd->info_cln);
    sec += (fat_fd->info_ofs >> fs_info->fat.vol.sec_log2);
    /* byte from points to start of 32bytes structure */
    byte = fat_fd->info_ofs & (fs_info->fat.vol.bps - 1);
 
    le_cl_low = CT_LE_W((unsigned16)(new_cln & 0x0000FFFF));
    ret1 = _fat_block_write(mt_entry, sec, 
                            byte + MSDOS_FIRST_CLUSTER_LOW_OFFSET, 2,
                            (char *)(&le_cl_low));
    le_cl_hi = CT_LE_W((unsigned16)((new_cln & 0xFFFF0000) >> 16));
    ret2 = _fat_block_write(mt_entry, sec,
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
    unsigned32       le_new_length = 0;
    unsigned32       sec = 0;
    unsigned32       byte = 0;

    sec = fat_cluster_num_to_sector_num(mt_entry, fat_fd->info_cln);
    sec += (fat_fd->info_ofs >> fs_info->fat.vol.sec_log2);
    byte = (fat_fd->info_ofs & (fs_info->fat.vol.bps - 1));

    le_new_length = CT_LE_L((fat_fd->fat_file_size));
    ret = _fat_block_write(mt_entry, sec, byte + MSDOS_FILE_SIZE_OFFSET, 4,
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
    unsigned32                            cl,
    unsigned32                            ofs,
    unsigned char                         fchar
    )
{
    ssize_t          ret = 0;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32       sec = 0;
    unsigned32       byte = 0;

    sec = fat_cluster_num_to_sector_num(mt_entry, cl);
    sec += (ofs >> fs_info->fat.vol.sec_log2);
    byte = (ofs & (fs_info->fat.vol.bps - 1));

    ret = _fat_block_write(mt_entry, sec, byte + MSDOS_FILE_NAME_OFFSET, 1, 
                           &fchar);
    if ( ret < 0)
        return -1;

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
    rtems_boolean                        *ret_val
    )
{
    ssize_t          ret = 0;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32       j = 0, i = 0;
  
    /* dir is not empty */
    *ret_val = FALSE;

    while ((ret = fat_file_read(mt_entry, fat_fd, j * fs_info->fat.vol.bps,
                                  fs_info->fat.vol.bps, 
                                  fs_info->cl_buf)) != FAT_EOF)
    {
        if (ret < MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
            return -1;

        assert(ret == fs_info->fat.vol.bps);
        
        for (i = 0; 
             i < fs_info->fat.vol.bps; 
             i += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            if (((*MSDOS_DIR_NAME(fs_info->cl_buf + i)) == 
                 MSDOS_THIS_DIR_ENTRY_EMPTY) || 
                (strncmp(MSDOS_DIR_NAME((fs_info->cl_buf + i)), MSDOS_DOT_NAME, 
                         MSDOS_SHORT_NAME_LEN) == 0) ||
                (strncmp(MSDOS_DIR_NAME((fs_info->cl_buf + i)),
                         MSDOS_DOTDOT_NAME, 
                         MSDOS_SHORT_NAME_LEN) == 0))
                continue;

            if ((*MSDOS_DIR_NAME(fs_info->cl_buf + i)) ==  
                MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY)
            {
                *ret_val = TRUE;
                return RC_OK;
            }
            return RC_OK;
        }    
        j++; 
    }
    *ret_val = TRUE;
    return RC_OK;
}


/* msdos_find_name_in_fat_file --
 *     This routine is used in two ways: for a new mode creation (a) or for
 *     search the node which correspondes to the 'name' parameter (b).
 *     In case (a) name should be set up to NULL and 'name_dir_entry' should 
 *     point to initialized 32 bytes structure described a new node. 
 *     In case (b) 'name' should contain a valid string.
 *
 *     (a): reading fat-file corresponded to directory we are going to create
 *          node in. If found free slot write contents of name_dir_entry into
 *          it.  
 *
 *     (b): reading fat-file corresponded to directory and trying to find slot
 *          with the name field == name parameter
 *
 * PARAMETERS:
 *     mt_entry       - mount table entry 
 *     fat_fd         - fat-file descriptor   
 *     name           - NULL or name to find 
 *     paux           - identify a node location on the disk -
 *                      number of cluster and offset inside the cluster
 *     name_dir_entry - node to create/placeholder for found node
 *
 * RETURNS:
 *     RC_OK on success, or error code if error occured (errno set 
 *     appropriately)
 *
 */
int
msdos_find_name_in_fat_file(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd, 
    char                                 *name, 
    fat_auxiliary_t                      *paux,
    char                                 *name_dir_entry
    )
{
    int              rc = RC_OK;
    ssize_t          ret = 0;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32       i = 0, j = 0;
    unsigned32       bts2rd = 0;

    if (FAT_FD_OF_ROOT_DIR(fat_fd) && 
       (fs_info->fat.vol.type & (FAT_FAT12 | FAT_FAT16)))
        bts2rd = fat_fd->fat_file_size;  
    else
        bts2rd = fs_info->fat.vol.bpc;
  
    while ((ret = fat_file_read(mt_entry, fat_fd, (j * bts2rd), bts2rd, 
                                fs_info->cl_buf)) != FAT_EOF) 
    {
        if (ret < MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
            set_errno_and_return_minus_one(EIO);
            
        assert(ret == bts2rd);    

        for (i = 0; i < bts2rd; i += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            /* is the entry empty ? */
            if (((*MSDOS_DIR_NAME(fs_info->cl_buf + i)) == 
                 MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY) ||
                 ((*MSDOS_DIR_NAME(fs_info->cl_buf + i)) == 
                 MSDOS_THIS_DIR_ENTRY_EMPTY))
            {
                /* whether we are looking for an empty entry */
                if (name == NULL)
                {
                    /* get current cluster number */
                    rc = fat_file_ioctl(mt_entry, fat_fd, F_CLU_NUM, 
                                        j * bts2rd, &paux->cln);
                    if (rc != RC_OK)
                        return rc;

                    /* offset is computed in bytes */
                    paux->ofs = i;
          
                    /* write new node entry */
                    ret = fat_file_write(mt_entry, fat_fd, j * bts2rd + i, 
                                         MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE,
                                         name_dir_entry);
                    if (ret != MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
                        return -1;

                    /* 
                     * we don't update fat_file_size here - it should not 
                     * increase 
                     */
                    return RC_OK;
                }

                /* 
                 * if name != NULL and there is no more entries in the 
                 * directory - return name-not-found
                 */
                if (((*MSDOS_DIR_NAME(fs_info->cl_buf + i)) == 
                     MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY))
                    return MSDOS_NAME_NOT_FOUND_ERR;
            }     
            else
            {
                /* entry not empty and name != NULL -> compare names */
                if (name != NULL)
                {
                    if (strncmp(MSDOS_DIR_NAME((fs_info->cl_buf + i)), name, 
                                MSDOS_SHORT_NAME_LEN) == 0)
                    {
                        /* 
                         * we get the entry we looked for - fill auxiliary 
                         * structure and copy all 32 bytes of the entry
                         */
                        rc = fat_file_ioctl(mt_entry, fat_fd, F_CLU_NUM, 
                                            j * bts2rd, &paux->cln);
                        if (rc != RC_OK)
                            return rc;

                        /* offset is computed in bytes */
                        paux->ofs = i;
                        memcpy(name_dir_entry,(fs_info->cl_buf + i),
                               MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
                        return RC_OK;
                    }
                }  
            }      
        }                          
        j++;
    }                        
    return MSDOS_NAME_NOT_FOUND_ERR;
}

/* msdos_find_node_by_cluster_num_in_fat_file --
 *     Find node with specified number of cluster in fat-file.
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
int
msdos_find_node_by_cluster_num_in_fat_file(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd,
    unsigned32                            cl4find, 
    fat_auxiliary_t                      *paux,
    char                                 *dir_entry
    )
{
    int              rc = RC_OK;
    ssize_t          ret = 0;
    msdos_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32       bts2rd = 0;  
    unsigned32       i = 0, j = 0;

    if (FAT_FD_OF_ROOT_DIR(fat_fd) && 
       (fs_info->fat.vol.type & (FAT_FAT12 | FAT_FAT16)))
        bts2rd = fat_fd->fat_file_size;  
    else
        bts2rd = fs_info->fat.vol.bpc;

    while ((ret = fat_file_read(mt_entry, fat_fd, j * bts2rd, bts2rd,
                                  fs_info->cl_buf)) != FAT_EOF)
    {
        if ( ret < MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE )
            set_errno_and_return_minus_one( EIO );

        assert(ret == bts2rd);
        
        for (i = 0; i < bts2rd; i += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            /* if this and all rest entries are empty - return not-found */
            if ((*MSDOS_DIR_NAME(fs_info->cl_buf + i)) ==
                MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY)
                return MSDOS_NAME_NOT_FOUND_ERR;

            /* if this entry is empty - skip it */ 
            if ((*MSDOS_DIR_NAME(fs_info->cl_buf + i)) ==
                MSDOS_THIS_DIR_ENTRY_EMPTY)
                continue;

            /* if get a non-empty entry - compare clusters num */
            if (MSDOS_EXTRACT_CLUSTER_NUM((fs_info->cl_buf + i)) == cl4find)
            {
                /* on success fill aux structure and copy all 32 bytes */
                rc = fat_file_ioctl(mt_entry, fat_fd, F_CLU_NUM, j * bts2rd, 
                                    &paux->cln);
                if (rc != RC_OK)
                    return rc;

                paux->ofs = i;
                memcpy(dir_entry, fs_info->cl_buf + i, 
                       MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
                return RC_OK;
            }
        }                          
        j++;
    }                        
    return MSDOS_NAME_NOT_FOUND_ERR;
}
