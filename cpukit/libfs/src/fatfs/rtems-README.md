# rtems-README

This file contains the import process of the FatFS source code from the original upstream. We follow the RTEMS [Third-Party Source Code](https://docs.rtems.org/docs/main/eng/coding-conventions.html#third-party-source-code) manual here.

The import is made by extracting the archive from the [FatFS](https://elm-chan.org/fsw/ff/archives.html) project, putting the source directory here, and adding the checksum with any changelogs on this README file. Here's how the checksum should be calculated (to exclude any file starting with `rtems-` prefix):

```
$ find . -type f ! -name 'rtems-*' -exec sha256sum {} + | sort | sha256sum
```

Any new file added must have `rtems-` prefix, and any changes made to the imported files should have rtems guards added, e.g. `#ifdef __rtems__`.

______________________________________________________________________

## [R0.15] - 2025-07-22

### Hash

67cb748f312f1bc3c95558372576dddcb3ab90d5030ecdd40f8090b42a11f3cd

### Added

`FF_PATH_DEPTH` has been added to the `ffconf.h` and to cppflags.

### Changed

`FFCONF_DEF` (Revision ID) for the new version has been updated.

## [R0.15b] - 2025-06-21

### Hash

02e3ec67aa7721810dc9164a58ea6a9a7780e50021d8583119895c367a6f9fe1

### Added

- Initial FatFS source code
