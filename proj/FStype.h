#include "ext2type.h"

typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

struct PROC{
	int pid, gid;
	int uid;
	int status;
	struct MINODE *cwd;
	struct OFT *fd[10];
};
struct MINODE{
	INODE ip;
	int dev;
	int ino;
	int refCount;
	int dirty;
	int mounted;
	struct MOUNT *mountptr;
};
struct OFT{
	int mode;
	int refCount;
	struct MINODE *mip;
	int offset;
};
struct MOUNT{
	int dev;
	int nblocks, ninodes;
	int bmap, imap, iblk;
	struct MINODE *mounted_inode;
	char name[64];
	char mount_name[64];
	int refCount;
};

struct PROC *proc[100];
struct MINODE *minode[100];
struct MINODE *root;
struct PROC *running;
struct MOUNT *mounttable[10];
char pathname[128], parameter[60];
