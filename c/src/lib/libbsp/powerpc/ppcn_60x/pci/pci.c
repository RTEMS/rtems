/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 */
#include <bsp.h>
#include <pci.h>

/* SCE 97/4/9
 *
 * Use PCI configuration space access mechanism 1
 *
 * This is the preferred access mechanism and must be used when accessing
 * bridged PCI busses.
 *
 * The address to be written to the PCI_CONFIG_ADDRESS port is constructed
 * thus (the representation below is little endian):
 *
 *  31  30   24 23        16 15        11 10        8 7          2 1   0
 *  ----------------------------------------------------------------------
 * | 1 | Resvd | Bus Number | Dev Number | Fn Number | Reg Number | 0 | 0 |
 *  ----------------------------------------------------------------------
 *
 * On bus 0, the first 'real' device is at Device number 11, the Eagle being
 * device 0. On all other busses, device numbering starts at 0.
 */
/*
 * Normal PCI device numbering on busses other than 0 is such that
 * that the first device (0) is attached to AD16, second (1) to AD17 etc.
 */
#define CONFIG_ADDRESS(Bus, Device, Function, Offset) \
    (0x80000000 | (Bus<<16) | \
    ((Device+(((Bus==0)&&(Device>0)) ? 10 : 0))<<11) | \
    (Function<<8) | \
    (Offset&~0x03))
#define BYTE_LANE_OFFSET(Offset) ((Offset)&0x3)

/*
 * Private data
 */
static unsigned8 ucMaxPCIBus;

/*
 * Public routines
 */
rtems_status_code PCIConfigWrite8(
	unsigned8 ucBusNumber,
	unsigned8 ucSlotNumber,
	unsigned8 ucFunctionNumber,
	unsigned8 ucOffset,
	unsigned8 ucValue
)
{
	ISR_Level Irql;

	/*
	 * Ensure that accesses to the addr/data ports are indivisible
	 */
	_ISR_Disable(Irql);

	/*
	 * Write to the configuration space address register
	 */
	outport_32(PCI_CONFIG_ADDR,
		   CONFIG_ADDRESS(ucBusNumber, ucSlotNumber,
				  ucFunctionNumber, ucOffset));
	/*
	 * Write to the configuration space data register with the appropriate
	 * offset
	 */
	outport_byte(PCI_CONFIG_DATA+BYTE_LANE_OFFSET(ucOffset), ucValue);

	 _ISR_Enable(Irql);

	return(RTEMS_SUCCESSFUL);
}

rtems_status_code PCIConfigWrite16(
	unsigned8 ucBusNumber,
	unsigned8 ucSlotNumber,
	unsigned8 ucFunctionNumber,
	unsigned8 ucOffset,
	unsigned16 usValue
)
{
	ISR_Level Irql;

	/*
	 * Ensure that accesses to the addr/data ports are indivisible
	 */
	_ISR_Disable(Irql);

	/*
	 * Write to the configuration space address register
	 */
	outport_32(PCI_CONFIG_ADDR,
		   CONFIG_ADDRESS(ucBusNumber, ucSlotNumber,
				  ucFunctionNumber, ucOffset));
	/*
	 * Write to the configuration space data register with the appropriate
	 * offset
	 */
	outport_16(PCI_CONFIG_DATA+BYTE_LANE_OFFSET(ucOffset), usValue);

	_ISR_Enable(Irql);

	return(RTEMS_SUCCESSFUL);
}

rtems_status_code PCIConfigWrite32(
	unsigned8 ucBusNumber,
	unsigned8 ucSlotNumber,
	unsigned8 ucFunctionNumber,
	unsigned8 ucOffset,
	unsigned32 ulValue
)
{
	ISR_Level Irql;

	/*
	 * Ensure that accesses to the addr/data ports are indivisible
	 */
	_ISR_Disable(Irql);

	/*
	 * Write to the configuration space address register
	 */
	outport_32(PCI_CONFIG_ADDR,
		   CONFIG_ADDRESS(ucBusNumber, ucSlotNumber,
				  ucFunctionNumber, ucOffset));
	/*
	 * Write to the configuration space data register with the appropriate
	 * offset
	 */
	outport_32(PCI_CONFIG_DATA, ulValue);

	_ISR_Enable(Irql);

	return(RTEMS_SUCCESSFUL);
}

rtems_status_code PCIConfigRead8(
	unsigned8 ucBusNumber,
	unsigned8 ucSlotNumber,
	unsigned8 ucFunctionNumber,
	unsigned8 ucOffset,
	unsigned8 *pucValue
)
{
	ISR_Level Irql;

	/*
	 * Ensure that accesses to the addr/data ports are indivisible
	 */
	_ISR_Disable(Irql);

	/*
	 * Write to the configuration space address register
	 */
	outport_32(PCI_CONFIG_ADDR,
		   CONFIG_ADDRESS(ucBusNumber, ucSlotNumber,
				  ucFunctionNumber, ucOffset));
	/*
	 * Read from the configuration space data register with the appropriate
	 * offset
	 */
	inport_byte(PCI_CONFIG_DATA+BYTE_LANE_OFFSET(ucOffset), *pucValue);

	_ISR_Enable(Irql);

	return(RTEMS_SUCCESSFUL);
}

rtems_status_code PCIConfigRead16(
	unsigned8 ucBusNumber,
	unsigned8 ucSlotNumber,
	unsigned8 ucFunctionNumber,
	unsigned8 ucOffset,
	unsigned16 *pusValue
)
{
	ISR_Level Irql;

	/*
	 * Ensure that accesses to the addr/data ports are indivisible
	 */
	_ISR_Disable(Irql);

	/*
	 * Write to the configuration space address register
	 */
	outport_32(PCI_CONFIG_ADDR,
		   CONFIG_ADDRESS(ucBusNumber, ucSlotNumber,
				  ucFunctionNumber, ucOffset));
	/*
	 * Read from the configuration space data register with the appropriate
	 * offset
	 */
	inport_16(PCI_CONFIG_DATA+BYTE_LANE_OFFSET(ucOffset), *pusValue);

	_ISR_Enable(Irql);

	return(RTEMS_SUCCESSFUL);
}

rtems_status_code PCIConfigRead32(
	unsigned8 ucBusNumber,
	unsigned8 ucSlotNumber,
	unsigned8 ucFunctionNumber,
	unsigned8 ucOffset,
	unsigned32 *pulValue
)
{
	ISR_Level Irql;

	/*
	 * Ensure that accesses to the addr/data ports are indivisible
	 */
	_ISR_Disable(Irql);

	/*
	 * Write to the configuration space address register
	 */
	outport_32(PCI_CONFIG_ADDR,
		   CONFIG_ADDRESS(ucBusNumber, ucSlotNumber,
				  ucFunctionNumber, ucOffset));
	/*
	 * Read from the configuration space data register with the appropriate
	 * offset
	 */
	inport_32(PCI_CONFIG_DATA, *pulValue);

	_ISR_Enable(Irql);

	return(RTEMS_SUCCESSFUL);
}

/*
 * This routine determines the maximum bus number in the system
 */
void InitializePCI()
{
	unsigned8 ucSlotNumber, ucFnNumber, ucNumFuncs;
	unsigned8 ucHeader;
	unsigned8 ucBaseClass, ucSubClass, ucMaxSubordinate;
	unsigned32 ulDeviceID;

	/*
	 * Scan PCI bus 0 looking for PCI-PCI bridges
	 */
	for(ucSlotNumber=0;ucSlotNumber<PCI_MAX_DEVICES;ucSlotNumber++)
	{
		(void)PCIConfigRead32(0,
				      ucSlotNumber,
				      0,
				      PCI_CONFIG_VENDOR_LOW,
				      &ulDeviceID);
		if(ulDeviceID==PCI_INVALID_VENDORDEVICEID)
		{
			/*
			 * This slot is empty
			 */
			continue;
		}
		(void)PCIConfigRead8(0,
				     ucSlotNumber,
				     0,
				     PCI_CONFIG_HEADER_TYPE,
				     &ucHeader);
		if(ucHeader&PCI_MULTI_FUNCTION)
		{
			ucNumFuncs=PCI_MAX_FUNCTIONS;
		}
		else
		{
			ucNumFuncs=1;
		}
		for(ucFnNumber=0;ucFnNumber<ucNumFuncs;ucFnNumber++)
		{
			(void)PCIConfigRead32(0,
					      ucSlotNumber,
					      ucFnNumber,
					      PCI_CONFIG_VENDOR_LOW,
					      &ulDeviceID);
			if(ulDeviceID==PCI_INVALID_VENDORDEVICEID)
			{
				/*
				 * This slot/function is empty
				 */
				continue;
			}

			/*
			 * This slot/function has a device fitted.
			 */
			(void)PCIConfigRead8(0,
					     ucSlotNumber,
					     ucFnNumber,
					     PCI_CONFIG_CLASS_CODE_U,
					     &ucBaseClass);
			(void)PCIConfigRead8(0,
					     ucSlotNumber,
					     ucFnNumber,
					     PCI_CONFIG_CLASS_CODE_M,
					     &ucSubClass);
			if((ucBaseClass==PCI_BASE_CLASS_BRIDGE) &&
			   (ucSubClass==PCI_SUB_CLASS_BRIDGE_PCI))
			{
				/*
				 * We have found a PCI-PCI bridge
				 */
				(void)PCIConfigRead8(0,
						     ucSlotNumber,
						     ucFnNumber,
						     PCI_BRIDGE_SUBORDINATE_BUS,
						     &ucMaxSubordinate);
				if(ucMaxSubordinate>ucMaxPCIBus)
				{
					ucMaxPCIBus=ucMaxSubordinate;
				}
			}
		}
	}
}

/*
 * Return the number of PCI busses in the system
 */
unsigned8 BusCountPCI()
{
	return(ucMaxPCIBus+1);
}
