/**************************************************************************************************
*			file: gpio.h
*			
*			date: 2013.2.28 Manley
**************************************************************************************************/
#ifndef	__GPIO_H
#define	__GPIO_H

#define   __I     volatile const     /*!< Defines 'read only' permissions                 */
#define   __O     volatile           /*!< Defines 'write only' permissions                */
#define   __IO    volatile           /*!< Defines 'read / write' permissions              */

/** GPIO - Register Layout Typedef */
typedef struct {
  __IO unsigned int pdor;                              /**< Port Data Output Register, offset: 0x0 */
  __O  unsigned int psor;                              /**< Port Set Output Register, offset: 0x4 */
  __O  unsigned int pcor;                              /**< Port Clear Output Register, offset: 0x8 */
  __O  unsigned int ptor;                              /**< Port Toggle Output Register, offset: 0xC */
  __I  unsigned int pdir;                              /**< Port Data Input Register, offset: 0x10 */
  __IO unsigned int pddr;                              /**< Port Data Direction Register, offset: 0x14 */
} kinetis_gpio_t;

/* PDOR Bit Fields */
#define GPIO_PDOR_PDO_MASK                       0xFFFFFFFFu
#define GPIO_PDOR_PDO_SHIFT                      0
#define GPIO_PDOR_PDO(x)                         (((uint32_t)(((uint32_t)(x))<<GPIO_PDOR_PDO_SHIFT))&GPIO_PDOR_PDO_MASK)
/* PSOR Bit Fields */
#define GPIO_PSOR_PTSO_MASK                      0xFFFFFFFFu
#define GPIO_PSOR_PTSO_SHIFT                     0
#define GPIO_PSOR_PTSO(x)                        (((uint32_t)(((uint32_t)(x))<<GPIO_PSOR_PTSO_SHIFT))&GPIO_PSOR_PTSO_MASK)
/* PCOR Bit Fields */
#define GPIO_PCOR_PTCO_MASK                      0xFFFFFFFFu
#define GPIO_PCOR_PTCO_SHIFT                     0
#define GPIO_PCOR_PTCO(x)                        (((uint32_t)(((uint32_t)(x))<<GPIO_PCOR_PTCO_SHIFT))&GPIO_PCOR_PTCO_MASK)
/* PTOR Bit Fields */
#define GPIO_PTOR_PTTO_MASK                      0xFFFFFFFFu
#define GPIO_PTOR_PTTO_SHIFT                     0
#define GPIO_PTOR_PTTO(x)                        (((uint32_t)(((uint32_t)(x))<<GPIO_PTOR_PTTO_SHIFT))&GPIO_PTOR_PTTO_MASK)
/* PDIR Bit Fields */
#define GPIO_PDIR_PDI_MASK                       0xFFFFFFFFu
#define GPIO_PDIR_PDI_SHIFT                      0
#define GPIO_PDIR_PDI(x)                         (((uint32_t)(((uint32_t)(x))<<GPIO_PDIR_PDI_SHIFT))&GPIO_PDIR_PDI_MASK)
/* PDDR Bit Fields */
#define GPIO_PDDR_PDD_MASK                       0xFFFFFFFFu
#define GPIO_PDDR_PDD_SHIFT                      0
#define GPIO_PDDR_PDD(x)                         (((uint32_t)(((uint32_t)(x))<<GPIO_PDDR_PDD_SHIFT))&GPIO_PDDR_PDD_MASK)


/* GPIO - Peripheral instance base addresses */
/** Peripheral PTA base address */
#define PTA_BASE                                 (0x400FF000u)
/** Peripheral PTA base pointer */
#define PTA                                      ((kinetis_gpio_t *)PTA_BASE)
/** Peripheral PTB base address */
#define PTB_BASE                                 (0x400FF040u)
/** Peripheral PTB base pointer */
#define PTB                                      ((kinetis_gpio_t *)PTB_BASE)
/** Peripheral PTC base address */
#define PTC_BASE                                 (0x400FF080u)
/** Peripheral PTC base pointer */
#define PTC                                      ((kinetis_gpio_t *)PTC_BASE)
/** Peripheral PTD base address */
#define PTD_BASE                                 (0x400FF0C0u)
/** Peripheral PTD base pointer */
#define PTD                                      ((kinetis_gpio_t *)PTD_BASE)
/** Peripheral PTE base address */
#define PTE_BASE                                 (0x400FF100u)
/** Peripheral PTE base pointer */
#define PTE                                      ((kinetis_gpio_t *)PTE_BASE)

#define IN 		((unsigned int)0x00000000)
#define OUT 	((unsigned int)0x00000001)

#define IO_0 	((unsigned int)0x00000001)
#define	IO_1 	((unsigned int)0x00000002)
#define	IO_2 	((unsigned int)0x00000004)
#define	IO_3 	((unsigned int)0x00000008)
#define	IO_4 	((unsigned int)0x00000010)
#define	IO_5 	((unsigned int)0x00000020)
#define	IO_6 	((unsigned int)0x00000040)
#define	IO_7 	((unsigned int)0x00000080)
#define	IO_8 	((unsigned int)0x00000100)
#define	IO_9 	((unsigned int)0x00000200)
#define	IO_10 	((unsigned int)0x00000400)
#define	IO_11 	((unsigned int)0x00000800)
#define	IO_12 	((unsigned int)0x00001000)
#define	IO_13 	((unsigned int)0x00002000)
#define	IO_14 	((unsigned int)0x00004000)
#define	IO_15 	((unsigned int)0x00008000)
#define	IO_16 	((unsigned int)0x00010000)
#define	IO_17 	((unsigned int)0x00020000)
#define	IO_18 	((unsigned int)0x00040000)
#define	IO_19 	((unsigned int)0x00080000)
#define	IO_20 	((unsigned int)0x00100000)
#define	IO_21 	((unsigned int)0x00200000)
#define	IO_22 	((unsigned int)0x00400000)
#define	IO_23 	((unsigned int)0x00800000)
#define	IO_24 	((unsigned int)0x01000000)
#define	IO_25 	((unsigned int)0x02000000)
#define	IO_26 	((unsigned int)0x04000000)
#define	IO_27 	((unsigned int)0x08000000)
#define	IO_28 	((unsigned int)0x10000000)
#define	IO_29 	((unsigned int)0x20000000)
#define	IO_30 	((unsigned int)0x40000000)
#define	IO_31 	((unsigned int)0x80000000)



static inline void gpio_bit_direction(kinetis_gpio_t *gpio, unsigned int bit_n, unsigned int bit_dir)
{
	(bit_dir == 0) ? (gpio->pddr &= ~bit_n) : (gpio->pddr |= bit_n);
}
static inline void gpio_port_direction(kinetis_gpio_t *gpio, unsigned int port_dir)
{
	gpio->pddr = port_dir;
}
static inline void gpio_set_bit(kinetis_gpio_t *gpio, unsigned int bit_n)
{
	gpio->psor = bit_n;
}
static inline void gpio_clear_bit(kinetis_gpio_t *gpio, unsigned int bit_n)
{
	gpio->pcor = bit_n;
}
static inline unsigned int gpio_read_port(kinetis_gpio_t *gpio)
{
	return (gpio->pdir);
}
static inline unsigned int gpio_read_bit(kinetis_gpio_t *gpio, unsigned int bit_n)
{
	unsigned int ret;	
	ret = ((gpio->pdir & bit_n) == 0) ? (unsigned int)0x0 : (unsigned int)0x1;
	return (ret);
}
static inline void gpio_write_port(kinetis_gpio_t *gpio, unsigned int val)
{
	gpio->pdor = val;
}
static inline void gpio_write_bit (kinetis_gpio_t *gpio, unsigned int bit_n, unsigned int val)
{
	(val == 0) ? (gpio->pdor &= bit_n) : (gpio->pdor |= bit_n);
}
static inline void gpio_toggle_port (kinetis_gpio_t *gpio)
{
	gpio->ptor = (unsigned int)0xFFFFFF;
}
static inline void gpio_toggle_bit (kinetis_gpio_t *gpio, unsigned int bit_n)
{
	gpio->ptor |= bit_n;
}

#endif
