dnl
dnl  $Id$
dnl 

dnl RTEMS_BSP_ALIAS(BSP_ALIAS,BSP_RETURNED)
dnl convert a bsp alias $1 into its bsp directory $2
AC_DEFUN(RTEMS_BSP_ALIAS,
[
    # account for "aliased" bsps which share source code
    case $1 in
      mvme162lx)    $2=mvme162  ;;  # mvme162 board variant
      gen68360_040) $2=gen68360 ;;  # 68360 in companion mode
      go32_p5)      $2=go32     ;;  # go32 on Pentium class CPU
      p4600)        $2=p4000    ;;  # p4000 board with IDT 4600
      p4650)        $2=p4000    ;;  # p4000 board with IDT 4650
      *)            $2=$1;;
    esac
])
