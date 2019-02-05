#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/printk.h>

static ssize_t
chiffrefs_read(struct file *filp,
               char __user *buf, size_t size,
               loff_t *p_off)
{
}

static ssize_t
chiffrefs_write(struct file *filp,
                char const __user *buf, size_t size,
                loff_t *p_off)
{
}

static struct inode_operations const chiffrefs_file_iops =
{
    .setattr = simple_setattr,
    .getattr = simple_getattr,
};

// TODO forward all ops to dev file
static struct file_operations const chiffrefs_file_fops =
{
    .llseek = generic_file_llseek,
    .read = chiffrefs_read,
    .write = chiffrefs_write,
    .open = generic_file_open,
};

static struct inode *
chiffrefs_get_inode(struct super_block *sb,
                    struct inode const *dir, umode_t mode)
{
    struct inode *inode = new_inode(sb);
    if (!inode) return ERR_PTR(-ENOMEM);

    inode->i_ino = get_next_ino();
    inode_init_owner(inode, dir, mode);
    inode->i_mapping->a_ops = &empty_aops;
    inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);

    switch (mode & S_IFMT)
    {
        case S_IFDIR:
            inode->i_op = &simple_dir_inode_operations;
            inode->i_fop = &simple_dir_operations;
            inc_nlink(inode);
            break;
        case S_IFREG:
            inode->i_op = &chiffrefs_file_iops;
            inode->i_fop = &chiffrefs_file_fops;
            break;
        default:
            init_special_inode(inode, mode, 0);
            break;
    }

    return inode;
}

static struct super_operations const super_ops =
{
    .statfs = &simple_statfs,
};

#define CHIFFREFS_MAGIC 0xDeadBeef

static int
chiffrefs_fill_super(struct super_block *sb, void *data, int silent)
{
    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = CHIFFREFS_MAGIC;
    sb->s_op = &super_ops;
    sb->s_root = d_make_root(chiffrefs_get_inode(sb, NULL, S_IFDIR));
    return !sb->s_root ? -ENOMEM : 0;
}

static struct dentry *
chiffrefs_mount(struct file_system_type *fs_type,
                int flags, char const *dev_name,
                void *data)
{
    pr_info("dev name: %s\n", dev_name);
    pr_info("options: %s\n", (char *)data);
    return mount_nodev(fs_type, flags, data, &chiffrefs_fill_super);
}

static struct file_system_type chiffrefs_fs_type =
{
    .name = "chiffrefs",
    .fs_flags = FS_USERNS_MOUNT,
    .mount = &chiffrefs_mount,
    .kill_sb = &kill_anon_super,
    .owner = THIS_MODULE,
};

static __init int
chiffrefs_load(void)
{
    return register_filesystem(&chiffrefs_fs_type);
}

static __exit void
chiffrefs_unload(void)
{
    unregister_filesystem(&chiffrefs_fs_type);
}

module_init(chiffrefs_load);
module_exit(chiffrefs_unload);

MODULE_AUTHOR("Victorien Le Couviour--Tuffet <victorien.lecouviour.tuffet@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ROT cipher VFS");
