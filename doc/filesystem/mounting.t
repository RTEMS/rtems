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

Mounting and Unmounting File Systems 

? Characteristics of a Mount Point
? The mount point must be a directory. It may have files and other directories under 
it. These files and directories will be hidden when the file system is mounted.
? The task must have read/write/execute permissions to the mount point or the 
mount attempt will be rejected.
? Only one file system can be mounted to a single mount point.
? The Root of the mountable file system will be referenced by the name of the mount 
point after the mount is complete.
? Mount table chain
? Content of the mount table chain entry

struct rtems_filesystem_mount_table_entry_tt
{
  	Chain_Node                             			Node;
  	rtems_filesystem_location_info_t       		mt_point_node;
  	rtems_filesystem_location_info_t       		mt_fs_root;
  	int                                    			options;
  	void                                  			*fs_info;

  	rtems_filesystem_limits_and_options_t  	pathconf_limits_and_options;

  /*
   *  When someone adds a mounted filesystem on a real device,
   *  this will need to be used.
   *
   *  The best option long term for this is probably an open file descriptor.
   */
  	char                                  			*dev;
};

? Adding entries to the chain during mount()
When a file system is mounted, its presence and location in the file system 
hierarchy is recorded in a dynamic list structure known as a chain. A unique 
rtems_filesystem_mount_table_entry_tt structure is logged for each file system that is 
mounted. This includes the base file system.

? Removing entries from the chain during unmount()
When a file system is dismounted its entry in the mount table chain is extracted 
and the memory for this entry is freed.



