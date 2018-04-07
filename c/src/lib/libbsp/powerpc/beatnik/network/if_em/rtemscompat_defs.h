#ifndef RTEMS_COMPAT_DEFS_H
#define RTEMS_COMPAT_DEFS_H

/* Number of device instances the driver should support
 * - may be limited to 1 depending on IRQ API
 * (braindamaged PC586 and powerpc)
 */
#define NETDRIVER_SLOTS 1

/* String name to print with error messages */
#define NETDRIVER       "em"
/* Name snippet used to make global symbols unique to this driver */
#define NETDRIVER_PREFIX em

#define adapter			em_softc
#define interface_data	arpcom

/* Define according to endianness of the *ethernet*chip*
 * (not the CPU - most probably are LE)
 * This must be either NET_CHIP_LE or NET_CHIP_BE
 */

#define NET_CHIP_LE
#undef  NET_CHIP_BE

/* Define either NET_CHIP_MEM_IO or NET_CHIP_PORT_IO,
 * depending whether the CPU sees it in memory address space
 * or (e.g. x86) uses special I/O instructions.
 */
#define NET_CHIP_MEM_IO
#undef  NET_CHIP_PORT_IO

/* The name of the hijacked 'bus handle' field in the softc
 * structure. We use this field to store the chip's base address.
 */
#define NET_SOFTC_BHANDLE_FIELD osdep.mem_bus_space_handle

/* define the names of the 'if_XXXreg.h' and 'if_XXXvar.h' headers
 * (only if present, i.e., if the BSDNET driver has no respective
 * header, leave this undefined).
 *
 */
#define  IF_REG_HEADER "../if_em/if_em.h"
#undef  IF_VAR_HEADER

/* define if a pci device */
#define NETDRIVER_PCI <bsp/pci.h>

/* Macros to disable and enable interrupts, respectively.
 * The 'disable' macro is expanded in the ISR, the 'enable'
 * macro is expanded in the driver task.
 * The global network semaphore usually provides mutex
 * protection of the device registers.
 * Special care must be taken when coding the 'disable' macro,
 * however to MAKE SURE THERE ARE NO OTHER SIDE EFFECTS such
 * as:
 *    - macro must not clear any status flags
 *    - macro must save/restore any context information
 *      (e.g., a address register pointer or a bank switch register)
 *
 * ARGUMENT: the macro arg is a pointer to the driver's 'softc' structure
 */

#define NET_ENABLE_IRQS(sc)	do { \
		E1000_WRITE_REG(&sc->hw, IMS, (IMS_ENABLE_MASK)); \
		} while (0)

#define NET_DISABLE_IRQS(sc)	do { \
		E1000_WRITE_REG(&sc->hw, IMC, 0xffffffff);	\
		} while (0)
		
#define KASSERT(a...) do {} while (0)

/* dmamap stuff; these are defined just to work with the current version
 * of this driver and the implementation must be carefully checked if
 * a newer version is merged.!
 *
 * The more cumbersome routines have been commented in the source, the
 * simpler ones are defined to be NOOPs here so the source gets less
 * cluttered...
 *
 * ASSUMPTIONS:
 *
 *	  -> dmamap_sync cannot sync caches; assume we have HW snooping
 *
 */

typedef unsigned bus_size_t;
typedef unsigned bus_addr_t;

typedef struct {
	unsigned	ds_addr;
	unsigned	ds_len;
} bus_dma_segment_t;

#define bus_dma_tag_destroy(args...) do {} while(0)

#define bus_dmamap_destroy(args...) do {} while(0)

#define bus_dmamap_unload(args...) do {} while (0)

#ifdef __PPC__
#define bus_dmamap_sync(args...) do { __asm__ volatile("sync":::"memory"); } while (0)
#else
#define bus_dmamap_sync(args...) do {} while (0)
#endif

#define BUS_DMA_NOWAIT		0xdeadbeef	/* unused */

#define em_adapter_list _bsd_em_adapter_list
#define em_arc_subsystem_valid _bsd_em_arc_subsystem_valid
#define em_check_downshift _bsd_em_check_downshift
#define em_check_for_link _bsd_em_check_for_link
#define em_check_mng_mode _bsd_em_check_mng_mode
#define em_check_phy_reset_block _bsd_em_check_phy_reset_block
#define em_check_polarity _bsd_em_check_polarity
#define em_cleanup_led _bsd_em_cleanup_led
#define em_clear_hw_cntrs _bsd_em_clear_hw_cntrs
#define em_clear_vfta _bsd_em_clear_vfta
#define em_commit_shadow_ram _bsd_em_commit_shadow_ram
#define em_config_collision_dist _bsd_em_config_collision_dist
#define em_config_dsp_after_link_change _bsd_em_config_dsp_after_link_change
#define em_config_fc_after_link_up _bsd_em_config_fc_after_link_up
#define em_dbg_config _bsd_em_dbg_config
#define em_detect_gig_phy _bsd_em_detect_gig_phy
#define em_disable_pciex_master _bsd_em_disable_pciex_master
#define em_display_debug_stats _bsd_em_display_debug_stats
#define em_driver_version _bsd_em_driver_version
#define em_enable_mng_pass_thru _bsd_em_enable_mng_pass_thru
#define em_enable_pciex_master _bsd_em_enable_pciex_master
#define em_enable_tx_pkt_filtering _bsd_em_enable_tx_pkt_filtering
#define em_force_mac_fc _bsd_em_force_mac_fc
#define em_get_auto_rd_done _bsd_em_get_auto_rd_done
#define em_get_bus_info _bsd_em_get_bus_info
#define em_get_cable_length _bsd_em_get_cable_length
#define em_get_hw_eeprom_semaphore _bsd_em_get_hw_eeprom_semaphore
#define em_get_phy_cfg_done _bsd_em_get_phy_cfg_done
#define em_get_speed_and_duplex _bsd_em_get_speed_and_duplex
#define em_hash_mc_addr _bsd_em_hash_mc_addr
#define em_hw_early_init _bsd_em_hw_early_init
#define em_id_led_init _bsd_em_id_led_init
#define em_init_eeprom_params _bsd_em_init_eeprom_params
#define em_init_hw _bsd_em_init_hw
#define em_init_rx_addrs _bsd_em_init_rx_addrs
#define em_io_read _bsd_em_io_read
#define em_io_write _bsd_em_io_write
#define em_is_onboard_nvm_eeprom _bsd_em_is_onboard_nvm_eeprom
#define em_led_off _bsd_em_led_off
#define em_led_on _bsd_em_led_on
#define em_mc_addr_list_update _bsd_em_mc_addr_list_update
#define em_mng_enable_host_if _bsd_em_mng_enable_host_if
#define em_mng_host_if_write _bsd_em_mng_host_if_write
#define em_mng_write_cmd_header _bsd_em_mng_write_cmd_header
#define em_mng_write_commit _bsd_em_mng_write_commit
#define em_mng_write_dhcp_info _bsd_em_mng_write_dhcp_info
#define em_mta_set _bsd_em_mta_set
#define em_pci_clear_mwi _bsd_em_pci_clear_mwi
#define em_pci_set_mwi _bsd_em_pci_set_mwi
#define em_phy_get_info _bsd_em_phy_get_info
#define em_phy_hw_reset _bsd_em_phy_hw_reset
#define em_phy_igp_get_info _bsd_em_phy_igp_get_info
#define em_phy_m88_get_info _bsd_em_phy_m88_get_info
#define em_phy_reset _bsd_em_phy_reset
#define em_phy_setup_autoneg _bsd_em_phy_setup_autoneg
#define em_poll_eerd_eewr_done _bsd_em_poll_eerd_eewr_done
#define em_put_hw_eeprom_semaphore _bsd_em_put_hw_eeprom_semaphore
#define em_rar_set _bsd_em_rar_set
#define em_read_eeprom _bsd_em_read_eeprom
#define em_read_eeprom_eerd _bsd_em_read_eeprom_eerd
#define em_read_mac_addr _bsd_em_read_mac_addr
#define em_read_part_num _bsd_em_read_part_num
#define em_read_pci_cfg _bsd_em_read_pci_cfg
#define em_read_phy_reg _bsd_em_read_phy_reg
#define em_read_reg_io _bsd_em_read_reg_io
#define em_reset_adaptive _bsd_em_reset_adaptive
#define em_reset_hw _bsd_em_reset_hw
#define em_set_d0_lplu_state _bsd_em_set_d0_lplu_state
#define em_set_d3_lplu_state _bsd_em_set_d3_lplu_state
#define em_set_mac_type _bsd_em_set_mac_type
#define em_set_media_type _bsd_em_set_media_type
#define em_set_pci_express_master_disable _bsd_em_set_pci_express_master_disable
#define em_setup_led _bsd_em_setup_led
#define em_setup_link _bsd_em_setup_link
#define em_tbi_adjust_stats _bsd_em_tbi_adjust_stats
#define em_update_adaptive _bsd_em_update_adaptive
#define em_update_eeprom_checksum _bsd_em_update_eeprom_checksum
#define em_validate_eeprom_checksum _bsd_em_validate_eeprom_checksum
#define em_validate_mdi_setting _bsd_em_validate_mdi_setting
#define em_wait_autoneg _bsd_em_wait_autoneg
#define em_write_eeprom _bsd_em_write_eeprom
#define em_write_eeprom_eewr _bsd_em_write_eeprom_eewr
#define em_write_pci_cfg _bsd_em_write_pci_cfg
#define em_write_phy_reg _bsd_em_write_phy_reg
#define em_write_reg_io _bsd_em_write_reg_io
#define em_write_vfta _bsd_em_write_vfta
#define the_em_devs _bsd_the_em_devs

#endif
