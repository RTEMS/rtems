

System Initialization


	After the RTEMS initialization is performed, the application's initialization will 
be performed. Part of initialization is a call to rtems_filesystem_initialize(). This routine 
will mount the `In Memory File System' as the base file system. 

Mounting the base file system consists of the following:

? Initialization of mount table chain control structure 
? Allocation of a -jnode- structure that will server as the root node of the `In Memory 
File System'
? Initialization of the allocated -jnode- with the appropriate OPS, directory handlers 
and pathconf limits and options.
? Allocation of a memory region for file system specific global management variables
? Creation of first mount table entry for the base file system 
? Initialization of the first mount table chain entry to indicate that the mount point is 
NULL and the mounted file system is the base file system


After the base file system has been mounted, the following operations are performed 
under its directory structure:

? Creation of the /dev directory
? Registration of devices under /dev directory 

