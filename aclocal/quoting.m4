dnl RTEMS_CONFIGURE_ARGS_QUOTE(dnl RETURN_VAR, [ADDITIONAL_CASES], [VAR_TO_PROCESS]])
dnl 
AC_DEFUN([_RTEMS_CONFIGURE_ARGS_QUOTE],
[
$1_prune()
{
  # Remove --cache-file and --srcdir arguments so they do not pile up.
  $1=
  ac_prev=
  for ac_arg
  do
    if test -n "$ac_prev"; then
      ac_prev=
      continue
    fi
    case $ac_arg in
    -cache-file | --cache-file | --cache-fil | --cache-fi \
    | --cache-f | --cache- | --cache | --cach | --cac | --ca | --c)
	  ac_prev=cache_file ;;
    -cache-file=* | --cache-file=* | --cache-fil=* | --cache-fi=* \
    | --cache-f=* | --cache-=* | --cache=* | --cach=* | --cac=* | --ca=* \
    | --c=*)
      ;;
    --config-cache | -C)
      ;;
    -srcdir | --srcdir | --srcdi | --srcd | --src | --sr)
	  ac_prev=srcdir ;;
    -srcdir=* | --srcdir=* | --srcdi=* | --srcd=* | --src=* | --sr=*)
      ;;
    m4_if([$2],,,[$2])
    *) $1="$$1 '$ac_arg'" ;;
    esac
  done
  export $1
}
])

AC_DEFUN([RTEMS_CONFIGURE_ARGS_QUOTE],[
m4_expand_once([_RTEMS_CONFIGURE_ARGS_QUOTE([$1],[$2])])
eval $1_prune m4_if([$3],,[$ac_configure_args],[[$]$3])
])
