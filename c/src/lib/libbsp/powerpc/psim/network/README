PSIM NETWORKING HOWTO
=====================

IMPLEMENTATION INFORMATION

1) NIC hardware emulation.

   In order to simulate networking the simulator (PSIM in our case)
   has to emulate some networking hardware.
   At the time of this writing (2009/09 - gdb-6.8) no such emulation
   is available.
   However, a patch has been created which adds this functionality
   to PSIM (see diff in this directory). Unfortunately, implementing
   a network chip (or some other sort of 'data-source') in PSIM is
   not quite that simple since PSIM is at the end of the day a single-
   threaded, monolithic application which has no built-in support for
   external asynchronous events (e.g., NIC packet reception or a
   character arriving at a UART [the PSIM BSP uses a polled console
   driver]).
   In order to add such asynchronous support, the 'async_io' module
   was added to PSIM. 'async_io' uses OS-support in the form of
   a signal (SIGIO) that the OS sends to the PSIM process when I/O
   becomes possible. The signal handler then executes a callback
   which e.g., may schedule a PSIM interrupt.
   However, the use of SIGIO and the O_ASYNC fcntl-flag is not portable
   (BSD and linux only).
   The 'ethtap' NIC emulation uses another not quite portable OS
   service -- the host OS' TUN/TAP device which is some sort of
   pipe with a networking-interface on one end and a file-system
   interface on the other. The 'ethtap' NIC reads/writes packets
   to the file-system interface and they then become available
   to the host OS' networking.
   This ascii-art shows how a RTEMS application inside PSIM can
   communicate with an application on the host with both using
   sockets. (If the host sets up proper routing table entries
   then the RTEMS APP can even communicate with the internet...)


      RTEMS APP                                       HOST APP
          |                                               |
     .............                                   ............
     . <socket>  .                                   .<socket>  .
     .RTEMS BSD  .                                   . HOST OS  .
     .networking .                                   .networking.
     .............                                   ............
     
   .......................                       .....................
   .RTEMS BSD IF "ifsim1".                       . HOST OS IF: "tap0".
   .e.g., 10.0.0.100     .                       . e.g., 10.0.0.1    .
   .......................                       .....................
                                                           o
     .............                                         o
     .RTEMS ifsim.                                         o
     . driver    .                                         o
     .............                                         o
          ||                                               o
          ||                                               o
     ---------------                                       o
       ethtap                                              o
       hw emulation                                        o
     ---------------                                       o
           ^                                               o
           |                                               o
           -----> /dev/net/tun (special file on host OS) ooo
                                                     
                                                   
2) Device-tree. Once PSIM supports the 'ethtap' device then it
   must be added to the device tree. The following properties are
   relevant (The register addresses must match with what the
   BSP/if_sim expects):

      #### ETHTAP @ 0x0c100020 for 0x40
      #
      
      /ethtap@0x0c100020/reg           0x0c100020 0x40
      # route interrupt to open-pic
      /ethtap@0x0c100020               > 0 irq0 /opic@0x0c130000
      # 'tun' device on host
      /ethtap@0x0c100020/tun-device    "/dev/net/tun"
      # name of 'tap' device to use
      /ethtap@0x0c100020/tap-ifname    "tap0"
      # ethernet address of simulated IF
      /ethtap@0x0c100020/hw-address    "00:00:00:22:11:00"
      # generate CRC and append to received packet before
      # handing over to the simulation. This is mostly for
      # debugging the rtems device driver. If unsure, leave 'false'.
      /ethtap@0x0c100020/enable-crc    false

   The 'tun-device' and 'tap-ifname' properties allow you to
   configure the name of the special-file and the 'tap' interface
   on the host.

3) RTEMS driver. The 'if_sim' driver implements a driver for
   the 'ethtap' device.

USAGE INFORMATION

1) Configure application for networking; the 
     RTEMS_BSP_NETWORK_DRIVER_NAME   is "ifsim1"
   and
     RTEMS_BSP_NETWORK_DRIVER_ATTACH is rtems_ifsim_attach

2) Patch, configure (--target=powerpc-rtems) and build
   gdb-6.8. As already mentioned, the NIC emulation only
   is available if your host-os is linux.

3) Create a 'device-tree' file. The BSP build process produces
   a shell-script 'psim' residing in

      <bsp_installdir>/powerpc-rtems/psim/tests/

   which can be used for generating a device-tree file.

   Call 'psim -d -n <application>'. The '-n' option adds the
   emulated interface (the lines above) to the device tree.
   The resulting file is saved as <application>.device.

   The 'psim' script can also be used to launch an application
   directly -- just omit the '-d' option.

4) Linux host network configuration:
   Create a 'permanent' 'tap' interface. This allows you
   to use 'psim' w/o special privileges (the 'tunctl' command
   still must be executed by the super-user).

      sudo tunctl -u <uid-of-user-running-psim>

   You now can configure the 'tap0' interface:

      sudo ifconfig tap0 10.0.0.1 up

   and e.g., run a BOOTP server to provide RTEMS with its
   network configuration:

      sudo dhcpd3 -d tap0

   Assuming that BOOTP gives the RTEMS guest an IP address
   e.g., '10.0.0.100' you can 'ping' the RTEMS guest
   from the linux host:

      ping 10.0.0.100
