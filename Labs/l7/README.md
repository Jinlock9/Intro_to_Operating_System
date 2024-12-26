# l7

## 3 Fixing Dadfs

### ECE4820 Tasks

1. What is a kernel module, and how does it differ from a regular library?

        A kernel module is a piece of code that can be loaded into the kernel of an operating system to extend its functionality without rebooting the system. It operates in kernel space, meaning it has direct access to the kernel's resources, which allows it to interact closely with hardware and perform low-level tasks.  

- Differences between a kernel module and a regular library:

    - Kernel modules operate in kernel space, while regular libraries run in user space.

    - Kernel modules are used to add system-level functionalities, while regular libraries are typically used to provide reusable functions for applications.

    - Kernel modules can be loaded or unloaded dynamically into the kernel as needed, while regular libraries are linked with applications at compile-time or runtime but do not interact directly with the kernel. 

2. How to compile a kernel module?

    Refer to [https://linux-kernel-labs.github.io/refs/heads/master/labs/kernel_modules.html#:~:text=A%20kernel%20module%20(or%20loadable,the%20form%20of%20kernel%20modules](https://linux-kernel-labs.github.io/refs/heads/master/labs/kernel_modules.html?highlight=kernel%20module) :

    - Kernel Modules should not be linked to libraries.

    - Kernel Modules must be compiled with the same options as the kernel that loads the modules.

    - Kernel Modules should use **Kbuild**:

        
        ```Makefile
        # Makefile

        KDIR = /lib/modules/`uname -r`/build

        kbuild:
                make -C $(KDIR) M=`pwd`

        clean:
                make -C $(KDIR) M=`pwd` clean
        ```

        ```Makefile
        # Kbuild

        EXTRA_CFLAGS = -Wall -g

        obj-m        = modul.o
        ```


3. How are `mutex` defined and used? How good is this approach?

- `dadfs_sb_lock`: Controls access to the superblock for operations such as updating free blocks and inode counts.
- `dadfs_inodes_mgmt_lock`: Manages access to the inode management functions, like adding or counting inodes.
- `dadfs_directory_children_update_lock`:  Prevents concurrent modifications to the directory contents when new files or directories are added.

        By using different mutexes for distinct areas (superblock, inodes, and directory updates), the approach minimizes the risk of deadlocks, as each mutex handles a separate part of the filesystem.

4. Based on the source code, how is information shared between the kernel and user spaces?

        The kernel allocates memory, which the userspace process then maps into its address space using `mmap()`. The kernel thread and userspace process use different pointers to access this memory: one in the kernel address space and the other in the userspace address space.

5. Following dad's advice ensure the part of the code the poses problem is only processed when working with older kernels. At this stage nothing needs to be done for newer ones. Ensure the module properly compiles on a newer kernel, although it cannot be of any use at this stage.

- Build: `make CC=clang`

### ECE4821 Tasks

6. Write and test all the commands that are only hinted in the README file. He seizes the opportunity to emphasise on the importance of a complete, precise, and accurate documentation.     

7. Document your changes in the README file.

- `dadfs_write`:
```Cpp
// # 355 - 361
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)	
ssize_t dadfs_write(struct file * filp, const char __user * buf, size_t len,
		       loff_t * ppos)
#else
static ssize_t dadfs_write(struct kiocb * kiocb, struct iov_iter * iov_iter)
#endif
{

// # 376 - 380
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)	
	sb = filp->f_path.dentry->d_inode->i_sb;
#else
	sb = kiocb->ki_filp->f_inode->i_sb;
#endif

// # 386 - 390
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)		
	retval = generic_write_checks(filp, ppos, &len, 0);
#else
	retval = generic_write_checks(kiocb, iov_iter);
#endif	

// # 394 - 398
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	inode = filp->f_path.dentry->d_inode;
#else
	inode = kiocb->ki_filp->f_inode;
#endif

// # 401 - 407
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	bh = sb_bread(filp->f_path.dentry->d_inode->i_sb,
					    sfs_inode->data_block_number);
#else
	bh = sb_bread(kiocb->ki_filp->f_inode->i_sb,
					    sfs_inode->data_block_number);
#endif

// # 417 - 421
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	buffer += *ppos;
#else
	buffer += kiocb->ki_pos;
#endif

// # 430 - 434
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	if (copy_from_user(buffer, buf, len)) {
#else
	if (copy_from_iter(buffer, iov_iter->count, iov_iter) == 0) {
#endif

// # 440 - 444
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)	
	*ppos += len;
#else
	kiocb->ki_pos += iov_iter->count;
#endif

// # 472 - 476
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	sfs_inode->file_size = *ppos;
#else
	sfs_inode->file_size = kiocb->ki_pos;
#endif

// # 478 - 492
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	if (retval) {
		len = retval;
	}
	mutex_unlock(&dadfs_inodes_mgmt_lock);

	return len;
#else
	if (retval) {
    	iov_iter->count = retval;
	}
	mutex_unlock(&dadfs_inodes_mgmt_lock);
	
	return iov_iter->count;
#endif
}
```

- `dadfs_dir_operations`:
```Cpp
// # 506 - 521
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 0, 6)
	.iterate_shared = dadfs_iterate,
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
	.iterate = dadfs_iterate,
#else
	.readdir = dadfs_readdir,
#endif
```

- `dadfs_create`, `dadfs_mkdir`:
```Cpp
// # 518 - 527
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
static int dadfs_create(struct inode *dir, struct dentry *dentry,
			   umode_t mode, bool excl);

static int dadfs_mkdir(struct inode *dir, struct dentry *dentry,
			  umode_t mode);
#else
static int dadfs_create(struct mnt_idmap *idmap, struct inode *dir, struct dentry *dentry, umode_t mode, bool excl);
static int dadfs_mkdir(struct mnt_idmap *idmap, struct inode *dir, struct dentry *dentry, umode_t mode);
#endif

// # 668 - 674
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
static int dadfs_mkdir(struct inode *dir, struct dentry *dentry,
			  umode_t mode)
#else
static int dadfs_mkdir(struct mnt_idmap *idmap, struct inode *dir, 
			struct dentry *dentry, umode_t mode)
#endif

// # 681 - 687
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
static int dadfs_create(struct inode *dir, struct dentry *dentry,
			   umode_t mode, bool excl)
#else
static int dadfs_create(struct mnt_idmap *idmap, struct inode *dir, 
				struct dentry *dentry, umode_t mode, bool excl)
#endif
```

- `dadfs_create_fs_object`:
```Cpp
// # 582 - 586
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
#else
	inode->__i_atime = inode->__i_mtime = inode->__i_ctime = current_time(inode);
#endif

// # 657 - 661
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	inode_init_owner(inode, dir, mode);
#else
	inode_init_owner(NULL, inode, dir, mode);
#endif
```

- `dadfs_iget`:
```Cpp
// # 713 - 719
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	inode->i_atime = inode->i_mtime = inode->i_ctime =
			current_time(inode);
#else
	inode->__i_atime = inode->__i_mtime = inode->__i_ctime =
			current_time(inode);
#endif
```

- `dadfs_lookup`:
```Cpp
// # 753 - 757
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
			inode_init_owner(inode, parent_inode, DADFS_INODE(inode)->mode);
#else
			inode_init_owner(NULL, inode, parent_inode, DADFS_INODE(inode)->mode);
#endif
```

- `dadfs_load_journal`:
```Cpp
// # 800 - 802
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	char b[BDEVNAME_SIZE];
#endif

// # 809 - 823
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	printk(KERN_INFO "Journal device is: %s\n", __bdevname(dev, b));

	bdev = blkdev_get_by_dev(dev, FMODE_READ|FMODE_WRITE|FMODE_EXCL, sb);
	
	if (IS_ERR(bdev))
		return 1;
#else
	bdev = blkdev_get_no_open(dev);
    
	if (IS_ERR(bdev))
		return 1;

    printk(KERN_INFO "Journal device is: %pg\n", bdev);
#endif
```

- `dadfs_fill_super`:
```Cpp
// # 968 - 972
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	inode_init_owner(root_inode, NULL, S_IFDIR);
#else
	inode_init_owner(NULL, root_inode, NULL, S_IFDIR);
#endif

// # 976 - 982
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime =
	    current_time(root_inode);
#else
	root_inode->__i_atime = root_inode->__i_mtime = root_inode->__i_ctime =
	    current_time(root_inode);
#endif

// # 987 - 993
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)
	sb->s_root = d_make_root(root_inode);
#else
	sb->s_root = d_alloc_root(root_inode);
	if (!sb->s_root)
		iput(root_inode);
#endif
```

- `dadfs_init`:
```Cpp
// # 1058 - 1070
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 6)
	sfs_inode_cachep = kmem_cache_create("sfs_inode_cache",
	                                     sizeof(struct dadfs_inode),
	                                     0,
	                                     (SLAB_RECLAIM_ACCOUNT| SLAB_MEM_SPREAD),
	                                     NULL);
#else
	sfs_inode_cachep = kmem_cache_create("sfs_inode_cache",
                                     sizeof(struct dadfs_inode),
                                     0,
                                     SLAB_RECLAIM_ACCOUNT,
                                     NULL);
#endif

```