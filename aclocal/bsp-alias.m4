dnl
dnl  $Id$
dnl 

dnl RTEMS_BSP_ALIAS(BSP_ALIAS,BSP_RETURNED)
dnl convert a bsp alias $1 into its bsp directory $2
AC_DEFUN(RTEMS_BSP_ALIAS,
[
  # account for "aliased" bsps which share source code
  case $1 in
    mcp750)       $2=motorola_powerpc ;; # Motorola PPC board variant
    mvme2307)     $2=motorola_powerpc ;; # Motorola PPC board variant
    mvme162lx)    $2=mvme162          ;; # m68k - mvme162 board variant
    gen68360_040) $2=gen68360         ;; # m68k - 68360 in companion mode
    p4600)        $2=p4000            ;; # mips64orion - p4000 board w/IDT 4600
    p4650)        $2=p4000            ;; # mips64orion - p4000 board w/IDT 4650
    mbx8*)        $2=mbx8xx           ;; # MBX821/MBX860 board
    pc486)        $2=pc386            ;; # i386 - PC with i486DX
    pc586)        $2=pc386            ;; # i386 - PC with Pentium
    pc686)        $2=pc386            ;; # i386 - PC with PentiumPro
    *)            $2=$1;;
  esac
])
