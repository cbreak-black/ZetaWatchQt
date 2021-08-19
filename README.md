ZetaWatch
=========

![ZetaWatch displaying pool status and filesystems][ZFSImage]

ZetaWatchQt is a small Qt program that displays the zfs status in the menu bar on linux, similar to what the [original ZetaWatch](../../../ZetaWatch) does on MacOS.

It is still a very early prototype, and due to the current state of libzfs and libzfs_core, changes will be required until the API stabilizes. It is only tested on Ubuntu 21.04 with the bundled ZFS.

Currently supported features are:
 * Show pool, vdev, filesystems

Eventually planed features
 * Show pool, vdev, filesystem stats
 * Show pool / filesystem properties
 * Start, stop, pause scrubs, and monitor their progress.
 * Import and Export pools manually, or auto-import when they become available
 * Mount / unmount datasets manually or at pool import automatically
 * Load/Unload encryption keys for encrypted datasets manually or automatically
 * Create, Display, Delete, Clone Snapshots or roll back to them
 * Report errors in notification center when they are discovered


For Developers
==============

Compiling
---------

ZetaWatchQt is compiled with CMake. It requires Qt5 and OpenZFS 2.0.2. It can be built with

    mkdir build
    pushd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j12


ZFS Binary Compatibility
------------------------

Since ZetaWatch directly links to the zfs libraries, it only works if those are
compatible. It is easiest to compile it from scratch for now.


License
=======

This program is copyrighted by me, because I wrote it.
This program is licensed under the "3-clause BSD" License. See the LICENSE.md file for
details.

[ZFSImage]: https://raw.githubusercontent.com/cbreak-black/ZetaWatchQt/master/doc/ZetaWatchQt.jpg
