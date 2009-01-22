#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "db.h"
#include "compress.h"
#define readary(fd,ary) size=fdsize(fd); ary=(typeof(ary)) malloc(size);read(fd,ary,size);
int fdsize (int fd ){	static struct stat st; fstat(fd,&st);return st.st_size;}
#define writeary(fd,ary) structsize(ary); write(fd,ary,size);
char str3[10000];
#define OPENCAT(fd,str,str2) strcpy(str3,str);strcat(str3,"/");strcat(str3,str2);fd=open(str3,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
int size;
void readmimes(dbase *db){
	int fd;	
	OPENCAT(fd,db->path,"mime.db");	readary(fd,db->mimes);
	close(fd);
}
void readdirs(dbase *db){
	int fd;
	OPENCAT(fd,db->path,"dir.db"); readary(fd,db->dirs);
	close(fd);
}
void readfiles(dbase *db){
	int fd;
	OPENCAT(fd,db->path,"file.db");	readary(fd,db->files);
	close(fd);
}
void writemimes(dbase *db){
	int fd;
	OPENCAT(fd,db->path,"mime.db");	writeary(fd,db->mimes);
	close(fd);
}
void writedirs(dbase *db){
	int fd;
	OPENCAT(fd,db->path,"dir.db"); writeary(fd,db->dirs);
	close(fd);
}
void writefiles(dbase *db){
	int fd;
	OPENCAT(fd,db->path,"file.db");	writeary(fd,db->files);
	close(fd);
}
short int getmime(dbase *db,char *file){
	char a[1000];
	sprintf(a,"file \"%s\"",file);
	FILE *f=popen(a,"r");
	fscanf(f,"%s",a);fscanf(f," ");
	fscanf(f,"%[^,]",a);
	mimeinfo *m=db->mimes;int i=1;
	while(*m->name){
		if (!strcmp(a,m->name)) goto e;
		i++;
		m=nextstruct(m);
	}
	strcpy(m->name,a);
	e: pclose(f);
	return i;
}
dbase *readdb(char *path){
	dbase *d=malloc(sizeof(dbase));
	d->path=strdup(path);
	readmimes(d);readdirs(d);readfiles(d);
	d->mimep=d->mimes;d->filep=d->files;d->dirp=d->dirs;
	return d;
}
dbase *dbdup(dbase *d){dbase *b=(dbase *)malloc(sizeof(dbase));memcpy(b,d,sizeof(dbase));return b; }
void writedb(dbase *db,char *path){
	db->path=strdup(path);
	writemimes(db);writedirs(db);writefiles(db);
}

char filebuf[1000];
char fsort[1000000];
char *fils[10000],*dirp[10000];

	static int
cmpstring(const void *p1, const void *p2)
{	return strcmp(* (char * const *) p1, * (char * const *) p2);}
/*void findrec(dbase *basedb,dbase *db, char *file){int i;
	static struct stat st;
	if (chdir(file)	) return;
	lstat(".",&st);
#define ATR(x,y) db->dirs->y=st.st_##y;
#define ATR2(x,y)
#include "atrs.h"
	db->dirs->firstfile=ptrdist(db->files, basedb->files);
	strcpy(db->dirs->name,file);
	dirinfo *ths=db->dirs;
	db->dirs=nextstruct(db->dirs);

	DIR * dir=opendir(".");
	struct dirent *e;
	*filebuf=0;
	char *fp=fsort;int fn=0,dn=0;
	while ((e=readdir(dir))){
		if (strcmp(e->d_name,".")&&strcmp(e->d_name,"..")) {
			if (e->d_type==DT_DIR)dirp[dn++]=fp;
			strcpy(fp,e->d_name); fils[fn++]=fp;fp+=strlen(fp)+1;
		}
	}
	closedir(dir);
	qsort(fils,fn,sizeof(char*),cmpstring);
	for(i=0;i<fn;i++){
		strcpy(db->filep->name,fils[i]);
		compress(db->filep->name,filebuf);
		db->filep->mime=0;
		db->filep=nextstruct(db->filep);
	}
	qsort(dirp,dn,sizeof(char*),cmpstring);

}*/
void copydir(dbase *old,dbase *ne,char *path,int rec){int i;
	if (chdir(path))return;
	static struct stat st;
	lstat(".",&st);
	ne->dirp->firstfile=ptrdist( ne->lfile,ne->files);
	ne->dirp->firstdir=ptrdist( ne->ldir,ne->dirs);
	if ((!strcmp(old->dirp->name,ne->dirp->name)&&(!rec|| old->dirp->mtime==st.st_mtime))){
		memcpy(ptrmov(ne->files,ne->dirp->firstfile),ptrmov(old->files,old->dirp->firstfile),old->dirp->filesize );
		ne->dirp->filesize=old->dirp->filesize;
		memcpy(ptrmov(ne->dirs,ne->dirp->firstdir),ptrmov(old->dirs,old->dirp->firstdir),old->dirp->dirsize );
		ne->dirp->dirsize=old->dirp->dirsize;
		ne->ldir=ptrmov(ne->dirs,ne->dirp->firstdir+ ne->dirp->dirsize);
		ne->lfile=ptrmov(ne->files,ne->dirp->firstfile+ ne->dirp->filesize);
		ne->dirp->mtime=old->dirp->mtime;
	}else{
		if (!rec)return;
		ne->dirp->mtime=st.st_mtime;
		fileinfo *lfp=ne->lfile;
		DIR * dir=opendir(".");
		struct dirent *e;
		*filebuf=0;
		char *fp=fsort;int fn=0,dn=0;
		while ((e=readdir(dir))){
			if (strcmp(e->d_name,".")&&strcmp(e->d_name,"..")) {
				if (e->d_type==DT_DIR)dirp[dn++]=fp;
				strcpy(fp,e->d_name); fils[fn++]=fp;fp+=strlen(fp)+1;
			}
		}
		closedir(dir);
		qsort(fils,fn,sizeof(char*),cmpstring);
		for(i=0;i<fn;i++){
			strcpy(lfp->name,fils[i]);
			compress(lfp->name,filebuf);
		//	lfp->mime=0;
			lfp=nextstruct(lfp);
		}
		ne->dirp->filesize=ptrdist(lfp,ne->lfile);
		ne->lfile=lfp;
		qsort(dirp,dn,sizeof(char*),cmpstring);
		dirinfo *di=ne->ldir;
		for(i=0;i<dn;i++){
			di->mtime=di->firstfile=di->filesize=di->firstdir=di->dirsize=0;
			strcpy(di->name,dirp[i]);strcat(di->name,"/");
			di=nextstruct(di);
		}
		ne->dirp->dirsize=ptrdist(di,ne->ldir);
		ne->ldir=di;
	}

}

dbase *createdb(){
	dbase *d=(dbase *)malloc(sizeof(dbase));
	d->mimes=(mimeinfo *)calloc(1000000,1);
	d->files=(fileinfo *)calloc(10000000,1);
	d->dirs=(dirinfo *)calloc(1000000,1);
	d->ldir=d->dirp=d->dirs;
	d->mimep=d->mimes;
	d->lfile=d->filep=d->files;
	return d;
}
dbase *makedb(char *path){
		dbase *d=createdb() ,*d2=(dbase *)malloc(sizeof(dbase));
//		findrec(d2,d,path);
		return d2;
}

