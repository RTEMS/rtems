/*
 * system.h - SOPC Builder system and BSP software package information
 *
 * Machine generated for CPU 'niosv_m_cpu' in SOPC Builder design 'c10lp_rtems'
 * SOPC Builder design path: C:/Temp/Altera/cl10/cyclone10LP_10cl025yu256_eval_v17.0.0stdb595/examples/rtems_vm/c10lp_rtems.sopcinfo
 *
 * Generated: Fri Aug 02 17:55:13 EDT 2024
 */

/*
 * DO NOT MODIFY THIS FILE
 *
 * Changing this file will have subtle consequences
 * which will almost certainly lead to a nonfunctioning
 * system. If you do modify this file, be aware that your
 * changes will be overwritten and lost when this file
 * is generated again.
 *
 * DO NOT MODIFY THIS FILE
 */

/*
 * License Agreement
 *
 * Copyright (c) 2008
 * Altera Corporation, San Jose, California, USA.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State
 * of California and by the laws of the United States of America.
 */

#ifndef __BSP_SYSTEM_VM_H_
#define __BSP_SYSTEM_VM_H_

/*
 * CPU configuration
 *
 */

#define ALT_CPU_ARCHITECTURE "intel_niosv_m"
#define ALT_CPU_CPU_FREQ 100000000u
#define ALT_CPU_DATA_ADDR_WIDTH 0x20
#define ALT_CPU_DCACHE_LINE_SIZE 0
#define ALT_CPU_DCACHE_LINE_SIZE_LOG2 0
#define ALT_CPU_DCACHE_SIZE 0
#define ALT_CPU_FREQ 100000000
#define ALT_CPU_HAS_CSR_SUPPORT 1
#define ALT_CPU_HAS_DEBUG_STUB
#define ALT_CPU_ICACHE_LINE_SIZE 0
#define ALT_CPU_ICACHE_LINE_SIZE_LOG2 0
#define ALT_CPU_ICACHE_SIZE 0
#define ALT_CPU_INST_ADDR_WIDTH 0x20
#define ALT_CPU_MTIME_OFFSET 0x10000100
#define ALT_CPU_NAME "niosv_m_cpu"
#define ALT_CPU_NIOSV_CORE_VARIANT 1
#define ALT_CPU_NUM_GPR 32
#define ALT_CPU_RESET_ADDR 0x10010000
#define ALT_CPU_TICKS_PER_SEC NIOSV_INTERNAL_TIMER_TICKS_PER_SECOND
#define ALT_CPU_TIMER_DEVICE_TYPE 2


/*
 * CPU configuration (with legacy prefix - don't use these anymore)
 *
 */

#define ABBOTTSLAKE_CPU_FREQ 100000000u
#define ABBOTTSLAKE_DATA_ADDR_WIDTH 0x20
#define ABBOTTSLAKE_DCACHE_LINE_SIZE 0
#define ABBOTTSLAKE_DCACHE_LINE_SIZE_LOG2 0
#define ABBOTTSLAKE_DCACHE_SIZE 0
#define ABBOTTSLAKE_HAS_CSR_SUPPORT 1
#define ABBOTTSLAKE_HAS_DEBUG_STUB
#define ABBOTTSLAKE_ICACHE_LINE_SIZE 0
#define ABBOTTSLAKE_ICACHE_LINE_SIZE_LOG2 0
#define ABBOTTSLAKE_ICACHE_SIZE 0
#define ABBOTTSLAKE_INST_ADDR_WIDTH 0x20
#define ABBOTTSLAKE_MTIME_OFFSET 0x10000100
#define ABBOTTSLAKE_NIOSV_CORE_VARIANT 1
#define ABBOTTSLAKE_NUM_GPR 32
#define ABBOTTSLAKE_RESET_ADDR 0x10010000
#define ABBOTTSLAKE_TICKS_PER_SEC NIOSV_INTERNAL_TIMER_TICKS_PER_SECOND
#define ABBOTTSLAKE_TIMER_DEVICE_TYPE 2


/*
 * Define for each module class mastered by the CPU
 *
 */

#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_ONCHIP_MEMORY2
#define __ALTERA_AVALON_PIO
#define __ALTERA_AVALON_SYSID_QSYS
#define __ALTERA_AVALON_TIMER
#define __ALTERA_GENERIC_QUAD_SPI_CONTROLLER2
#define __HYPERBUS_CTRL
#define __INTEL_NIOSV_M
#define __INTERVAL_TIMER


/*
 * System configuration
 *
 */

#define ALT_DEVICE_FAMILY "Cyclone 10 LP"
#define ALT_ENHANCED_INTERRUPT_API_PRESENT
#define ALT_IRQ_BASE NULL
#define ALT_LOG_PORT "/dev/null"
#define ALT_LOG_PORT_BASE 0x0
#define ALT_LOG_PORT_DEV null
#define ALT_LOG_PORT_TYPE ""
#define ALT_NUM_EXTERNAL_INTERRUPT_CONTROLLERS 0
#define ALT_NUM_INTERNAL_INTERRUPT_CONTROLLERS 1
#define ALT_NUM_INTERRUPT_CONTROLLERS 1
#define ALT_STDERR "/dev/jtag_uart"
#define ALT_STDERR_BASE 0x10000208
#define ALT_STDERR_DEV jtag_uart
#define ALT_STDERR_IS_JTAG_UART
#define ALT_STDERR_PRESENT
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN "/dev/jtag_uart"
#define ALT_STDIN_BASE 0x10000208
#define ALT_STDIN_DEV jtag_uart
#define ALT_STDIN_IS_JTAG_UART
#define ALT_STDIN_PRESENT
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT "/dev/jtag_uart"
#define ALT_STDOUT_BASE 0x10000208
#define ALT_STDOUT_DEV jtag_uart
#define ALT_STDOUT_IS_JTAG_UART
#define ALT_STDOUT_PRESENT
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_SYSTEM_NAME "c10lp_rtems"
#define ALT_SYS_CLK_TICKS_PER_SEC ALT_CPU_TICKS_PER_SEC
#define ALT_TIMESTAMP_CLK_TIMER_DEVICE_TYPE ALT_CPU_TIMER_DEVICE_TYPE


/*
 * benchmark_timer configuration
 *
 */

#define ALT_MODULE_CLASS_benchmark_timer interval_timer
#define BENCHMARK_TIMER_BASE 0x10000180
#define BENCHMARK_TIMER_FREQ 100000000
#define BENCHMARK_TIMER_IRQ 6
#define BENCHMARK_TIMER_IRQ_INTERRUPT_CONTROLLER_ID 0
#define BENCHMARK_TIMER_NAME "/dev/benchmark_timer"
#define BENCHMARK_TIMER_SPAN 32
#define BENCHMARK_TIMER_TYPE "interval_timer"


/*
 * epcq_controller_avl_csr configuration
 *
 */

#define ALT_MODULE_CLASS_epcq_controller_avl_csr altera_generic_quad_spi_controller2
#define EPCQ_CONTROLLER_AVL_CSR_BASE 0x10000140
#define EPCQ_CONTROLLER_AVL_CSR_FLASH_TYPE "EPCQ128"
#define EPCQ_CONTROLLER_AVL_CSR_IRQ 1
#define EPCQ_CONTROLLER_AVL_CSR_IRQ_INTERRUPT_CONTROLLER_ID 0
#define EPCQ_CONTROLLER_AVL_CSR_IS_EPCS 0
#define EPCQ_CONTROLLER_AVL_CSR_NAME "/dev/epcq_controller_avl_csr"
#define EPCQ_CONTROLLER_AVL_CSR_NUMBER_OF_SECTORS 256
#define EPCQ_CONTROLLER_AVL_CSR_PAGE_SIZE 256
#define EPCQ_CONTROLLER_AVL_CSR_SECTOR_SIZE 65536
#define EPCQ_CONTROLLER_AVL_CSR_SPAN 64
#define EPCQ_CONTROLLER_AVL_CSR_SUBSECTOR_SIZE 4096
#define EPCQ_CONTROLLER_AVL_CSR_TYPE "altera_generic_quad_spi_controller2"


/*
 * epcq_controller_avl_mem configuration
 *
 */

#define ALT_MODULE_CLASS_epcq_controller_avl_mem altera_generic_quad_spi_controller2
#define EPCQ_CONTROLLER_AVL_MEM_BASE 0x11000000
#define EPCQ_CONTROLLER_AVL_MEM_FLASH_TYPE "EPCQ128"
#define EPCQ_CONTROLLER_AVL_MEM_IRQ -1
#define EPCQ_CONTROLLER_AVL_MEM_IRQ_INTERRUPT_CONTROLLER_ID -1
#define EPCQ_CONTROLLER_AVL_MEM_IS_EPCS 0
#define EPCQ_CONTROLLER_AVL_MEM_NAME "/dev/epcq_controller_avl_mem"
#define EPCQ_CONTROLLER_AVL_MEM_NUMBER_OF_SECTORS 256
#define EPCQ_CONTROLLER_AVL_MEM_PAGE_SIZE 256
#define EPCQ_CONTROLLER_AVL_MEM_SECTOR_SIZE 65536
#define EPCQ_CONTROLLER_AVL_MEM_SPAN 16777216
#define EPCQ_CONTROLLER_AVL_MEM_SUBSECTOR_SIZE 4096
#define EPCQ_CONTROLLER_AVL_MEM_TYPE "altera_generic_quad_spi_controller2"


/*
 * hal2 configuration
 *
 */

#define ALT_MAX_FD 32
#define ALT_SYS_CLK NIOSV_M_CPU
#define ALT_TIMESTAMP_CLK NIOSV_M_CPU
#define INTEL_FPGA_DFL_START_ADDRESS 0xffffffffffffffff
#define INTEL_FPGA_USE_DFL_WALKER 0


/*
 * hyperbus_ctrl_altera_axi4_slave_memory configuration
 *
 */

#define ALT_MODULE_CLASS_hyperbus_ctrl_altera_axi4_slave_memory hyperbus_ctrl
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_MEMORY_BASE 0x0
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_MEMORY_IRQ -1
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_MEMORY_IRQ_INTERRUPT_CONTROLLER_ID -1
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_MEMORY_NAME "/dev/hyperbus_ctrl_altera_axi4_slave_memory"
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_MEMORY_SPAN 268435456
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_MEMORY_TYPE "hyperbus_ctrl"


/*
 * hyperbus_ctrl_altera_axi4_slave_register configuration
 *
 */

#define ALT_MODULE_CLASS_hyperbus_ctrl_altera_axi4_slave_register hyperbus_ctrl
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_REGISTER_BASE 0x10000000
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_REGISTER_IRQ -1
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_REGISTER_IRQ_INTERRUPT_CONTROLLER_ID -1
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_REGISTER_NAME "/dev/hyperbus_ctrl_altera_axi4_slave_register"
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_REGISTER_SPAN 256
#define HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_REGISTER_TYPE "hyperbus_ctrl"


/*
 * intel_niosv_m_hal_driver configuration
 *
 */

#define NIOSV_INTERNAL_TIMER_TICKS_PER_SECOND 1000


/*
 * jtag_uart configuration
 *
 */

#define ALT_MODULE_CLASS_jtag_uart altera_avalon_jtag_uart
#define JTAG_UART_BASE 0x10000208
#define JTAG_UART_IRQ 4
#define JTAG_UART_IRQ_INTERRUPT_CONTROLLER_ID 0
#define JTAG_UART_NAME "/dev/jtag_uart"
#define JTAG_UART_READ_DEPTH 64
#define JTAG_UART_READ_THRESHOLD 8
#define JTAG_UART_SPAN 8
#define JTAG_UART_TYPE "altera_avalon_jtag_uart"
#define JTAG_UART_WRITE_DEPTH 64
#define JTAG_UART_WRITE_THRESHOLD 8


/*
 * led_pio configuration
 *
 */

#define ALT_MODULE_CLASS_led_pio altera_avalon_pio
#define LED_PIO_BASE 0x100001a0
#define LED_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define LED_PIO_BIT_MODIFYING_OUTPUT_REGISTER 1
#define LED_PIO_CAPTURE 0
#define LED_PIO_DATA_WIDTH 5
#define LED_PIO_DO_TEST_BENCH_WIRING 0
#define LED_PIO_DRIVEN_SIM_VALUE 0
#define LED_PIO_EDGE_TYPE "NONE"
#define LED_PIO_FREQ 100000000
#define LED_PIO_HAS_IN 0
#define LED_PIO_HAS_OUT 1
#define LED_PIO_HAS_TRI 0
#define LED_PIO_IRQ -1
#define LED_PIO_IRQ_INTERRUPT_CONTROLLER_ID -1
#define LED_PIO_IRQ_TYPE "NONE"
#define LED_PIO_NAME "/dev/led_pio"
#define LED_PIO_RESET_VALUE 15
#define LED_PIO_SPAN 32
#define LED_PIO_TYPE "altera_avalon_pio"


/*
 * onchip_boot_rom configuration
 *
 */

#define ALT_MODULE_CLASS_onchip_boot_rom altera_avalon_onchip_memory2
#define ONCHIP_BOOT_ROM_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define ONCHIP_BOOT_ROM_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define ONCHIP_BOOT_ROM_BASE 0x10010000
#define ONCHIP_BOOT_ROM_CONTENTS_INFO ""
#define ONCHIP_BOOT_ROM_DUAL_PORT 0
#define ONCHIP_BOOT_ROM_GUI_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_BOOT_ROM_INIT_CONTENTS_FILE "bootloader_niosvc10lp"
#define ONCHIP_BOOT_ROM_INIT_MEM_CONTENT 1
#define ONCHIP_BOOT_ROM_INSTANCE_ID "NONE"
#define ONCHIP_BOOT_ROM_IRQ -1
#define ONCHIP_BOOT_ROM_IRQ_INTERRUPT_CONTROLLER_ID -1
#define ONCHIP_BOOT_ROM_NAME "/dev/onchip_boot_rom"
#define ONCHIP_BOOT_ROM_NON_DEFAULT_INIT_FILE_ENABLED 1
#define ONCHIP_BOOT_ROM_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_BOOT_ROM_READ_DURING_WRITE_MODE "DONT_CARE"
#define ONCHIP_BOOT_ROM_SINGLE_CLOCK_OP 0
#define ONCHIP_BOOT_ROM_SIZE_MULTIPLE 1
#define ONCHIP_BOOT_ROM_SIZE_VALUE 4096
#define ONCHIP_BOOT_ROM_SPAN 4096
#define ONCHIP_BOOT_ROM_TYPE "altera_avalon_onchip_memory2"
#define ONCHIP_BOOT_ROM_WRITABLE 0


/*
 * onchip_ram configuration
 *
 */

#define ALT_MODULE_CLASS_onchip_ram altera_avalon_onchip_memory2
#define ONCHIP_RAM_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define ONCHIP_RAM_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define ONCHIP_RAM_BASE 0x10020000
#define ONCHIP_RAM_CONTENTS_INFO ""
#define ONCHIP_RAM_DUAL_PORT 0
#define ONCHIP_RAM_GUI_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_RAM_INIT_CONTENTS_FILE "c10lp_rtems_onchip_ram"
#define ONCHIP_RAM_INIT_MEM_CONTENT 0
#define ONCHIP_RAM_INSTANCE_ID "NONE"
#define ONCHIP_RAM_IRQ -1
#define ONCHIP_RAM_IRQ_INTERRUPT_CONTROLLER_ID -1
#define ONCHIP_RAM_NAME "/dev/onchip_ram"
#define ONCHIP_RAM_NON_DEFAULT_INIT_FILE_ENABLED 0
#define ONCHIP_RAM_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_RAM_READ_DURING_WRITE_MODE "DONT_CARE"
#define ONCHIP_RAM_SINGLE_CLOCK_OP 0
#define ONCHIP_RAM_SIZE_MULTIPLE 1
#define ONCHIP_RAM_SIZE_VALUE 8192
#define ONCHIP_RAM_SPAN 8192
#define ONCHIP_RAM_TYPE "altera_avalon_onchip_memory2"
#define ONCHIP_RAM_WRITABLE 1


/*
 * sys_id configuration
 *
 */

#define ALT_MODULE_CLASS_sys_id altera_avalon_sysid_qsys
#define SYS_ID_BASE 0x10000200
#define SYS_ID_ID 405222982
#define SYS_ID_IRQ -1
#define SYS_ID_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SYS_ID_NAME "/dev/sys_id"
#define SYS_ID_SPAN 8
#define SYS_ID_TIMESTAMP 1722632857
#define SYS_ID_TYPE "altera_avalon_sysid_qsys"


/*
 * user_dipsw configuration
 *
 */

#define ALT_MODULE_CLASS_user_dipsw altera_avalon_pio
#define USER_DIPSW_BASE 0x100001f0
#define USER_DIPSW_BIT_CLEARING_EDGE_REGISTER 0
#define USER_DIPSW_BIT_MODIFYING_OUTPUT_REGISTER 0
#define USER_DIPSW_CAPTURE 0
#define USER_DIPSW_DATA_WIDTH 4
#define USER_DIPSW_DO_TEST_BENCH_WIRING 0
#define USER_DIPSW_DRIVEN_SIM_VALUE 0
#define USER_DIPSW_EDGE_TYPE "NONE"
#define USER_DIPSW_FREQ 100000000
#define USER_DIPSW_HAS_IN 1
#define USER_DIPSW_HAS_OUT 0
#define USER_DIPSW_HAS_TRI 0
#define USER_DIPSW_IRQ -1
#define USER_DIPSW_IRQ_INTERRUPT_CONTROLLER_ID -1
#define USER_DIPSW_IRQ_TYPE "NONE"
#define USER_DIPSW_NAME "/dev/user_dipsw"
#define USER_DIPSW_RESET_VALUE 0
#define USER_DIPSW_SPAN 16
#define USER_DIPSW_TYPE "altera_avalon_pio"


/*
 * user_pb configuration
 *
 */

#define ALT_MODULE_CLASS_user_pb altera_avalon_pio
#define USER_PB_BASE 0x100001e0
#define USER_PB_BIT_CLEARING_EDGE_REGISTER 0
#define USER_PB_BIT_MODIFYING_OUTPUT_REGISTER 0
#define USER_PB_CAPTURE 0
#define USER_PB_DATA_WIDTH 4
#define USER_PB_DO_TEST_BENCH_WIRING 0
#define USER_PB_DRIVEN_SIM_VALUE 0
#define USER_PB_EDGE_TYPE "NONE"
#define USER_PB_FREQ 100000000
#define USER_PB_HAS_IN 1
#define USER_PB_HAS_OUT 0
#define USER_PB_HAS_TRI 0
#define USER_PB_IRQ -1
#define USER_PB_IRQ_INTERRUPT_CONTROLLER_ID -1
#define USER_PB_IRQ_TYPE "NONE"
#define USER_PB_NAME "/dev/user_pb"
#define USER_PB_RESET_VALUE 0
#define USER_PB_SPAN 16
#define USER_PB_TYPE "altera_avalon_pio"


/*
 * watchdog_timer configuration
 *
 */

#define ALT_MODULE_CLASS_watchdog_timer altera_avalon_timer
#define WATCHDOG_TIMER_ALWAYS_RUN 1
#define WATCHDOG_TIMER_BASE 0x100001c0
#define WATCHDOG_TIMER_COUNTER_SIZE 32
#define WATCHDOG_TIMER_FIXED_PERIOD 1
#define WATCHDOG_TIMER_FREQ 100000000
#define WATCHDOG_TIMER_IRQ 5
#define WATCHDOG_TIMER_IRQ_INTERRUPT_CONTROLLER_ID 0
#define WATCHDOG_TIMER_LOAD_VALUE 99
#define WATCHDOG_TIMER_MULT 1.0E-6
#define WATCHDOG_TIMER_NAME "/dev/watchdog_timer"
#define WATCHDOG_TIMER_PERIOD 1
#define WATCHDOG_TIMER_PERIOD_UNITS "us"
#define WATCHDOG_TIMER_RESET_OUTPUT 1
#define WATCHDOG_TIMER_SNAPSHOT 0
#define WATCHDOG_TIMER_SPAN 32
#define WATCHDOG_TIMER_TICKS_PER_SEC 1000000
#define WATCHDOG_TIMER_TIMEOUT_PULSE_OUTPUT 0
#define WATCHDOG_TIMER_TIMER_DEVICE_TYPE 1
#define WATCHDOG_TIMER_TYPE "altera_avalon_timer"

#endif /* __BSP_SYSTEM_VM_H_ */
