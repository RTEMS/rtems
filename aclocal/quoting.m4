dnl RTEMS_CONFIGURE_ARGS_QUOTE(dnl RETURN_VAR, [ADDITIONAL_CASES], [VAR_TO_PROCESS]])
dnl 
AC_DEFUN([RTEMS_CONFIGURE_ARGS_QUOTE],
[
  # Remove --cache-file and --srcdir arguments so they do not pile up.
  $1=
  ac_prev=
  for ac_arg in m4_if([$3],,[$ac_configure_args],[[$]$3]); do
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
    *) $1="$$1 $ac_arg" ;;
    esac
  done
])
