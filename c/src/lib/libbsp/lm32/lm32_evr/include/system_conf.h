#ifndef __SYSTEM_CONFIG_H_
#define __SYSTEM_CONFIG_H_


#define FPGA_DEVICE_FAMILY    "ECP2M"
#define PLATFORM_NAME         "platform1"
#define USE_PLL               (0)
#define CPU_FREQUENCY         (75000000)


/* FOUND 1 CPU UNIT(S) */

/*
 * CPU Instance LM32 component configuration
 */
#define CPU_NAME "LM32"
#define CPU_EBA (0x04000000)
#define CPU_DIVIDE_ENABLED (1)
#define CPU_SIGN_EXTEND_ENABLED (1)
#define CPU_MULTIPLIER_ENABLED (1)
#define CPU_SHIFT_ENABLED (1)
#define CPU_DEBUG_ENABLED (1)
#define CPU_HW_BREAKPOINTS_ENABLED (0)
#define CPU_NUM_HW_BREAKPOINTS (0)
#define CPU_NUM_WATCHPOINTS (0)
#define CPU_ICACHE_ENABLED (1)
#define CPU_ICACHE_SETS (512)
#define CPU_ICACHE_ASSOC (1)
#define CPU_ICACHE_BYTES_PER_LINE (16)
#define CPU_DCACHE_ENABLED (1)
#define CPU_DCACHE_SETS (512)
#define CPU_DCACHE_ASSOC (1)
#define CPU_DCACHE_BYTES_PER_LINE (16)
#define CPU_DEBA (0x0C000000)
#define CPU_CHARIO_IN        (1)
#define CPU_CHARIO_OUT       (1)
#define CPU_CHARIO_TYPE      "JTAG UART"

/*
 * gpio component configuration
 */
#define GPIO_NAME  "gpio"
#define GPIO_BASE_ADDRESS  (0x80004000)
#define GPIO_SIZE  (128)
#define GPIO_CHARIO_IN        (0)
#define GPIO_CHARIO_OUT       (0)
#define GPIO_ADDRESS_LOCK  (1)
#define GPIO_DISABLE  (0)
#define GPIO_OUTPUT_PORTS_ONLY  (1)
#define GPIO_INPUT_PORTS_ONLY  (0)
#define GPIO_TRISTATE_PORTS  (0)
#define GPIO_BOTH_INPUT_AND_OUTPUT  (0)
#define GPIO_DATA_WIDTH  (4)
#define GPIO_INPUT_WIDTH  (1)
#define GPIO_OUTPUT_WIDTH  (1)
#define GPIO_IRQ_MODE  (0)
#define GPIO_LEVEL  (0)
#define GPIO_EDGE  (0)
#define GPIO_EITHER_EDGE_IRQ  (0)
#define GPIO_POSE_EDGE_IRQ  (0)
#define GPIO_NEGE_EDGE_IRQ  (0)

/*
 * uart component configuration
 */
#define UART_NAME  "uart"
#define UART_BASE_ADDRESS  (0x80006000)
#define UART_SIZE  (128)
#define UART_IRQ (0)
#define UART_CHARIO_IN        (1)
#define UART_CHARIO_OUT       (1)
#define UART_CHARIO_TYPE      "RS-232"
#define UART_ADDRESS_LOCK  (1)
#define UART_DISABLE  (0)
#define UART_MODEM  (0)
#define UART_ADDRWIDTH  (5)
#define UART_DATAWIDTH  (8)
#define UART_BAUD_RATE  (115200)
#define UART_IB_SIZE  (4)
#define UART_OB_SIZE  (4)
#define UART_BLOCK_WRITE  (1)
#define UART_BLOCK_READ  (1)
#define UART_DATA_BITS  (8)
#define UART_STOP_BITS  (1)
#define UART_FIFO  (0)
#define UART_INTERRUPT_DRIVEN  (1)

/*
 * ebr component configuration
 */
#define EBR_NAME  "ebr"
#define EBR_BASE_ADDRESS  (0x04000000)
#define EBR_SIZE  (32768)
#define EBR_IS_READABLE   (1)
#define EBR_IS_WRITABLE   (1)
#define EBR_ADDRESS_LOCK  (1)
#define EBR_DISABLE  (0)
#define EBR_EBR_DATA_WIDTH  (32)
#define EBR_INIT_FILE_NAME  "none"
#define EBR_INIT_FILE_FORMAT  "hex"

/*
 * ts_mac_core component configuration
 */
#define TS_MAC_CORE_NAME  "ts_mac_core"
#define TS_MAC_CORE_BASE_ADDRESS  (0x80008000)
#define TS_MAC_CORE_SIZE  (8192)
#define TS_MAC_CORE_IRQ (2)
#define TS_MAC_CORE_CHARIO_IN        (0)
#define TS_MAC_CORE_CHARIO_OUT       (0)
#define TS_MAC_CORE_ADDRESS_LOCK  (1)
#define TS_MAC_CORE_DISABLE  (0)
#define TS_MAC_CORE_STAT_REGS  (1)
#define TS_MAC_CORE_TXRX_FIFO_DEPTH  (512)
#define TS_MAC_CORE_MIIM_MODULE  (1)
#define TS_MAC_CORE_NGO  "l:/mrf/lattice/crio-lm32/platform1/components/ts_mac_top_v27/ipexpress/ts_mac_core/ts_mac_core.ngo"
#define TS_MAC_CORE_ISPLEVER_PRJ  "l:/mrf/lattice/crio-lm32/criomico.syn"

/*
 * timer0 component configuration
 */
#define TIMER0_NAME  "timer0"
#define TIMER0_BASE_ADDRESS  (0x80002000)
#define TIMER0_SIZE  (128)
#define TIMER0_IRQ (1)
#define TIMER0_CHARIO_IN        (0)
#define TIMER0_CHARIO_OUT       (0)
#define TIMER0_ADDRESS_LOCK  (1)
#define TIMER0_DISABLE  (0)
#define TIMER0_PERIOD_NUM  (20)
#define TIMER0_PERIOD_WIDTH  (32)
#define TIMER0_WRITEABLE_PERIOD  (1)
#define TIMER0_READABLE_SNAPSHOT  (1)
#define TIMER0_START_STOP_CONTROL  (1)
#define TIMER0_WATCHDOG  (0)

/*
 * timer1 component configuration
 */
#define TIMER1_NAME  "timer1"
#define TIMER1_BASE_ADDRESS  (0x8000A000)
#define TIMER1_SIZE  (128)
#define TIMER1_IRQ (3)
#define TIMER1_CHARIO_IN        (0)
#define TIMER1_CHARIO_OUT       (0)
#define TIMER1_ADDRESS_LOCK  (1)
#define TIMER1_DISABLE  (0)
#define TIMER1_PERIOD_NUM  (20)
#define TIMER1_PERIOD_WIDTH  (32)
#define TIMER1_WRITEABLE_PERIOD  (1)
#define TIMER1_READABLE_SNAPSHOT  (1)
#define TIMER1_START_STOP_CONTROL  (1)
#define TIMER1_WATCHDOG  (0)

/*
 * ddr2_sdram component configuration
 */
#define DDR2_SDRAM_NAME  "ddr2_sdram"
#define DDR2_SDRAM_BASE_ADDRESS  (0x08000000)
#define DDR2_SDRAM_SIZE  (33554432)
#define DDR2_SDRAM_IS_READABLE   (1)
#define DDR2_SDRAM_IS_WRITABLE   (1)
#define DDR2_SDRAM_BST_CNT_READ  (1)
#define DDR2_SDRAM_ADDRESS_LOCK  (1)
#define DDR2_SDRAM_DISABLE  (0)
#define DDR2_SDRAM_NGO  "L:/mrf/lattice/cRIO-LM32/platform1/components/wb_ddr2_ctl_v65/ipexpress/ddr2_sdram/ddr2_sdram.ngo"
#define DDR2_SDRAM_ISPLEVER_PRJ  "l:/mrf/lattice/crio-lm32/criomico.syn"
#define DDR2_SDRAM_PARAM_FILE  "ddr_p_eval/$/src/params/ddr_sdram_mem_params.v"
#define DDR2_SDRAM_MEM_TOP  "ddr_p_eval/$/src/rtl/top/@/ddr_sdram_mem_top.v"


#endif /* __SYSTEM_CONFIG_H_ */
