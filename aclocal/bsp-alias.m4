dnl
dnl  $Id$
dnl 

dnl _RTEMS_BSP_ALIAS(BSP_ALIAS,RTEMS_BSP_FAMILY)
dnl Internal subroutine to RTEMS_BSP_ALIAS
AC_DEFUN(_RTEMS_BSP_ALIAS,
[# account for "aliased" bsps which share source code
  case $1 in
    simcpu32)     $2=sim68000         ;; # BSVC CPU32 variant
    c3xsim)       $2=c4xsim           ;; # TI C3x Simulator in gdb
    mcp750)       $2=motorola_powerpc ;; # Motorola PPC board variant
    mvme2307)     $2=motorola_powerpc ;; # Motorola PPC board variant
    mvme162lx)    $2=mvme162          ;; # m68k - mvme162 board variant
    gen68360_040) $2=gen68360         ;; # m68k - 68360 in companion mode
    p4600)        $2=p4000            ;; # mips64orion - p4000 board w/IDT 4600
    p4650)        $2=p4000            ;; # mips64orion - p4000 board w/IDT 4650
    mbx8*)        $2=mbx8xx           ;; # MBX821/MBX860 board
    pc386dx)      $2=pc386            ;; # i386 - PC w/o FPU
    pc486)        $2=pc386            ;; # i386 - PC with i486DX
    pc586)        $2=pc386            ;; # i386 - PC with Pentium
    pc686)        $2=pc386            ;; # i386 - PC with PentiumPro
    pck6)         $2=pc386            ;; # i386 - PC with K6
    bare*)        $2=bare             ;; # EXP: bare-aliases
    erc32nfp)     $2=erc32            ;; # erc32 without fpu
    leon1)     	  $2=leon             ;; # leon without fpu
    leon2)     	  $2=leon             ;; # leon with fpu
    simsh7032)    $2=shsim            ;; # SH7032 simulator
    simsh7045)    $2=shsim            ;; # SH7045 simulator
    *)            $2=$1;;
  esac]
)

dnl RTEMS_BSP_ALIAS(BSP_ALIAS,RTEMS_BSP_FAMILY)
dnl convert a bsp alias $1 into its bsp directory RTEMS_BSP_FAMILY
AC_DEFUN(RTEMS_BSP_ALIAS,
[_RTEMS_BSP_ALIAS(ifelse([$1],,[$RTEMS_BSP],[$1]),
  ifelse([$2],,[RTEMS_BSP_FAMILY],[$2]))]
)
