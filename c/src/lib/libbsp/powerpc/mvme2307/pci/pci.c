#include <bsp.h>

static const void *PciConfAddr = IO_TO_LOCAL(0x0cf8);
static const void *PciConfData = IO_TO_LOCAL(0x0cfc);

int pci_make_devsig(int bus, int device, int function) {
    return ((bus & 255) << 16) | ((device & 31) << 11) | 
           ((function & 7) << 8) | 0x80000000;
}

int pci_conf_read8(int sig, int off) {
    st_le32(PciConfAddr, sig + (off & 252));
    synchronize_io();
    return *(unsigned char *)(PciConfData + (off & 3));
}

int pci_conf_read16(int sig, int off) {
    st_le32(PciConfAddr, sig + (off & 252));
    synchronize_io();
    return ld_le16(PciConfData + (off & 3));
}

int pci_conf_read32(int sig, int off) {
    st_le32(PciConfAddr, sig + (off & 252));
    synchronize_io();
    return ld_le32(PciConfData);
}

void pci_conf_write8(int sig, int off, unsigned int data) {
    st_le32(PciConfAddr, sig + (off & 252));
    synchronize_io();
    *(unsigned char *)(PciConfData + (off & 3)) = data;
}

void pci_conf_write16(int sig, int off, unsigned int data) {
    st_le32(PciConfAddr, sig + (off & 252));
    synchronize_io();
    st_le16(PciConfData + (off & 3), data);
}

void pci_conf_write32(int sig, int off, unsigned int data) {
    st_le32(PciConfAddr, sig + (off & 252));
    synchronize_io();
    st_le32(PciConfData + (off & 3), data);
}

static int recursive_find(int bus, int devid_venid, int idx, int *count) {
    int devsig, dev, func, max_func, idreg, class, secondary_bus;
    char header_type;

    for (dev = 0; dev < 32; dev++) {
        devsig = pci_make_devsig(bus, dev, 0);
        header_type = pci_conf_read8(devsig, 0x0e);
        max_func = (header_type & 0x80) ? 8 : 1;   /* multi-function device? */
        for (func = 0; func < max_func; func++) {
            devsig = pci_make_devsig(bus, dev, func);
            idreg = pci_conf_read32(devsig, 0x00);
            if (idreg == 0xffffffff) { 
                break;      /* empty slot */
            }
            if (idreg == devid_venid) {
                /* id's match - is it the right instance? */
                if (*count == idx) {
                    return devsig;
                } else {
                    (*count)++;
                }
            }
            class = pci_conf_read32(devsig, 0x08) & 0xffff0000;
            if (class == 0x06040000) {      /* pci-pci bridge? */
                secondary_bus = pci_conf_read8(devsig, 0x19);
                devsig = recursive_find(secondary_bus, devid_venid, idx, count);
                if (devsig != PCI_NOT_FOUND) {
                    return devsig;
                }
            }
        }
    }
    return PCI_NOT_FOUND;
}

int pci_find_by_devid(int vendorId, int devId, int idx) {
    int count = 0;
    int devid_venid = ((devId << 16) & 0xffff0000) | (vendorId & 0xffff);

    return recursive_find(0, devid_venid, idx, &count);
}
