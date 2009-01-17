#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "db.h"

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
	sprintf(a,"file %s",file);
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
	return d;
}
void writedb(dbase *db,char *path){
	db->path=strdup(path);
	writemimes(db);writedirs(db);writefiles(db);
}
void mergedb(dbase * db){
	;
}
void findrec(dbase *basedb,dbase *db, char *file){
	static struct stat st;
	chdir(file);	
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
	int first=1;
	while ((e=readdir(dir))){
		if (strcmp(e->d_name,".")&&strcmp(e->d_name,"..")) {
			lstat(e->d_name,&st);
			if (first){
				#define ATR(x,y) db->dirs->min##y = db->dirs->max##y = st.st_##y;	
#define ATR2(x,y)
#include "atrs.h"
				first=0;
			} else {
				#define ATR(x,y) db->dirs->min##y = (db->dirs->min##y < st.st_##y) ?db->dirs-> min##y : st.st_##y;db->dirs->max##y = (db->dirs->max##y > st.st_##y) ? db->dirs->max##y : st.st_##y;
				//#define ATR2(x,y) db->dirs->min##y = db->dirs->min ## y & st.st_##y; db->dirs->max##y = db->dirs->max##y | st.st_##y;
#define ATR2(x,y)
#include "atrs.h"
			}
			strcpy(db->files->name,e->d_name);
			db->files->mime=getmime(db,e->d_name);
			db->files=nextstruct(db->files);
		}
	}
	closedir(dir);
	ths->midfile=ptrdist(db->files,basedb->files);
	dir=opendir(".");
	while ((e=readdir(dir))){
		if (strcmp(e->d_name,".")&&strcmp(e->d_name,"..")) {
			if (e->d_type==DT_DIR)
				findrec(basedb,db,e->d_name);
		}
	}
	closedir(dir);
	ths->lastfile=ptrdist(db->files, basedb->files);
	ths->lastdir =ptrdist(db->dirs, basedb->dirs);
	chdir("..");
}
dbase *makedb(char *path){
		dbase *d=(dbase *)malloc(sizeof(dbase)),*d2=(dbase *)malloc(sizeof(dbase));
		d->mimes=(mimeinfo *)malloc(100000);
		d->files=(fileinfo *)malloc(100000);
		d->dirs=(dirinfo *)malloc(100000);
		memcpy((char *)d2,(char *)d,sizeof(dbase));
		findrec(d2,d,path);
		return d2;
}

