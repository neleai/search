#ifndef ATR3
#define ATR3(x,y)
#endif
#ifndef ATR2
#define ATR2(x,y) ATR(x,y)
#endif
/*in each directory we remember minimal and maximal value*/
ATR(off_t     ,size);    /* total size, in bytes */
ATR(time_t    ,mtime);   /* time of last modification */

/*needs special treatment because we and/or */
ATR2(mode_t   ,mode);    /* protection */

/* Remember these values only makes database bigger.*/
ATR3(time_t    ,ctime);   /* time of last status change */
ATR3(uid_t     ,uid);     /* user ID of owner */
ATR3(gid_t     ,gid);     /* group ID of owner */
ATR3(nlink_t   ,nlink);   /* number of hard links */
ATR3(time_t    ,atime);   /* time of last access */
ATR3(blksize_t, blksize); /* blocksize for file system I/O */
ATR3(blkcnt_t , blocks);  /* number of blocks allocated */
ATR3(dev_t,     dev);     /* ID of device containing file */
ATR3(ino_t,     ino);     /* inode number */
ATR3(dev_t,     rdev);    /* device ID (if special file) */

#undef ATR
#undef ATR2
#undef ATR3
