#
# $Id$
#
# RTEMS Build RPM script.
#
# This script builds the RTEMS tools into RPM packages on a Linux Fedora host,
# and supports Canadian cross builds to other hosts. It is light sugar around
# the fine work of Ralf Corsepius thats actuallt builds the RPM files.
# 
# We build all the targets for a given host.
#
# Note: This file should be placed in crossrpms.
#

terminate()
{
  echo "error: $*" >&2
  exit 1
}

check()
{
 if [ $? -ne 0 ]; then
  terminate
 fi
}

processor=$(uname -p)

version=4.7

base_tool_list="binutils gcc"

target_list="i386 m68k powerpc sparc arm mips"
host_list="cygwin freebsd5.2 freebsd6.0 freebsd6.1 mingw32"

rtems_tool_list="$base_tool_list"
cygwin_tool_list="w32api libs $base_tool_list"
freebsd_tool_list="libs $base_tool_list"
mingw32_tool_list="w32api libs $base_tool_list"

cygwin_cpu_list="i686"
freebsd_cpu_list="i586"
mingw32_cpu_list="i686"

rpm_topdir=$(rpmbuild --showrc | grep "\?\?\: _topdir" | sed 's/.*:.*_topdir\t*//')

prefix=/opt/rtems
hosts=$host_list
build=$processor-linux-gnu
infos=
targets=$target_list
run_prefix=
rpm_prefix=custom-
rpm_prefix_arg=
local_rpm_database=yes
clean=yes
source=$(pwd)

while [ $# -gt 0 ];
do
 case $1 in
  -c)
   clean=no
   ;;
  -d)
   set -x
   ;;
  -h)
   shift
   hosts=$1
   ;;
  -i)
   infos="--enable-infos"
   ;;
  -l)
   shift
   rpm_prefix=$1-
   rpm_prefix_arg="--enable-rpmprefix=$rpm_label"
   ;;
  -n)
   run_prefix="echo "
   ;;
  -p)
   shift
   prefix=$1
   ;;
  -R)
   local_rpm_database=no
   ;;
  -s)
   shift
   source=$1
   ;;
  -t)
   shift
   targets=$1
   ;;
  -v)
   shift
   version=$1
   ;;
  --help)
   echo "$0 -cdinr -o <output> -h <hosts>" \
        "-l <rpm prefix> -p <prefix> -s <source> -t <targets> -v <version>"
   exit 2
   ;;
  *)
   terminate "error: invalid option (try --help): $1"
   ;;
 esac
 shift
done

for t in $targets;
do
 if [ -z "$(echo $target_list | grep $t)" ]; then
  terminate "error: invalid target: $t"
 fi
done

cd=${run_prefix}cd
configure=${run_prefix}${source}/configure
cp=${run_prefix}cp
make=${run_prefix}make
mkdir=${run_prefix}mkdir
rm=${run_prefix}rm
rpmbuild=${run_prefix}rpmbuild
rpm=${run_prefix}rpm

if [ $local_rpm_database = yes ]; then
  rpm_database="--dbpath $prefix/var/lib/rpm"
else
  rpm_database=
fi

echo " Source: $source"
echo " Prefix: $prefix"
echo "  Hosts: $hosts"
echo "Targets: $targets"

if [ ! -f $source/configure ]; then
 terminate "$source/configure not found under: $source"
fi

if [ x$prefix != x ]; then
 if [ -e $prefix ]; then
  if [ ! -d $prefix ]; then
   terminate "$prefix is not a directory"
  fi
 else
  echo "Creating $(pwd)"
  $mkdir $prefix
  check "making the prefix directory: $prefix"
 fi
fi

$cd $prefix
check "cannot change to the prefix directory: $prefix"

if [ $clean = yes ]; then
 echo "Cleaning: $(pwd)"
 $rm -rf *
fi

#
# Using a local RPM database means we do not need to be root to
# perform the build.
#
if [ $local_rpm_database = yes ]; then
 echo "Creating private RPM database: $(pwd)/var/lib/rpm"
 if [ ! -d /var/lib/rpm ]; then
  terminate "no RPM database found on this host"
  exit 1
 fi
 if [ ! -d var/lib ]; then
  $mkdir -p var/lib
  check "making the local RPM database directory: var/lib"
 fi
 if [ $clean = yes -o ! -e var/lib/rpmPackages ]; then
  echo "Coping RPM database to a local RPM database"
  $cp -r /var/lib/rpm var/lib/rpm
  check "coping the RPM database to the local database"
 fi
fi

#
# Set the path to the new directory.
#
export PATH=$prefix/bin:$PATH

#
# Install the RPM.
#

rpm_installer()
{
 local rpm_bases=
 local rpm_libs=
 local rpm_tools=
 local r

 local rpm_database=$1
 shift
 local rpm_arch=$1
 shift
 local rpm_path=$1
 shift

 #
 # Get the names of the rpm files that have been built. This means we
 # do not have maintain version info.
 #
 rpm_names=$(rpmbuild $rpm_database $* --nobuild 2> /dev/null | \
             grep -v "debuginfo" | grep "Process" | sed 's/Process.*\: //')
 #
 # If we have a base place that one first then libs then the tools.
 #
 for r in $rpm_names
 do
  if [ $(echo $r | sed 's/.*base.*/yes/') = yes ]; then
   rpm_bases="$rpm_bases $r"
  elif [ $(echo $r | sed 's/.*lib.*/yes/') = yes ]; then
   rpm_libs="$rpm_libs $r"
  elif [ $(echo $r | sed 's/.*sys\-root.*/yes/') = yes ]; then
   rpm_libs="$rpm_libs $r"
  else
   rpm_tools="$rpm_tools $r"
  fi
 done

 for r in $rpm_bases $rpm_libs $rpm_tools
 do
  echo "rpm $rpm_database --force" \
           "-i $rpm_path/$rpm_arch/$r.$rpm_arch.rpm"
  $rpm $rpm_database --force -i $rpm_path/$rpm_arch/$r.$rpm_arch.rpm
  check "installing the $r.$rpm_arch.rpm rpm failed"
 done
}

#
# Select the type of arch we have.
#

rpm_arch()
{
  if [ $(echo $1 | sed 's/.*api.*/yes/') = yes ]; then
   echo "noarch"
  elif [ $(echo $1 | sed 's/.*lib.*/yes/') = yes ]; then
   echo "noarch"
  else
   echo $2
  fi
}

#
# We always build tools for the build host. We need them to
# build the libraries for the target processor.
#
hosts="linux $hosts"

echo "Configuring host:$h($th) target:$t-rtems$version"
echo "configure --prefix=$prefix $rpm_prefix_arg " \
     " --target=all $infos"
$configure --prefix=$prefix $rpm_prefix_arg \
           --target=all \
           $infos
check "configuring the crossrpms failed"
echo "make"
$make
check "building the rpm spec files failed"

for t in $targets;
do
 for h in $hosts;
 do
  #
  # Need to translate the build host to an RTEMS host. Create a
  # target host (th) and specs directory (sd) variable to handle
  # the use of RTEMS.
  #
  if [ $h = "linux" ]; then
   th="linux-gnu"
   sd=rtems$version
  else
   th=$h
   sd=$h
  fi

  th="$processor-pc-$th"

  if [ $(echo $th | sed -e "s/.*linux.*/yes/") = yes ]; then
   canadian_cross=no
  else
   canadian_cross=yes
  fi

  echo "Canadian Cross: $canadian_cross"

  if [ $canadian_cross = yes ]; then
   #
   # Associate the host to its tool list to get the packages to build.
   #
   tl=${h}_tool_list
   pl=${h}_cpu_list

   echo "Native Host Tools: ${!tl} for ${!pl}"

   echo "make -C $sd/$p"
   $make -C $sd/$p
   check "building the rpm spec files failed: $sd/$p"

   for p in ${!pl}
   do
    for s in ${!tl}
    do
     rpmbuild_cmd="-ba $prefix/$sd/$p/$rpm_prefix$th-$s.spec --target=$build"

     echo "rpmbuild $rpm_database $rpmbuild_cmd"
     $rpmbuild $rpm_database $rpmbuild_cmd
     check "building the $sd/$p/$rpm_prefix$th-$s rpm failed"

     rpm_installer "$rpm_database" \
                   $(rpm_arch $rpm_prefix$th-$s $processor) \
                   $rpm_topdir/linux/RPMS \
                   $rpmbuild_cmd
    done
   done
  fi

  for s in ${rtems_tool_list}
  do
   rpmbuild_cmd="-ba $prefix/rtems$version/$t/$rpm_prefix$t-rtems$version-$s.spec --target=$th"

   if [ $canadian_cross = yes ]; then
    echo "rpmbuild --define '_build i686-redhat-linux' --define '_host $th' $rpm_database $rpmbuild_cmd"
    $rpmbuild --define "_build i686-redhat-linux" \
              --define "_host $th" \
              $rpm_database $rpmbuild_cmd
    check "building host cross target: $rpm_prefix$t-rtems$version-$s"
   else
    echo "rpmbuild $rpm_database $rpmbuild_cmd"
    $rpmbuild $rpm_database $rpmbuild_cmd
    check "building host cross target: $rpm_prefix$t-rtems$version-$s"
   fi 

   if [ $canadian_cross != yes ]; then
    rpm_installer "$rpm_database" \
                  $(rpm_arch $rpm_prefix$t-rtems$version-$s $processor) \
                  $rpm_topdir/$h/RPMS \
                  $rpmbuild_cmd
   fi
  done
 done
done
