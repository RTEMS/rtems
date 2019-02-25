# we store all generated files here.
TMPDIR=tmp_sdcard_dir.$$

FATIMG=$TMPDIR/bbxm_boot_fat.img
SIZE=65536
OFFSET=2048
FATSIZE=`expr $SIZE - $OFFSET`
UENV=uEnv.txt

rm -rf $TMPDIR
mkdir -p $TMPDIR
trap 'rm -rf $TMPDIR' EXIT

if [ $# -ne 3 ]
then	echo "Usage: $0 <RTEMS prefix> <RTEMS executable> <Device Tree Blob>"
	exit 1
fi

PREFIX=$1

if [ ! -d "$PREFIX" ]
then	echo "This script needs the RTEMS tools bindir as the first argument."
	exit 1
fi

executable=$2

case "$2" in
	*beagleboard*)
		ubootcfg=omap3_beagle
		imgtype=bb
		;;
	*beaglebone*)
		ubootcfg=am335x_evm
		imgtype=bone
		;;
	*)
		echo "Can't guess which uboot to use - please specify full path to executable."
		exit 1
		;;
esac

dtb="$3"

app=rtems-app.img

if [ ! -f "$executable" ]
then	echo "Expecting RTEMS executable as arg; $executable not found."
	exit 1
fi

set -e

IMG=${imgtype}_`basename $2`-sdcard.img

# Make an empty image
dd if=/dev/zero of=$IMG bs=512 seek=`expr $SIZE - 1` count=1
dd if=/dev/zero of=$FATIMG bs=512 seek=`expr $FATSIZE - 1` count=1

# Make an ms-dos FS on it
$PREFIX/bin/newfs_msdos -r 1 -m 0xf8 -c 4 -F16  -h 64 -u 32 -S 512 -s $FATSIZE -o 0 ./$FATIMG

# Prepare the executable.
base=`basename $executable`
$PREFIX/bin/arm-rtems5-objcopy $executable -O binary $TMPDIR/$base.bin
gzip -9 $TMPDIR/$base.bin
$PREFIX/bin/mkimage -A arm -O rtems -T kernel -a 0x80000000 -e 0x80000000 -n RTEMS -d $TMPDIR/$base.bin.gz $TMPDIR/$app
echo "setenv bootdelay 5
uenvcmd=run boot
boot=fatload mmc 0 0x80800000 $app ; fatload mmc 0 0x88000000 $(basename "$dtb") ; bootm 0x80800000 - 0x88000000" >$TMPDIR/$UENV

# Copy the uboot and app image onto the FAT image
$PREFIX/bin/mcopy -bsp -i $FATIMG $PREFIX/uboot/$ubootcfg/MLO ::MLO
$PREFIX/bin/mcopy -bsp -i $FATIMG $PREFIX/uboot/$ubootcfg/u-boot.img ::u-boot.img
$PREFIX/bin/mcopy -bsp -i $FATIMG $TMPDIR/$app ::$app
$PREFIX/bin/mcopy -bsp -i $FATIMG $TMPDIR/$UENV ::$UENV
# Copy DTB
$PREFIX/bin/mcopy -bsp -i $FATIMG "$dtb" ::"$(basename "$dtb")"

# Just a single FAT partition (type C) that uses all of the image
$PREFIX/bin/partition -m $IMG $OFFSET c:${FATSIZE}\*

# Put the FAT image into the SD image
dd if=$FATIMG of=$IMG seek=$OFFSET

echo "Result is in $IMG."
