/*
 * empty file. Just because referenced by portable application layer.
 */
#ifndef LIBBSP_POWERPC_MCP750_BSP_H
#define LIBBSP_POWERPC_MCP750_BSP_H

#include <rtems.h>
#include <console.h>
#include <libcpu/io.h>
#include <clockdrv.h>

#ifndef ASM
#define outport_byte(port,value) outb(value,port)
#define outport_word(port,value) outw(value,port)
#define outport_long(port,value) outl(value,port)

#define inport_byte(port,value) (value = inb(port))
#define inport_word(port,value) (value = inw(port))
#define inport_long(port,value) (value = inl(port))
/*
 * Vital Board data Start using DATA RESIDUAL
 */
/*
 * Total memory using RESIDUAL DATA
 */
unsigned int BSP_mem_size;
/*
 * PCI Bus Frequency
 */
unsigned int BSP_bus_frequency;
/*
 * processor clock frequency
 */
unsigned int BSP_processor_frequency;
/*
 * Time base divisior (how many tick for 1 second).
 */
unsigned int BSP_time_base_divisor;

extern rtems_configuration_table  BSP_Configuration;
extern void BSP_panic(char *s);
extern int printk(const char *, ...) __attribute__((format(printf, 1, 2)));
#endif

#endif
