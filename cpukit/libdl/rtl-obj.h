/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
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
 * @brief RTEMS Run-Time Linker Object Support.
 */

#if !defined (_RTEMS_RTL_OBJ_H_)
#define _RTEMS_RTL_OBJ_H_

#include <rtems.h>
#include <rtems/chain.h>
#include <rtems/rtl/rtl-sym.h>
#include <rtems/rtl/rtl-unresolved.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Loader format flags.
 */
#define RTEMS_RTL_FMT_ELF     (1 << 0)
#define RTEMS_RTL_FMT_COMP    (1 << 1)
#define RTEMS_RTL_FMT_PRIVATE (1 << 16)

/**
 * Loader format definition.
 */
typedef struct rtems_rtl_loader_format_s
{
  /**
   * The format label. This can be used to determine and manage
   * specific formats.
   */
  const char* label;

  /**
   * The format flags.
   */
  uint32_t flags;
} rtems_rtl_loader_format_t;

/**
 * The type of the format loader check handler. This handler checks the format
 * and if it is detected as suitable it returns true.
 */
typedef bool (*rtems_rtl_loader_check) (rtems_rtl_obj_t* obj, int fd);

/**
 * The type of the format loader load handler. This handler loads the specific
 * format.
 */
typedef bool (*rtems_rtl_loader_load) (rtems_rtl_obj_t* obj, int fd);

/**
 * The type of the format loader unload handler. This handler unloads the
 * specific format.
 */
typedef bool (*rtems_rtl_loader_unload) (rtems_rtl_obj_t* obj);

/**
 * The type of the format loader signature handler. This handler checks the
 * format signature.
 */
typedef rtems_rtl_loader_format_t* (*rtems_rtl_loader_sig) (void);

/**
 * Table for supported loadable formats.
 */
typedef struct rtems_rtl_loader_table_s
{
  rtems_rtl_loader_check  check;     /**< The check handler. */
  rtems_rtl_loader_load   load;      /**< The loader. */
  rtems_rtl_loader_unload unload;    /**< The unloader. */
  rtems_rtl_loader_sig    signature; /**< The loader's signature. */
} rtems_rtl_loader_table_t;

/**
 * Flags for the various section types.
 */
#define RTEMS_RTL_OBJ_SECT_TEXT  (1 << 0)  /**< Section holds program text. */
#define RTEMS_RTL_OBJ_SECT_CONST (1 << 1)  /**< Section holds program text. */
#define RTEMS_RTL_OBJ_SECT_DATA  (1 << 2)  /**< Section holds program data. */
#define RTEMS_RTL_OBJ_SECT_BSS   (1 << 3)  /**< Section holds program bss. */
#define RTEMS_RTL_OBJ_SECT_EH    (1 << 4)  /**< Section holds exception data. */
#define RTEMS_RTL_OBJ_SECT_REL   (1 << 5)  /**< Section holds relocation records. */
#define RTEMS_RTL_OBJ_SECT_RELA  (1 << 6)  /**< Section holds relocation addend
                                            *   records. */
#define RTEMS_RTL_OBJ_SECT_SYM   (1 << 7)  /**< Section holds symbols. */
#define RTEMS_RTL_OBJ_SECT_STR   (1 << 8)  /**< Section holds strings. */
#define RTEMS_RTL_OBJ_SECT_ALLOC (1 << 9)  /**< Section allocates runtime memory. */
#define RTEMS_RTL_OBJ_SECT_LOAD  (1 << 10) /**< Section is loaded from object file. */
#define RTEMS_RTL_OBJ_SECT_WRITE (1 << 11) /**< Section is writable, ie data. */
#define RTEMS_RTL_OBJ_SECT_EXEC  (1 << 12) /**< Section is executable. */
#define RTEMS_RTL_OBJ_SECT_ZERO  (1 << 13) /**< Section is preset to zero. */
#define RTEMS_RTL_OBJ_SECT_LINK  (1 << 14) /**< Section is link-ordered. */
#define RTEMS_RTL_OBJ_SECT_CTOR  (1 << 15) /**< Section contains constructors. */
#define RTEMS_RTL_OBJ_SECT_DTOR  (1 << 16) /**< Section contains destructors. */
#define RTEMS_RTL_OBJ_SECT_LOCD  (1 << 17) /**< Section has been located. */

/**
 * Section types mask.
 */
#define RTEMS_RTL_OBJ_SECT_TYPES (RTEMS_RTL_OBJ_SECT_TEXT | \
                                  RTEMS_RTL_OBJ_SECT_CONST | \
                                  RTEMS_RTL_OBJ_SECT_DATA | \
                                  RTEMS_RTL_OBJ_SECT_BSS | \
                                  RTEMS_RTL_OBJ_SECT_EH)

/**
 * An object file is made up of sections and the can be more than
 * one of a specific type of sections. All sections and grouped
 * together in memory.
 */
struct rtems_rtl_obj_sect_s
{
  rtems_chain_node node;        /**< The node's link in the chain. */
  int              section;     /**< The section number. */
  const char*      name;        /**< The section's name. */
  size_t           size;        /**< The size of the section in memory. */
  off_t            offset;      /**< Offset into the object file. Relative to
                                 *   the start of the object file. */
  uint32_t         alignment;   /**< Alignment of this section. */
  int              link;        /**< Section link field. */
  int              info;        /**< Secfion info field. */
  uint32_t         flags;       /**< The section's flags. */
  void*            base;        /**< The base address of the section in
                                 *   memory. */
  int              load_order;  /**< Order we load sections. */
};

/**
 * Object file descriptor flags.
 */
#define RTEMS_RTL_OBJ_LOCKED     (1 << 0) /**< Lock the object file so it cannot
                                           *   be unloaded. */
#define RTEMS_RTL_OBJ_UNRESOLVED (1 << 1) /**< The object file has unresolved
                                           *   external symbols. */

/**
 * RTL Object. There is one for each object module loaded plus one for the base
 * kernel image.
 */
struct rtems_rtl_obj_s
{
  rtems_chain_node     link;         /**< The node's link in the chain. */
  uint32_t             flags;        /**< The status of the object file. */
  uint32_t             users;        /**< References to the object file. */
  int                  format;       /**< The format of the object file. */
  const char*          fname;        /**< The file name for the object. */
  const char*          oname;        /**< The object file name. Can be
                                      *   relative. */
  const char*          aname;        /**< The archive name containing the
                                      *   object. NULL means the object is not
                                      *   in a lib */
  off_t                ooffset;      /**< The object offset in the archive. */
  size_t               fsize;        /**< Size of the object file. */
  rtems_chain_control  sections;     /**< The sections of interest in the
                                      *   object file. */
  rtems_rtl_obj_sym_t* local_table;  /**< Local symbol table. */
  size_t               local_syms;   /**< Local symbol count. */
  size_t               local_size;   /**< Local symbol memory usage. */
  rtems_rtl_obj_sym_t* global_table; /**< Global symbol table. */
  size_t               global_syms;  /**< Global symbol count. */
  size_t               global_size;  /**< Global symbol memory usage. */
  uint32_t             unresolved;   /**< The number of unresolved relocations. */
  void*                text_base;    /**< The base address of the text section
                                      *   in memory. */
  size_t               text_size;     /**< The size of the text section. */
  void*                const_base;   /**< The base address of the const section
                                      *   in memory. */
  void*                eh_base;      /**< The base address of the eh section
                                      *   in memory. */
  size_t               eh_size;      /**< The size of the eh section. */
  void*                data_base;    /**< The base address of the data section
                                      *   in memory. */
  void*                bss_base;     /**< The base address of the bss section
                                      *   in memory. */
  size_t               bss_size;     /**< The size of the bss section. */
  size_t               exec_size;    /**< The amount of executable memory
                                      *   allocated */
  void*                entry;        /**< The entry point of the module. */
  uint32_t             checksum;     /**< The checksum of the text sections. A
                                      *   zero means do not checksum. */
  uint32_t*            sec_num;      /**< The sec nums of each obj. */
  uint32_t             obj_num;      /**< The count of elf files in an rtl obj. */
  struct link_map*     linkmap;      /**< For GDB. */
  void*                loader;       /**< The file details specific to a loader. */
};

/**
 * A section handler is called once for each section that needs to be
 * processed by this handler.
 *
 * @param obj The object file's descriptor the section belongs too.
 * @param fd The file descriptor of the object file beling loaded.
 * @param sect The section the handler is being invoked to handle.
 * @param data A user supplied data variable.
 * @retval true The operation was successful.
 * @retval false The operation failed and the RTL has been set.
 */
typedef bool (*rtems_rtl_obj_sect_handler_t)(rtems_rtl_obj_t*      obj,
                                             int                   fd,
                                             rtems_rtl_obj_sect_t* sect,
                                             void*                 data);

/**
 * Get the file name.
 *
 * @param obj The object file.
 * @return const char* The string.
 */
static inline const char* rtems_rtl_obj_fname (const rtems_rtl_obj_t* obj)
{
  return obj->fname;
}

/**
 * Is the file name valid ?
 *
 * @param obj The object file.
 * @return bool There is a file name
 */
static inline bool rtems_rtl_obj_fname_valid (const rtems_rtl_obj_t* obj)
{
  return obj->fname;
}

/**
 * Get the object name.
 *
 * @param obj The object file.
 * @return const char* The string.
 */
static inline const char* rtems_rtl_obj_oname (const rtems_rtl_obj_t* obj)
{
  return obj->oname;
}

/**
 * Is the object name valid ?
 *
 * @param obj The object file.
 * @return bool There is an object name
 */
static inline bool rtems_rtl_obj_oname_valid (const rtems_rtl_obj_t* obj)
{
  return obj->oname;
}

/**
 * Get the archive name.
 *
 * @param obj The object file.
 * @return const char* The string.
 */
static inline const char* rtems_rtl_obj_aname (const rtems_rtl_obj_t* obj)
{
  return obj->aname;
}

/**
 * Is the archive name valid ?
 *
 * @param obj The object file.
 * @return bool There is an archive name
 */
static inline bool rtems_rtl_obj_aname_valid (const rtems_rtl_obj_t* obj)
{
  return obj->aname;
}

/**
 * Is the address inside the text section?
 *
 * @param obj The object file.
 * @return bool There is an archive name
 */
static inline bool rtems_rtl_obj_text_inside (const rtems_rtl_obj_t* obj,
                                              const void*            address)
{
  return
    (address >= obj->text_base) &&
    (address < (obj->text_base + obj->text_size));
}

/**
 * Allocate an object structure on the heap.
 *
 * @retval NULL No memory for the object.
 */
rtems_rtl_obj_t* rtems_rtl_obj_alloc (void);

/**
 * Free the object structure and related resources.
 *
 * @param obj The object file's descriptor to free.
 * @retval false The object has dependences.
 * @retval true The object has been freed.
 */
bool rtems_rtl_obj_free (rtems_rtl_obj_t* obj);

/**
 * Does the object file have unresolved external references ? If it does the
 * results of executing code is unpredictable.
 *
 * @param obj The object file's descriptor.
 * @retval true The object file has unresolved externals.
 * @retval false The object file has all external references resolved.
 */
bool rtems_rtl_obj_unresolved (rtems_rtl_obj_t* obj);

/**
 * Parses a filename and returns newly allocated strings with the archive name,
 * object name, and the object's offset
 *
 * @param name The filename of the object
 * @param aname Address of a string pointer that holds the archive name
 * @param oname Address of a string pointer that holds the object name
 * @param ooffset Address of an int that holds the object offset
 * @retval true The parsing was successful
 * @retval false The parsing was unsuccessful
 */
bool rtems_rtl_parse_name (const char*  name,
                           const char** aname,
                           const char** oname,
                           off_t*       ooffset);

/**
 * Check of the name matches the object file's object name.
 *
 * @param obj The object file's descriptor.
 * @param name The name to match.
 */
bool rtems_rtl_match_name (rtems_rtl_obj_t* obj, const char* name);

/**
 * Find an object file on disk that matches the name. The object descriptor is
 * fill in with the various parts of a name. A name can have archive, object
 * file and offset components. The search path in the RTL is searched.
 *
 * @param obj The object file's descriptor.
 * @param name The name to locate on disk.
 * @retval true The file has been found.
 * @retval false The file could not be located. The RTL error has been set.
 */
bool rtems_rtl_obj_find_file (rtems_rtl_obj_t* obj, const char* name);

/**
 * Add a section to the object descriptor.
 *
 * @param obj The object file's descriptor.
 * @param section The section's index number.
 * @param name The name of the section.
 * @param size The size of the section in memory.
 * @param offset The offset of the section in the object file.
 * @param alignment The alignment of the section in memory.
 * @param link The section's link field (from the ELF format).
 * @param info The section's info field (from the ELF format).
 * @param flags The section's flags.
 * @retval true The section has been added.
 * @retval false The section has not been added. See the RTL error.
 */
bool rtems_rtl_obj_add_section (rtems_rtl_obj_t* obj,
                                int              section,
                                const char*      name,
                                size_t           size,
                                off_t            offset,
                                uint32_t         alignment,
                                int              link,
                                int              info,
                                uint32_t         flags);

/**
 * Erase the object file descriptor's sections.
 *
 * @param obj The object file's descriptor.
 */
void rtems_rtl_obj_erase_sections (rtems_rtl_obj_t* obj);

/**
 * Find the section given a name.
 *
 * @param obj The object file's descriptor.
 * @param name The name of the section to find.
 * @retval NULL The section was not found.
 * @return rtems_rtl_obj_sect_t* The named section.
 */
rtems_rtl_obj_sect_t* rtems_rtl_obj_find_section (const rtems_rtl_obj_t* obj,
                                                  const char*            name);

/**
 * Find a section given a section's index number.
 *
 * @param obj The object file's descriptor.
 * @param index The section's index to find.
 * @retval NULL The section was not found.
 * @return rtems_rtl_obj_sect_t* The found section.
 */
rtems_rtl_obj_sect_t* rtems_rtl_obj_find_section_by_index (const rtems_rtl_obj_t* obj,
                                                           int                    index);

/**
 * The text section size. Only use once all the sections has been added. It
 * includes alignments between sections that are part of the object's text
 * area. The consts sections are included in this section.
 *
 * @param obj The object file's descriptor.
 * @return size_t The size of the text area of the object file.
 */
size_t rtems_rtl_obj_text_size (const rtems_rtl_obj_t* obj);

/**
 * The text section alignment for the object file. Only use once all the
 * sections has been added. The section alignment is the alignment of the first
 * text type section loaded the text section.
 *
 * You can assume the alignment is a positive integral power of 2 if not 0 or
 * 1. If 0 or 1 then there is no alignment.
 *
 * @param obj The object file's descriptor.
 * @return uint32_t The alignment. Can be 0 or 1 for not aligned or the alignment.
 */
uint32_t rtems_rtl_obj_text_alignment (const rtems_rtl_obj_t* obj);

/**
 * The const section size. Only use once all the sections has been added. It
 * includes alignments between sections that are part of the object's const
 * area. The consts sections are included in this section.
 *
 * @param obj The object file's descriptor.
 * @return size_t The size of the const area of the object file.
 */
size_t rtems_rtl_obj_const_size (const rtems_rtl_obj_t* obj);

/**
 * The const section alignment for the object file. Only use once all the
 * sections has been added. The section alignment is the alignment of the first
 * const type section loaded the const section.
 *
 * You can assume the alignment is a positive integral power of 2 if not 0 or
 * 1. If 0 or 1 then there is no alignment.
 *
 * @param obj The object file's descriptor.
 * @return uint32_t The alignment. Can be 0 or 1 for not aligned or the alignment.
 */
uint32_t rtems_rtl_obj_const_alignment (const rtems_rtl_obj_t* obj);

/**
 * The eh section size. Only use once all the sections has been added. It
 * includes alignments between sections that are part of the object's bss area.
 *
 * @param obj The object file's descriptor.
 * @return size_t The size of the bss area of the object file.
 */
size_t rtems_rtl_obj_eh_size (const rtems_rtl_obj_t* obj);

/**
 * The eh section alignment for the object file. Only use once all the sections
 * has been added. The section alignment is the alignment of the first bss type
 * section loaded the bss section.
 *
 * You can assume the alignment is a positive integral power of 2 if not 0 or
 * 1. If 0 or 1 then there is no alignment.
 *
 * @param obj The object file's descriptor.
 * @return uint32_t The alignment. Can be 0 or 1 for not aligned or the alignment.
 */
uint32_t rtems_rtl_obj_eh_alignment (const rtems_rtl_obj_t* obj);

/**
 * The data section size. Only use once all the sections has been added. It
 * includes alignments between sections that are part of the object's data
 * area.
 *
 * @param obj The object file's descriptor.
 * @return size_t The size of the data area of the object file.
 */
size_t rtems_rtl_obj_data_size (const rtems_rtl_obj_t* obj);

/**
 * The data section alignment for the object file. Only use once all the
 * sections has been added. The section alignment is the alignment of the first
 * data type section loaded the data section.
 *
 * You can assume the alignment is a positive integral power of 2 if not 0 or
 * 1. If 0 or 1 then there is no alignment.
 *
 * @param obj The object file's descriptor.
 * @return uint32_t The alignment. Can be 0 or 1 for not aligned or the alignment.
 */
uint32_t rtems_rtl_obj_data_alignment (const rtems_rtl_obj_t* obj);

/**
 * The bss section size. Only use once all the sections has been added. It
 * includes alignments between sections that are part of the object's bss area.
 *
 * @param obj The object file's descriptor.
 * @return size_t The size of the bss area of the object file.
 */
size_t rtems_rtl_obj_bss_size (const rtems_rtl_obj_t* obj);

/**
 * The bss section alignment for the object file. Only use once all the
 * sections has been added. The section alignment is the alignment of the first
 * bss type section loaded the bss section.
 *
 * You can assume the alignment is a positive integral power of 2 if not 0 or
 * 1. If 0 or 1 then there is no alignment.
 *
 * @param obj The object file's descriptor.
 * @return uint32_t The alignment. Can be 0 or 1 for not aligned or the alignment.
 */
uint32_t rtems_rtl_obj_bss_alignment (const rtems_rtl_obj_t* obj);

/**
 * Relocate the object file. The object file's section are parsed for any
 * relocation type sections.
 *
 * @param obj The object file's descriptor.
 * @param fd The object file's file descriptor.
 * @param handler The object file's format specific relocation handler.
 * @param data User specific data handle.
 * @retval true The object file was relocated.
 * @retval false The relocation failed. The RTL error is set.
 */
bool rtems_rtl_obj_relocate (rtems_rtl_obj_t*             obj,
                             int                          fd,
                             rtems_rtl_obj_sect_handler_t handler,
                             void*                        data);

/**
 * Synchronize caches to make code visible to CPU(s)
 *
 * @param obj The object file's descriptor.
 */
void rtems_rtl_obj_synchronize_cache (rtems_rtl_obj_t*    obj);

/**
 * Relocate an object file's unresolved reference.
 *
 * @param rec The unresolved relocation record.
 * @param sym The unresolved relocation's referenced symbol.
 * @retval true The object file record was relocated.
 * @retval false The relocation failed. The RTL error is set.
 */
bool rtems_rtl_obj_relocate_unresolved (rtems_rtl_unresolv_reloc_t* reloc,
                                        rtems_rtl_obj_sym_t*        sym);

/**
 * Load the symbols from the object file. Only the exported or public symbols
 * are read into memory and held in the global symbol table.
 *
 * @param obj The object file's descriptor.
 * @param fd The object file's file descriptor.
 * @param handler The object file's format specific symbol handler.
 * @param data User specific data handle.
 * @retval true The object file's symbol where loaded.
 * @retval false The symbol loading failed. The RTL error is set.
 */
bool rtems_rtl_obj_load_symbols (rtems_rtl_obj_t*             obj,
                                 int                          fd,
                                 rtems_rtl_obj_sect_handler_t handler,
                                 void*                        data);

/**
 * Load the sections that have been allocated memory in the target. The bss
 * type section does not load any data, it is set to 0. The text and data
 * sections read the detault data from the object file into the target memory.
 *
 * @param obj The object file's descriptor.
 * @param fd The object file's file descriptor.
 * @param handler The object file's format specific load handler.
 * @param data User specific data handle.
 * @retval true The object has been sucessfully loaded.
 * @retval false The load failed. The RTL error has been set.
 */
bool rtems_rtl_obj_load_sections (rtems_rtl_obj_t*             obj,
                                  int                          fd,
                                  rtems_rtl_obj_sect_handler_t handler,
                                  void*                        data);

/**
 * Invoke the constructors the object has. Constructors are a table of pointers
 * to "void (*)(void);" where NULL pointers are skipped. The table's size is
 * taken from the section's size. The objet ELF specific code is responisble
 * for flagging which sections contain constructors.
 *
 * @param obj The object file's descriptor.
 */
void rtems_rtl_obj_run_ctors (rtems_rtl_obj_t* obj);

/**
 * Invoke the destructors the object has. Destructors are a table of pointers
 * to "void (*)(void);" where NULL pointers are skipped. The table's size is
 * taken from the section's size. The objet ELF specific code is responisble
 * for flagging which sections contain destructors.
 *
 * @param obj The object file's descriptor.
 */
void rtems_rtl_obj_run_dtors (rtems_rtl_obj_t* obj);

/**
 * Load the object file, reading all sections into memory, symbols and
 * performing any relocation fixups.
 *
 * @param obj The object file's descriptor.
 * @retval true The object file has been loaded.
 * @retval false The load failed. The RTL error has been set.
 */
bool rtems_rtl_obj_load (rtems_rtl_obj_t* obj);

/**
 * Unload the object file, erasing all symbols and releasing all memory.
 *
 * @param obj The object file's descriptor.
 * @retval true The object file has been unloaded.
 * @retval false The unload failed. The RTL error has been set.
 */
bool rtems_rtl_obj_unload (rtems_rtl_obj_t* obj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
