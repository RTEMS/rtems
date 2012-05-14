#! /bin/sh
# RTEMS Build Executable Installers script.
#
# This script takes the RPM files built using the mingw/build-rpms.sh
# script.
#

echo $0 $*

source=$(dirname $0)

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

. $source/version

target_list=$(cat $source/targets)

. $source/target-section-text

mingw32_cpu_list="i686"

rpm_topdir=$(rpm --eval "%{_topdir}")

rtems_url="http://www.rtems.org/ftp/pub/rtems/windows/${version}/build-${tool_build}"

common_label="common"
local_rpm_database=yes
targets=$target_list
run_prefix=
relocation=
rpm_install=yes

package_source=$(cat $HOME/.rpmmacros | grep "^%_topdir" | sed -e "s/^.* //g")/SOURCES

if [ "$source" = "." ]; then
 source=$(pwd)
fi

while [ $# -gt 0 ];
do
 case $1 in
  -d)
   set -x
   ;;
  -i)
   rpm_install=no
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
  -r)
   shift
   relocation=$1
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
   echo "$0 -??"
   exit 2
   ;;
  *)
   terminate "error: invalid option (try --help): $1"
   ;;
 esac
 shift
done

cd=${run_prefix}cd
cp=${run_prefix}cp
make=${run_prefix}make
makensis=${run_prefix}makensis
mkdir=${run_prefix}mkdir
rm=${run_prefix}rm
rpmbuild=${run_prefix}rpmbuild
rpm=${run_prefix}rpm

if [ -z "$relocation" ]; then
 terminate "error: a relocation path needs to be specified"
fi

if [ $local_rpm_database = yes ]; then
  rpm_database="--dbpath $prefix/var/lib/rpm"
else
  rpm_database=
fi

#
# Get the RPM list given the architecture and the package.
#
# $1 - architecture
# $2 - package
#
get_rpm_list()
{
  if [ -d $rpm_topdir/mingw32/RPMS/$1 ]; then
   echo $(ls $rpm_topdir/mingw32/RPMS/$1/*.rpm | grep -v "debuginfo" | grep $2)
  fi
}

#
# Write target installer code.
#
# $1 - target
# $2 - target section test
# $3 - size of the tools
# $4 - Installer output file name
# $5 - Output file
# 
target_installer_exec()
{
  local t=$1
  local tst=$2
  local size_in_k=$3
  local outfile=$4
  local of=$5

  echo "" >> $of
  echo "; Target: $t" >> $of
  echo "Section \"${!tst}\" Section_$t" >> $of
  echo " AddSize $size_in_k" >> $of
  echo " StrCpy \$1 \$EXEDIR\\$outfile" >> $of
  echo " DetailPrint \"Checking for \$1\"" >> $of
  echo " IfFileExists \$1 ${t}_found" >> $of
  echo " StrCpy \$1 \$INSTDIR\\Packages\\$outfile" >> $of
  echo " DetailPrint \"Checking for \$1\"" >> $of
  echo " IfFileExists \$1 ${t}_found" >> $of
  echo "  SetOutPath \"\$INSTDIR\Packages\"" >> $of
  echo "  DetailPrint \"Downloading $rtems_url/$outfile\"" >> $of
  echo "  NSISdl::download $rtems_url/$outfile $outfile" >> $of
  echo "  Pop \$R0" >> $of
  echo "  StrCmp \$R0 \"success\" ${t}_found_2 ${t}_not_found_2" >> $of
  echo " ${t}_not_found_2:" >> $of
  echo "   SetDetailsView show" >> $of
  echo "   DetailPrint \"Download failed: \$R0\"" >> $of
  echo "   MessageBox MB_OK \"Download failed: \$R0\"" >> $of
  echo "   Goto ${t}_done" >> $of
  echo " ${t}_found_2:" >> $of
  echo "   Strcpy \$1 \"\$INSTDIR\\Packages\\$outfile\"" >> $of
  echo " ${t}_found:" >> $of
  echo "  DetailPrint \"Installing: \$1\"" >> $of
  echo "  ExecWait '\"\$1\" \$SilentOption \$DebugOption /D=\$INSTDIR' \$0" >> $of
  echo "  BringToFront" >> $of
  echo "  IntCmp \$0 0 +3" >> $of
  echo "   MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \"${!tst} install failed. Do you wish to continue ?\" IDYES +2" >> $of
  echo "   Abort" >> $of
  echo " ${t}_done:" >> $of
  echo "SectionEnd" >> $of
}

#
# Get the size if kilo-bytes of the package as the listed files.
#
# $1 - relocation directory of installed files.
#
get_size_in_k()
{
  if [ ! -d $1 ]; then
    terminate "target directory not found: $1"
  fi
  if [ ! -f $1/files.txt ]; then
    terminate "target files not found: $1/files.txt"
  fi

  local here=$(pwd)
  check "getting the current directory"
  cd $1
  check "changing directory: $1"

  local size_in_k=$(du -c -k $(cat files.txt) | grep total | sed -e "s/\t.*//g")
  check "getting total size"

  cd $here
  check "changing directory: $here"

  echo $size_in_k
}

#
# Create the installer given the architecture and package.
#
# $1 - architecture/processor
# $2 - package
# $3 - package name
# $4 - outfile
#
create_installer()
{
  local p=$1
  local t=$2
  local n=$3
  local outfile=$4

  rpm_options="--ignoreos --force --nodeps --noorder "

  rpms=$(get_rpm_list $p $t)
  check "getting the $n RPM list"

  if [ -n "$rpms" ]; then
   treloc=$relocation/$n
   echo "Relocation path: $treloc"

   if [ $rpm_install = yes ]; then
    echo "Clean the relocation directory: $treloc"
    $rm -rf $treloc
    check "removing the relocation directory: $treloc"

    for r in $rpms
    do
     echo "rpm $rpm_database --relocate $prefix=$treloc $rpm_options -i $r"
     $rpm $rpm_database --relocate $prefix=$treloc $rpm_options -i $r
     check "installing rpm: $r"
    done
   fi

   files=$(find $treloc -type f -a -not \( -name \*.gch \
                                        -o -name rtems.nsi \
                                        -o -name rtems-files.nsi \
                                        -o -name rtems.ini \
                                        -o -name files.txt \) | \
           sed -e "s/^$(echo ${treloc} | sed 's/\//\\\//g')//" -e "s/^\///" | sort)
   check "find the file list"

   echo "$files" > $treloc/files.txt
   check "write the file list"

   size_in_k=$(get_size_in_k $treloc)

   of=$treloc/rtems-files.nsi

   echo "!macro RTEMS_INSTALL_FILES" > $of
   echo " !ifndef EMPTY_INSTALLER" >> $of

   install_dir=

   for f in $files
   do
    d=$(dirname $f)
    if [ "$install_dir" != "$d" ]; then
     id=$(echo $d | sed -e 's/\//\\/g' -e 's/\/$//')
     echo "  SetOutPath \"\$INSTDIR\\$id\"" >> $of
     install_dir=$d
    fi
    echo "  File \"$treloc/$f\"" >> $of
   done

   echo " !endif" >> $of
   echo "!macroend" >> $of

   echo "!macro RTEMS_DELETE_FILES" >> $of
   echo " !ifndef EMPTY_INSTALLER" >> $of

   #
   # Get a list of directories so we can delete them once all
   # the files have been deleted. We need to be selective as
   # the directory maybe used by another installer.
   #
   # Once we get a list we need to also add the path down
   # to that node and then sort them so we work from bottom up.
   #

   remove_dirs=

   for f in $files
   do
    d=$(dirname $f)
    found=no
    for rd in $remove_dirs
    do
     if [ "$d" = "$rd" ]; then
      found=yes
      break;
     fi
    done
    if [ $found = no ]; then
     remove_dirs="$remove_dirs $d"
    fi
    rf=$(echo $f | sed -e 's/\//\\/g' -e 's/\/$//')
    echo "  Delete \"\$INSTDIR\\$rf\"" >> $of
   done

   for d in $remove_dirs
   do
    while [ $d != . ]
    do
     found=no
     for rd in $remove_dirs
     do
      if [ "$d" = "$rd" ]; then
       found=yes
       break
      fi
     done
     if [ $found = no ]; then
      remove_dirs="$remove_dirs $d"
     fi
     d=$(dirname $d)
    done
   done

   remove_dirs=$(for r in $remove_dirs; do echo $r; done | sort -r -u)

   for d in $remove_dirs
   do
     if [ "$d" = "." ]; then
      d=
     fi 
     rd=$(echo $d | sed -e 's/\//\\/g' -e 's/\/$//')
     echo "  RMDir \"\$INSTDIR\\$rd\"" >> $of
   done

   echo " !endif" >> $of
   echo "!macroend" >> $of

   rtems_binary=$rpm_topdir/mingw32/exe/$p
   echo "mkdir -p $rtems_binary"
   $mkdir -p $rtems_binary
   check "make the RTEMS binary install point: $rtems_binary"

   of=$treloc/rtems.nsi
   echo "!define RTEMS_TARGET \"$n\"" > $of
   echo "!define RTEMS_VERSION \"$version\"" >> $of
   echo "!define RTEMS_BUILD_VERSION \"$tool_build\"" >> $of
   echo "!define RTEMS_PREFIX \"rtems-tools\"" >> $of
   echo "!define RTEMS_SOURCE \"$source\"" >> $of
   echo "!define RTEMS_RELOCATION \"$treloc\"" >> $of
   echo "!define RTEMS_LOGO \"$source/rtems_logo.bmp\"" >> $of
   echo "!define RTEMS_BINARY \"$rtems_binary\"" >> $of
   echo "!define RTEMS_LICENSE_FILE \"$source/rtems-license.rtf\"" >> $of
   echo "!define RTEMS_OUTFILE \"$outfile\"" >> $of
   echo "!define TOOL_PREFIX \"$prefix\"" >> $of
   echo "!define RTEMS_TOOLS_SIZE \"$size_in_k\"" >> $of

   if [ $n = $common_label ]; then
     echo "!define COMMON_FILES" >> $of
   fi

   echo "!include \"$treloc/rtems-files.nsi\"" >> $of
   echo "!include \"$source/options.nsi\"" >> $of

   if [ $n = $common_label ]; then
     echo "!include \"$relocation/rtems-sections.nsi\"" >> $of
   fi

   echo "!include \"$source/rtems-tools.nsi\"" >> $of

   echo "cp $source/rtems.ini $treloc/rtems.ini"
   $cp $source/rtems.ini $treloc/rtems.ini
   check "coping the dialog definition file: $treloc/rtems.ini"

   if [ $n = $common_label ]; then
     echo "Section -SecCommon" >> $of
     echo " SetOutPath \"\$INSTDIR"\" >> $of
     echo " File \"\${RTEMS_SOURCE}/AUTHORS"\" >> $of
     echo " File \"\${RTEMS_SOURCE}/COPYING\"" >> $of
     echo " File \"\${RTEMS_SOURCE}/README\"" >> $of
     echo "SectionEnd" >> $of
   fi

   echo "makensis $of"
   $makensis $of
   check "making the installer: $of"

  fi
}

create_autotools_installer()
{
  local p=$1
  local t=$2
  local n=$3

  treloc=$relocation/$n
  echo "Relocation path: $treloc"

  echo "Clean the relocation directory: $treloc"
  $rm -rf $treloc
  check "removing the relocation directory: $treloc"
  mkdir -p $treloc
  check "creating relocation directory: $treloc"

  rtems_binary=$rpm_topdir/mingw32/exe/$p
  echo "mkdir -p $rtems_binary"
  $mkdir -p $rtems_binary
  check "make the RTEMS binary install point: $rtems_binary"

  size_in_k=2000

  outfile=rtems$version-tools-$n-$tool_build.exe

  of=$treloc/rtems.nsi
  echo "!define RTEMS_TARGET \"$n\"" > $of
  echo "!define RTEMS_VERSION \"$version\"" >> $of
  echo "!define RTEMS_BUILD_VERSION \"$tool_build\"" >> $of
  echo "!define RTEMS_PREFIX \"rtems-tools\"" >> $of
  echo "!define RTEMS_SOURCE \"$source\"" >> $of
  echo "!define RTEMS_PACKAGE_SOURCE \"$package_source\"" >> $of
  echo "!define RTEMS_LOGO \"$source/rtems_logo.bmp\"" >> $of
  echo "!define RTEMS_BINARY \"$rtems_binary\"" >> $of
  echo "!define RTEMS_LICENSE_FILE \"$source/rtems-license.rtf\"" >> $of
  echo "!define RTEMS_OUTFILE \"$outfile\"" >> $of
  echo "!define TOOL_PREFIX \"$prefix\"" >> $of
  echo "!define RTEMS_TOOLS_SIZE \"$size_in_k\"" >> $of

  . $source/autoconf.def

  echo "!define RTEMS_AUTOCONF \"$package_name\"" >> $of
  echo "!define RTEMS_AUTOCONF_SOURCE \"$package_source\"" >> $of
  echo "!macro RTEMS_AUTOCONF_PATCHES" >> $of
  for p in $package_patches
  do
    echo " File \"\${RTEMS_PACKAGE_SOURCE}/$p\"" >> $of
  done
  echo "!macroend" >> $of 

  echo "!macro RTEMS_DELETE_AUTOCONF_PATCHES" >> $of
  for p in $package_patches
  do
    echo " Delete \"\$INSTDIR\\Packages\\Source\\$p\"" >> $of
  done
  echo "!macroend" >> $of 

  . $source/automake.def

  echo "!define RTEMS_AUTOMAKE \"$package_name\"" >> $of
  echo "!define RTEMS_AUTOMAKE_SOURCE \"$package_source\"" >> $of
  echo "!macro RTEMS_AUTOMAKE_PATCHES" >> $of
  for p in $package_patches
  do
    echo " File \"\${RTEMS_PACKAGE_SOURCE}/$p\"" >> $of
  done
  echo "!macroend" >> $of 

  echo "!macro RTEMS_DELETE_AUTOMAKE_PATCHES" >> $of
  for p in $package_patches
  do
    echo " Delete \"\$INSTDIR\\Packages\\Source\\$p\"" >> $of
  done
  echo "!macroend" >> $of 

  echo "!include \"$source/rtems-autotools.nsi\"" >> $of

  echo "cp $source/rtems.ini $treloc/rtems.ini"
  $cp $source/rtems.ini $treloc/rtems.ini
  check "coping the dialog definition file: $treloc/rtems.ini"

  echo "makensis $of"
  $makensis $of
  check "making the installer: $of"
}

#
# Handle each type of host processor.
#
for p in $mingw32_cpu_list
do
 echo "; Components based on each target." > $relocation/rtems-sections.nsi
 create_autotools_installer noarch auto autotools
 target_installer_exec auto auto_section_text 2000 \
                       rtems$version-tools-autotools-$tool_build.exe \
                       $relocation/rtems-sections.nsi
 for t in $targets
 do
  create_installer $p $t $t rtems$version-tools-$t-$tool_build.exe
 done
 for t in $(cat $source/targets)
 do
  target_installer_exec $t ${t}_section_text \
                        $(get_size_in_k $relocation/$t) \
                        rtems$version-tools-$t-$tool_build.exe \
                        $relocation/rtems-sections.nsi
 done
 # Must be done last
 create_installer $p $common_label $common_label $version-tools-$tool_build.exe
done
