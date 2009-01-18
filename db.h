#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#define ptrdist(a,b) (((char *)a)-((char *)b))
#define ptrmov(a,b) ((typeof(a)) (((char *)(a))+b))
#define nextstruct(p) ((typeof(p)) (((char *)(p))+sizeof(*p)+1+strlen(((char *)(p))+sizeof(*p) )))
#define structsize(p) {typeof(p) _r=p; while (_r->name[0]) _r=nextstruct(_r); size=ptrdist(_r,p)+sizeof(*p)+1; }
typedef struct {
	char name[1];
} mimeinfo;
typedef struct {
  /*#define ATR(x,y) x y , min##y , max##y;
	#define ATR2(x,y)
	#include "atrs.h"*/
	int mtime;
	int firstfile,filesize,firstdir,dirsize;
	char name[1];
} dirinfo;
typedef struct {
	short int mime;
	char name[1]; 
} fileinfo;
typedef struct {
	char *path;
	mimeinfo *mimes,*mimep;
	dirinfo *dirs,*dirp,*ldir;
	fileinfo *files,*filep,*lfile;
} dbase;
dbase *makedb(char *path);
dbase *readdb(char *path);
dbase * dbdup(dbase *d);
dbase *createdb();
void copydir(dbase *old,dbase *ne,char *path,int rec);
void writedb(dbase * db,char *path);
