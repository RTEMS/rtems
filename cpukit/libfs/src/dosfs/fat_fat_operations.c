/*
 *  fat_fat_operations.c
 *
 *  General operations on File Allocation Table
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"

/* fat_scan_fat_for_free_clusters --
 *     Allocate chain of free clusters from Files Allocation Table
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *     chain    - the number of the first allocated cluster (first cluster
 *                in  the chain)
 *     count    - count of clusters to allocate (chain length)
 *
 * RETURNS:
 *     RC_OK on success, or error code if error occured (errno set
 *     appropriately)
 *
 *
 */
int
fat_scan_fat_for_free_clusters(
    fat_fs_info_t                        *fs_info,
    uint32_t                             *chain,
    uint32_t                              count,
    uint32_t                             *cls_added,
    uint32_t                             *last_cl,
    bool                                  zero_fill
    )
{
    int            rc = RC_OK;
    uint32_t       cl4find = 2;
    uint32_t       next_cln = 0;
    uint32_t       save_cln = 0;
    uint32_t       data_cls_val = fs_info->vol.data_cls + 2;
    uint32_t       i = 2;

    *cls_added = 0;

    if (count == 0)
        return rc;

    if (fs_info->vol.next_cl != FAT_UNDEFINED_VALUE)
        cl4find = fs_info->vol.next_cl;

    /*
     * fs_info->vol.data_cls is exactly the count of data clusters
     * starting at cluster 2, so the maximum valid cluster number is
     * (fs_info->vol.data_cls + 1)
     */
    while (i < data_cls_val)
    {
        rc = fat_get_fat_cluster(fs_info, cl4find, &next_cln);
        if ( rc != RC_OK )
        {
            if (*cls_added != 0)
                fat_free_fat_clusters_chain(fs_info, (*chain));
            return rc;
        }

        if (next_cln == FAT_GENFAT_FREE)
        {
            /*
             * We are enforced to process allocation of the first free cluster
             * by separate 'if' statement because otherwise undo function
             * wouldn't work properly
             */
            if (*cls_added == 0)
            {
                *chain = cl4find;
                rc = fat_set_fat_cluster(fs_info, cl4find, FAT_GENFAT_EOC);
                if ( rc != RC_OK )
                {
                    /*
                     * this is the first cluster we tried to allocate so no
                     * cleanup activity needed
                     */
                     return rc;
                }
            }
            else
            {
                /* set EOC value to new allocated cluster */
                rc = fat_set_fat_cluster(fs_info, cl4find, FAT_GENFAT_EOC);
                if ( rc != RC_OK )
                {
                    /* cleanup activity */
                    fat_free_fat_clusters_chain(fs_info, (*chain));
                    return rc;
                }

                rc = fat_set_fat_cluster(fs_info, save_cln, cl4find);
                if ( rc != RC_OK )
                    goto cleanup;
            }

            if (zero_fill) {
                uint32_t sec = fat_cluster_num_to_sector_num(fs_info,
                                                             cl4find);

                rc = _fat_block_zero(fs_info, sec, 0, fs_info->vol.bpc);
                if ( rc != RC_OK )
                    goto cleanup;
            }

            save_cln = cl4find;
            (*cls_added)++;

            /* have we satisfied request ? */
            if (*cls_added == count)
            {
                    fs_info->vol.next_cl = save_cln;
                    if (fs_info->vol.free_cls != 0xFFFFFFFF)
                        fs_info->vol.free_cls -= (*cls_added);
                *last_cl = save_cln;
                fat_buf_release(fs_info);
                return rc;
            }
        }
        i++;
        cl4find++;
        if (cl4find >= data_cls_val)
            cl4find = 2;
    }

        fs_info->vol.next_cl = save_cln;
        if (fs_info->vol.free_cls != 0xFFFFFFFF)
            fs_info->vol.free_cls -= (*cls_added);

    *last_cl = save_cln;
    fat_buf_release(fs_info);
    return RC_OK;

cleanup:

    /* cleanup activity */
    fat_free_fat_clusters_chain(fs_info, (*chain));
    /* trying to save last allocated cluster for future use */
    fat_set_fat_cluster(fs_info, cl4find, FAT_GENFAT_FREE);
    fat_buf_release(fs_info);
    return rc;
}

/* fat_free_fat_clusters_chain --
 *     Free chain of clusters in Files Allocation Table.
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *     chain    - number of the first cluster in  the chain
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int
fat_free_fat_clusters_chain(
    fat_fs_info_t                        *fs_info,
    uint32_t                              chain
    )
{
    int            rc = RC_OK, rc1 = RC_OK;
    uint32_t       cur_cln = chain;
    uint32_t       next_cln = 0;
    uint32_t       freed_cls_cnt = 0;

    while ((cur_cln & fs_info->vol.mask) < fs_info->vol.eoc_val)
    {
        rc = fat_get_fat_cluster(fs_info, cur_cln, &next_cln);
        if ( rc != RC_OK )
        {
              if(fs_info->vol.free_cls != FAT_UNDEFINED_VALUE)
                fs_info->vol.free_cls += freed_cls_cnt;

            fat_buf_release(fs_info);
            return rc;
        }

        rc = fat_set_fat_cluster(fs_info, cur_cln, FAT_GENFAT_FREE);
        if ( rc != RC_OK )
            rc1 = rc;

        freed_cls_cnt++;
        cur_cln = next_cln;
    }

        fs_info->vol.next_cl = chain;
        if (fs_info->vol.free_cls != FAT_UNDEFINED_VALUE)
            fs_info->vol.free_cls += freed_cls_cnt;

    fat_buf_release(fs_info);
    if (rc1 != RC_OK)
        return rc1;

    return RC_OK;
}

/* fat_get_fat_cluster --
 *     Fetches the contents of the cluster (link to next cluster in the chain)
 *     from Files Allocation Table.
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *     cln      - number of cluster to fetch the contents from
 *     ret_val  - contents of the cluster 'cln' (link to next cluster in
 *                the chain)
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured
 *     and errno set appropriately
 */
int
fat_get_fat_cluster(
    fat_fs_info_t                        *fs_info,
    uint32_t                              cln,
    uint32_t                             *ret_val
    )
{
    int                     rc = RC_OK;
    rtems_bdbuf_buffer     *block0 = NULL;
    uint32_t                sec = 0;
    uint32_t                ofs = 0;

    /* sanity check */
    if ( (cln < 2) || (cln > (fs_info->vol.data_cls + 1)) )
        rtems_set_errno_and_return_minus_one(EIO);

    sec = (FAT_FAT_OFFSET(fs_info->vol.type, cln) >> fs_info->vol.sec_log2) +
          fs_info->vol.afat_loc;
    ofs = FAT_FAT_OFFSET(fs_info->vol.type, cln) & (fs_info->vol.bps - 1);

    rc = fat_buf_access(fs_info, sec, FAT_OP_TYPE_READ, &block0);
    if (rc != RC_OK)
        return rc;

    switch ( fs_info->vol.type )
    {
        case FAT_FAT12:
            /*
             * we are enforced in complex computations for FAT12 to escape CPU
             * align problems for some architectures
             */
            *ret_val = (*((uint8_t   *)(block0->buffer + ofs)));
            if ( ofs == (fs_info->vol.bps - 1) )
            {
                rc = fat_buf_access(fs_info, sec + 1, FAT_OP_TYPE_READ,
                                    &block0);
                if (rc != RC_OK)
                    return rc;

                *ret_val |= (*((uint8_t   *)(block0->buffer)))<<8;
            }
            else
            {
                *ret_val |= (*((uint8_t   *)(block0->buffer + ofs + 1)))<<8;
            }

            if ( FAT_CLUSTER_IS_ODD(cln) )
                *ret_val = (*ret_val) >> FAT12_SHIFT;
            else
                *ret_val = (*ret_val) & FAT_FAT12_MASK;
            break;

        case FAT_FAT16:
            *ret_val = *((uint16_t   *)(block0->buffer + ofs));
            *ret_val = CF_LE_W(*ret_val);
            break;

        case FAT_FAT32:
            *ret_val = *((uint32_t   *)(block0->buffer + ofs));
            *ret_val = CF_LE_L(*ret_val);
            break;

        default:
            rtems_set_errno_and_return_minus_one(EIO);
            break;
    }

    return RC_OK;
}

/* fat_set_fat_cluster --
 *     Set the contents of the cluster (link to next cluster in the chain)
 *     from Files Allocation Table.
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *     cln      - number of cluster to set contents to
 *     in_val   - value to set
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured
 *     and errno set appropriately
 */
int
fat_set_fat_cluster(
    fat_fs_info_t                        *fs_info,
    uint32_t                              cln,
    uint32_t                              in_val
    )
{
    int                 rc = RC_OK;
    uint32_t            sec = 0;
    uint32_t            ofs = 0;
    uint16_t            fat16_clv = 0;
    uint32_t            fat32_clv = 0;
    rtems_bdbuf_buffer *block0 = NULL;

    /* sanity check */
    if ( (cln < 2) || (cln > (fs_info->vol.data_cls + 1)) )
        rtems_set_errno_and_return_minus_one(EIO);

    sec = (FAT_FAT_OFFSET(fs_info->vol.type, cln) >> fs_info->vol.sec_log2) +
          fs_info->vol.afat_loc;
    ofs = FAT_FAT_OFFSET(fs_info->vol.type, cln) & (fs_info->vol.bps - 1);

    rc = fat_buf_access(fs_info, sec, FAT_OP_TYPE_READ, &block0);
    if (rc != RC_OK)
        return rc;

    switch ( fs_info->vol.type )
    {
        case FAT_FAT12:
            if ( FAT_CLUSTER_IS_ODD(cln) )
            {
                fat16_clv = ((uint16_t  )in_val) << FAT_FAT12_SHIFT;
                *((uint8_t   *)(block0->buffer + ofs)) =
                        (*((uint8_t   *)(block0->buffer + ofs))) & 0x0F;

                *((uint8_t   *)(block0->buffer + ofs)) =
                        (*((uint8_t   *)(block0->buffer + ofs))) |
                        (uint8_t  )(fat16_clv & 0x00FF);

                fat_buf_mark_modified(fs_info);

                if ( ofs == (fs_info->vol.bps - 1) )
                {
                    rc = fat_buf_access(fs_info, sec + 1, FAT_OP_TYPE_READ,
                                        &block0);
                    if (rc != RC_OK)
                        return rc;

                     *((uint8_t   *)(block0->buffer)) &= 0x00;

                     *((uint8_t   *)(block0->buffer)) =
                            (*((uint8_t   *)(block0->buffer))) |
                            (uint8_t  )((fat16_clv & 0xFF00)>>8);

                     fat_buf_mark_modified(fs_info);
                }
                else
                {
                    *((uint8_t   *)(block0->buffer + ofs + 1)) &= 0x00;

                    *((uint8_t   *)(block0->buffer + ofs + 1)) =
                            (*((uint8_t   *)(block0->buffer + ofs + 1))) |
                            (uint8_t  )((fat16_clv & 0xFF00)>>8);
                }
            }
            else
            {
                fat16_clv = ((uint16_t  )in_val) & FAT_FAT12_MASK;
                *((uint8_t   *)(block0->buffer + ofs)) &= 0x00;

                *((uint8_t   *)(block0->buffer + ofs)) =
                        (*((uint8_t   *)(block0->buffer + ofs))) |
                        (uint8_t  )(fat16_clv & 0x00FF);

                fat_buf_mark_modified(fs_info);

                if ( ofs == (fs_info->vol.bps - 1) )
                {
                    rc = fat_buf_access(fs_info, sec + 1, FAT_OP_TYPE_READ,
                                        &block0);
                    if (rc != RC_OK)
                        return rc;

                    *((uint8_t   *)(block0->buffer)) =
                            (*((uint8_t   *)(block0->buffer))) & 0xF0;

                    *((uint8_t   *)(block0->buffer)) =
                            (*((uint8_t   *)(block0->buffer))) |
                            (uint8_t  )((fat16_clv & 0xFF00)>>8);

                    fat_buf_mark_modified(fs_info);
                }
                else
                {
                    *((uint8_t   *)(block0->buffer + ofs + 1)) =
                      (*((uint8_t   *)(block0->buffer + ofs + 1))) & 0xF0;

                    *((uint8_t   *)(block0->buffer + ofs+1)) =
                           (*((uint8_t   *)(block0->buffer + ofs+1))) |
                           (uint8_t  )((fat16_clv & 0xFF00)>>8);
                }
            }
            break;

        case FAT_FAT16:
            *((uint16_t   *)(block0->buffer + ofs)) =
                    (uint16_t  )(CT_LE_W(in_val));
            fat_buf_mark_modified(fs_info);
            break;

        case FAT_FAT32:
            fat32_clv = CT_LE_L((in_val & FAT_FAT32_MASK));

            *((uint32_t   *)(block0->buffer + ofs)) =
            (*((uint32_t   *)(block0->buffer + ofs))) & (CT_LE_L(0xF0000000));

            *((uint32_t   *)(block0->buffer + ofs)) =
                   fat32_clv | (*((uint32_t   *)(block0->buffer + ofs)));

            fat_buf_mark_modified(fs_info);
            break;

        default:
            rtems_set_errno_and_return_minus_one(EIO);
            break;

    }

    return RC_OK;
}
