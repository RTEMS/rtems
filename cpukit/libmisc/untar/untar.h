/**
 * @file
 *
 * @brief Untar an Image
 *
 * This file defines the interface to methods which can untar an image.
 */

/*
 *  Written by: Jake Janovetz <janovetz@tempest.ece.uiuc.edu>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_UNTAR_H
#define _RTEMS_UNTAR_H

#include <stdbool.h>
#include <stddef.h>
#include <tar.h>

#include <rtems/print.h>

/**
 *  @defgroup libmisc_untar_img Untar Image
 *
 *  @ingroup libmisc
 */
/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

#define UNTAR_SUCCESSFUL         0
#define UNTAR_FAIL               1
#define UNTAR_INVALID_CHECKSUM   2
#define UNTAR_INVALID_HEADER     3


int Untar_FromMemory(void *tar_buf, size_t size);
int Untar_FromMemory_Print(void *tar_buf, size_t size, const rtems_printer* printer);
int Untar_FromFile(const char *tar_name);
int Untar_FromFile_Print(const char *tar_name, const rtems_printer* printer);

typedef struct {
  /**
   * @brief Current context state.
   */
  enum {
    UNTAR_CHUNK_HEADER,
    UNTAR_CHUNK_SKIP,
    UNTAR_CHUNK_WRITE,
    UNTAR_CHUNK_ERROR
  } state;

  /**
   * @brief Header buffer.
   */
  char header[512];

  /**
   * @brief Name buffer.
   */
  char fname[100];

  /**
   * @brief Number of bytes of overall length are already processed.
   */
  size_t done_bytes;

  /**
   * @brief Overall amount of bytes to be processed.
   */
  long unsigned todo_bytes;

  /**
   * @brief Overall amount of blocks to be processed.
   */
  unsigned long todo_blocks;

  /**
   * @brief File descriptor of output file.
   */
  int out_fd;
} Untar_ChunkContext;

typedef struct {
  /**
   * @brief Instance of Chunk Context needed for tar decompression.
   */
  Untar_ChunkContext base;

  /**
   * @brief Current zlib context.
   */
  z_stream strm;

  /**
   * @brief Buffer that contains the inflated data.
   */
  void *inflateBuffer;

  /**
   * @brief Size of buffer that contains the inflated data.
   */
  size_t inflateBufferSize;

} Untar_GzChunkContext;

/**
 * @brief Initializes the Untar_ChunkContext files out of a part of a block of
 * memory.
 *
 * @param Untar_ChunkContext *context [in] Pointer to a context structure.
 */
void Untar_ChunkContext_Init(Untar_ChunkContext *context);

/*
 * @brief Rips links, directories and files out of a part of a block of memory.
 *
 * @param Untar_ChunkContext *context [in] Pointer to a context structure.
 * @param void *chunk [in] Pointer to a chunk of a TAR buffer.
 * @param size_t chunk_size [in] Length of the chunk of a TAR buffer.
 *
 * @retval UNTAR_SUCCESSFUL (0)    on successful completion.
 * @retval UNTAR_FAIL              for a faulty step within the process.
 * @retval UNTAR_INVALID_CHECKSUM  for an invalid header checksum.
 * @retval UNTAR_INVALID_HEADER    for an invalid header.
 */

int Untar_FromChunk_Print(
  Untar_ChunkContext *context,
  void *chunk,
  size_t chunk_size,
  const rtems_printer* printer
);

/**************************************************************************
 * This converts octal ASCII number representations into an
 * unsigned long.  Only support 32-bit numbers for now.
 *************************************************************************/
extern unsigned long
_rtems_octal2ulong(const char *octascii, size_t len);

/************************************************************************
 * Compute the TAR checksum and check with the value in
 * the archive.  The checksum is computed over the entire
 * header, but the checksum field is substituted with blanks.
 ************************************************************************/
extern int
_rtems_tar_header_checksum(const char *bufr);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif  /* _RTEMS_UNTAR_H */
