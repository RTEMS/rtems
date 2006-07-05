# RTEMS_CANONICAL_SPLIT(THING)
# --------------------------
# Generate the variables THING, THING_{alias cpu vendor os}.
m4_define([RTEMS_CANONICAL_SPLIT],
[case $ac_cv_$1 in
*-*-*) ;;
*) AC_MSG_ERROR([invalid value of canonical $1]);;
esac
ac_save_IFS=$IFS; IFS='-'
set x $ac_cv_$1
shift
$1_cpu=$[1]
$1_vendor=$[2]
shift; shift
[# Remember, the first character of IFS is used to create $]*,
# except with old shells:
$1_os=$[*]
IFS=$ac_save_IFS
case $$1_os in *\ *) $1_os=`echo "$$1_os" | sed 's/ /-/g'`;; esac
])# RTEMS_CANONICAL_SPLIT
