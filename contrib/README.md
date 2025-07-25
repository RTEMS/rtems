# Contributed code to RTEMS

If it is located within this directory it is *not* maintained by the RTEMS
project.

## Updating

To update code in this directory follow these steps:

1. Update code to the latest upstream version as a pure import.
2. Create a tag for that import.
3. Apply any changes required on top as separate commit(s).

Using this we will be able to diff between upstream versions and our own
changes.

## Commit message

In the import commit message add:

1. Upstream hash if pulling from a repository
2. Upstream version if release. If development version add hash.
3. URL file was taken from or cloned from.

## Tag format

The tag should always include the full path in the contrib directory with the
version appended.

If there is no version or it is a hash use `<dev version>-<hash>`.

For example:

- contrib/bsps/arm/stm32u5/stm32u5xx-hal-driver/1.5.0

The path is `contrib/bsps/arm/stm32u5/stm32u5xx-hal-driver` with `1.5.0` being the version.

### No available version

If there is no version then take the commit date + hash used for the import for example with `xz`:

- contrib/cpukit/xz-embedded/2015-11-03-e75f4eb

## Current Sources

These are the current source directories and where they are from. Look at the
Git log in order to find out version and update details

| Directory                             | Source                                                         |
| ------------------------------------- | -------------------------------------------------------------- |
| bsps/arm/stm32h7/cmsis-device-h7/     | https://github.com/STMicroelectronics/cmsis_device_h7.git      |
| bsps/arm/stm32h7/stm32h7xx_hal_driver | https://github.com/STMicroelectronics/stm32u5xx-hal-driver     |
| bsps/arm/stm32u5/stm32u5xx-hal-driver | https://github.com/STMicroelectronics/stm32h7xx_hal_driver.git |
| cpukit/fastlz                         | https://github.com/ariya/FastLZ                                |
| cpukit/utf8proc                       | https://juliastrings.github.io/utf8proc/releases/              |
| cpukit/libz                           | https://github.com/madler/zlib                                 |
| cpukit/uuid                           | https://github.com/tytso/e2fsprogs/tree/master/lib/uuid        |
| cpukit/xz-embedded                    | https://github.com/tukaani-project/xz-embedded                 |
