dnl
dnl  $Id$
dnl 

dnl _RTEMS_BSP_ALIAS(BSP_ALIAS,RTEMS_BSP_FAMILY)
dnl Internal subroutine to RTEMS_BSP_ALIAS
AC_DEFUN([_RTEMS_BSP_ALIAS],
[# account for "aliased" bsps which share source code
  case $1 in
    bare*)        $2=bare             ;; # EXP: bare-aliases

    c3xsim)       $2=c4xsim           ;; # TI C3x Simulator in gdb
    gen68360_040) $2=gen68360         ;; # m68k - 68360 in companion mode
    h8sxsim)      $2=h8sim          ;; # h8sim built for h8sx
    pghplus)      $2=tqm8xx           ;; # powerpc - mpc8xx custom board
    tqm8xx_stk8xx) $2=tqm8xx          ;; # powerpc - tqm8xx on starter kit
    pgh360)       $2=gen68360         ;; # m68k - 68360 custom board
    m5484FireEngine) $2=genmcf548x    ;; # m68k - mcf548x eva board
    mbx8*)        $2=mbx8xx           ;; # MBX821/MBX860 board
    mcp750)       $2=motorola_powerpc ;; # Motorola PPC board variant
    mtx603e)      $2=motorola_powerpc ;; # Motorola PPC board variant
    mvme162lx)    $2=mvme162          ;; # m68k - mvme162 board variant
    mvme2100)     $2=motorola_powerpc ;; # Motorola PPC board variant
    mvme2307)     $2=motorola_powerpc ;; # Motorola PPC board variant
    m32csim)      $2=m32cbsp          ;; # M32C on gdb simulator
    p4600)        $2=p4000            ;; # mips - p4000 board w/IDT 4600
    p4650)        $2=p4000            ;; # mips - p4000 board w/IDT 4650
    pc386dx)      $2=pc386            ;; # i386 - PC w/o FPU
    pc486)        $2=pc386            ;; # i386 - PC with i486DX
    pc586)        $2=pc386            ;; # i386 - PC with Pentium
    pc686)        $2=pc386            ;; # i386 - PC with PentiumPro
    brs5l*)       $2=gen5200          ;; # MPC5200 based board
    pm520*)       $2=gen5200          ;; # MPC5200 based board
    icecube)      $2=gen5200          ;; # MPC5200 based board - LITE5200B
    mpc55??evb)   $2=mpc55xxevb       ;; # MPC5566EVB based board
    mpc8349eamds) $2=gen83xx          ;; # MPC8349 based board
    hsc_cm01)     $2=gen83xx          ;; # MPC8349 based board
    mpc8313erdb)  $2=gen83xx          ;; # MPC83XX based board
    rtl22xx_t)    $2=rtl22xx          ;; # rtl22xx bsp in thumb mode
    lpc2478)      $2=lpc24xx          ;; # LPC2478 (QVGA Base Board from Embedded Artists)
    lpc2478_ncs)  $2=lpc24xx          ;; # LPC2478 (NCS)
    lpc2478_ncs_ram) $2=lpc24xx       ;; # LPC2478 (NCS, code and data in external RAM)
    simcpu32)     $2=sim68000         ;; # BSVC CPU32 variant
    simsh1)       $2=shsim            ;; # SH1 simulator in GDB
    simsh2)       $2=shsim            ;; # SH2 simulator in GDB
    simsh2e)       $2=shsim           ;; # SH2e simulator in GDB
    simsh4)       $2=shsim            ;; # SH4 simulator in GDB
    sis)          $2=erc32            ;; # erc32 SIS simulator
    *)            $2=$1;;
  esac]
)

dnl RTEMS_BSP_ALIAS(BSP_ALIAS,RTEMS_BSP_FAMILY)
dnl convert a bsp alias $1 into its bsp directory RTEMS_BSP_FAMILY
AC_DEFUN([RTEMS_BSP_ALIAS],
[_RTEMS_BSP_ALIAS(m4_if([$1],,[$RTEMS_BSP],[$1]),
  m4_if([$2],,[RTEMS_BSP_FAMILY],[$2]))]
)
