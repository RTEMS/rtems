#! /bin/sh

terminate()
{
  echo "error: $*" >&2
  if [ $? -eq 0 ]; then
    exit 222
  fi
  exit $?
}

check()
{
  if [ $? -ne 0 ]; then
    terminate $*
  fi
}

source=$(dirname $0)

if [ "$source" = "." ]; then
  source=$(pwd)
fi

build=none
clean=no
prefix=none

while [ $# -gt 0 ];
do
  case $1 in
    -b)
      shift
      build=$1
      ;;
    -c)
      shift
      config=$1
      ;;
    -d)
      set -x
      ;;
    -h)
      usage
      exit 223
      ;;
    -p)
      shift
      prefix=$1
      ;;
    -s)
      shift
      source=$1
      ;;
    *)
      terminate "invalid option (try -h): $1"
      ;;
  esac
  shift
done

#
# dirdepth: find the depth of a directory
#
# $1 : path
#
dirdepth()
{
  local depth=0
  local p=$1
  p=$(echo $p | sed -e "s/^\///g")
  p=$(echo $p | sed -e "s/\/$//g")
  while [ $p != . ];
  do
    depth=$(expr $depth + 1)
    p=$(dirname $p)
  done
  echo $depth
}

#
# build_package()
#
# $1 : package
# $2 : package source
# $3 : tar options
# $4 : patch list
#
build_package()
{
  local mydir=$(pwd)

  if [ ! -f $source/$2 ]; then
    terminate "$1 source is not present or not a file: $2"
  fi

  for p in $4
  do
    if [ ! -f $source/$p ]; then
      terminate "$1 patch is not present or not a file: $p"
    fi
  done

  echo "cd $build"
  cd $build
  check "changing to $build directory"

  echo "tar $3 $source/$2"
  tar $3 $source/$2
  check "extraction of source: $2"

  echo "cd $build/$1"
  cd $build/$1
  check "changing to $build/$1 directory"

  for p in $4
  do
    echo "patch -p0 < $source/$p"
    patch -p0 < $source/$p
    check "patching $autoconf with $p"
  done

  echo "cd $build"
  cd $build
  check "changing to $build directory"

  echo "rm -rf mingw32-$1"
  rm -rf mingw32-$1
  check "removing mingw32-$1 build directory"

  echo "mkdir mingw32-$1"
  mkdir mingw32-$1
  check "creating mingw32-$1"

  echo "cd $build/mingw32-$1"
  cd $build/mingw32-$1
  check "changing to $build/mingw32-$1"

  echo "$build/$1/configure --prefix=$prefix"
  $build/$1/configure --prefix=$prefix
  check "configuring $1"

  echo "make"
  make
  check "building $1"

  echo "make DESTDIR=$build/install-$1 install"
  make DESTDIR=$build/install-$1 install
  check "installing to $build/install-$1"

  echo find $build/install-$1$prefix -type f
  local files=$(find $build/install-$1$prefix -type f)
  check "reading $1 file list"

  of=$prefix/Uninstall/$1-files
  echo "b install-$1$prefix" | sed -e "s/\//\\\\/g" >> $of

  re_path=$(echo $build/install-$1$prefix/ | sed -e 's/\//\\\//g' -e 's/\./\\\./g')

  dirs=
  for f in $files
  do
    if [ $(basename $f) = dir ]; then
      continue
    fi
    f=$(echo $f | sed -e "s/$re_path//g")
    found=no
    fd=$(dirname $f)
    for d in $dirs
    do
      if [ $d = $fd ]; then
        found=yes
        break;
      fi
    done
    if [ $found = no ]; then
      echo "d $fd" | sed -e "s/\//\\\\/g" >> $of
      dirs="$fd $dirs"
    fi
    echo "f $f" | sed -e "s/\//\\\\/g" >> $of
  done

  for d in $dirs
  do
    while [ $d != . ]
    do
      found=no
      for rd in $dirs
      do
        if [ "$d" = "$rd" ]; then
          found=yes
          break
        fi
      done
      if [ $found = no ]; then
        dirs="$dirs $d"
      fi
      d=$(dirname $d)
    done
  done

  sorted_dirs=
  for d in $dirs
  do
    depth=$(dirdepth $d)
    nsp=
    for sd in $sorted_dirs
    do
      if [ ! -z "$d" ]; then
        spd=$(dirdepth $sd)
        if [ $depth -ge $spd ]; then
          sd="$d $sd"
          d=
        fi
      fi
      nsp="$nsp $sd"
    done
    if [ ! -z "$d" ]; then
      nsp="$nsp $d"
    fi
    sorted_dirs=$nsp
  done

  for d in $sorted_dirs
  do
    echo "D $d" | sed -e "s/\//\\\\/g" >> $of
  done

  echo "cd $mydir"
  cd $mydir
  check "changing to $mydir"
}

munch_path()
{
  local p=$1
  if [ $(echo $p | sed -e "s/[a-zA-Z]:.*/yes/g") = yes ]; then
    p=$(echo $p | sed -e "s/^[a-zA-Z]:/\/\0/g" -e "s/://g")
  fi
  echo $p
}

if [ $build = none ]; then
  terminate "no build specified (try -h)"
fi

if [ $prefix = none ]; then
  terminate "no prefix specified (try -h)"
fi

source=$(munch_path $source)
build=$(munch_path $build)
prefix=$(munch_path $prefix)
config=$(munch_path $config)

if [ ! -d $build ]; then
  terminate "build path is not a directory or does not exist: $build"
fi

if [ ! -d $prefix ]; then
  terminate "prefix path is not a directory or does not exist: $prefix"
fi

if [ ! -f $config ]; then
  terminate "could find package configuration file: $config"
fi

. $config

# Need to handle paths carefully as a problem seems to exist
# with autoconf when it is in the path.
if [ $(echo $package_name | sed -e "s/autoconf-.*/yes/g") != yes ]; then
 export PATH=$prefix/bin:$PATH
fi

build_package $package_name \
              $package_source \
              $package_taropts \
              "$package_patches"

exit $?
