#include "db.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fnmatch.h>
#include <pcre.h>
#include <limits.h>
#include "compress.h"
char matchmime[30000];
char *searchpath;
int statless(struct stat *a,struct stat *b){
	#define ATR(x,y) if(a->st_##y < b->st_##y)return 1;
	#define ATR3(x,y) if(a->st_##y < b->st_##y)return 1;
	#define ATR2(x,y) if((a->st_##y & b->st_##y)== a->st_##y) return 1;
#include "atrs.h"
	return 0;
}

char *namepattern;
int comparestat(char *name){
	return 1;
}

void minmaxstat(struct stat *b,struct stat *a){/*dont know better way how get infinity*/
#define ATR3(x,y) ATR(x,y)
#define ATR(x,y) b->st_##y=0; a->st_##y=1; while (2*a->st_##y+1>a->st_##y) a->st_##y=2*a->st_##y+1;
#include "atrs.h"
}
typedef struct{
	pcre *re;
	pcre_extra *rex;
	char *wild;
} pattern;

int ovec[30];char *lastpat;
char outptr[100];
char substbuf[1000];
char *substmatches(char *path, char *name){
	char *b=substbuf,*op=outptr;int r;if (*outptr=='#') op++;
	while (*op){
		*b=0;
		switch (*op){
			case '%':
				op++;
				switch (*op){
					case '%':
					*b++='%';
					break;
					case '*':
					strcat(b, path);strcat(b,name);
					b+=strlen(b);
					break;
					default:
					r=*op- '0';
					strncpy(b,lastpat+ovec[2*r],ovec[2*r+1]-ovec[2*r]);
					b+=ovec[2*r+1]-ovec[2*r];
				}
				op++;
				break;
			default:
				*b++=*op++;
				break;
		}
	}
	*b=0;
	return substbuf;
}
void output(char *path,char *name){
	if (*outptr=='#')
		printf(substmatches(path,name));
	else
		system(substmatches(path,name));
}
pattern pat[20];
int matchpattern(pattern pat,char *chr){lastpat=chr;
	if (pat.re) {
		int r=pcre_exec(pat.re,pat.rex,chr,strlen(chr),0,0,ovec,30);
		return r>=0;
	}
	if (pat.wild) return fnmatch(pat.wild,chr,0)==0;
	return 1;
}
int patternid(char *name)
{int i=0;
#define PATTERN(nam) if(!strcmp(#nam,name)) return i; i++;
#include "searchtype.h"
return -42;
}
int needmime,needstat;
/*void rquery(dbase *basedb,dbase *db,char *path){
	int l=strlen(path);
	path[l]='/';path[l+1]=0;strcat(path,db->dirs->name);
	db->files=ptrmov(basedb->files,db->dirs->firstfile);
	if(!matchpattern(pat[patternid("path")],path))goto e;
	while (db->files!=ptrmov(basedb->files,db->dirs->midfile)){
		decompress(db->files->name, filebuf);
//		if(needmime && !matchmime[db->files->mime] )goto n;
		if(!matchpattern(pat[patternid("name")],filebuf))goto n;
		if(needstat && !comparestat(filebuf))goto n;
		output(path,filebuf);
n:			db->files=nextstruct(db->files);
	}
e:;	dirinfo *enddir=ptrmov(basedb->dirs,db->dirs->lastdir);
	while(db->dirs!=enddir){
		db->dirs=nextstruct(db->dirs);
		if (db->dirs==enddir) break;
	 	rquery(basedb,db,path);
	}
	path[l]=0;
}
void query(dbase *db){
	char path[10000];memset(path,10000,0);
	dbase d2;
	strncpy((char *)&d2,(char *)db,sizeof(dbase));
	rquery(&d2,db,path);
}*/
char filebuf[10000];
void scanfiles(dbase *db,char *path){
	db->filep=ptrmov(db->files,db->dirp->firstfile);
	*filebuf=0;
	if(!matchpattern(pat[patternid("path")],path))return;
	while (db->filep!=ptrmov(db->files,db->dirp->firstfile+db->dirp->filesize)){
		decompress(db->filep->name, filebuf);
	//	if(needmime && !matchmime[db->filep->mime] )goto n;
		if(!matchpattern(pat[patternid("name")],filebuf))goto n;
		if(needstat && !comparestat(filebuf))goto n;
		output(path,filebuf);
n:	db->filep=nextstruct(db->filep);
	}
}
void query(dbase *olddb,dbase *newdb,char *bpath,char *tpath){
	char *cpath=bpath+strlen(bpath);int cont=0;
	if (! strncmp(bpath,tpath,strlen(bpath))) cont=1;
	if (! strncmp(bpath,tpath,strlen(tpath))) cont=2;
	copydir(olddb,newdb,bpath,cont);
	if (cont==2) scanfiles(newdb,bpath); 
	dirinfo *ldi= ptrmov(newdb->dirs,newdb->dirp->firstdir+ newdb->dirp->dirsize);
	dirinfo *di = ptrmov(newdb->dirs,newdb->dirp->firstdir);
	dirinfo *oldi=ptrmov(olddb->dirs,olddb->dirp->firstdir+ olddb->dirp->dirsize);
	dirinfo *odi =ptrmov(olddb->dirs,olddb->dirp->firstdir);
	if (strcmp(olddb->dirp->name,newdb->dirp->name)) oldi=odi=olddb->dirs;
	for(;di!=ldi;di=nextstruct(di)){
		while(odi!=oldi&&strcmp(odi->name,di->name)==-1 ) odi=nextstruct(odi);
		olddb->dirp=(odi!=oldi)?odi:olddb->dirs;
		newdb->dirp=di;
		strcpy(cpath,di->name);	
		query(olddb,newdb,bpath,tpath);
	}
}
pattern getpattern(char *p,int *n){
	pattern pat;pat.re=NULL;pat.wild=NULL;
	if (*p=='/'){
		char *p2=p;
		while (*p2=='\\'||*(p2+1)!='/'){p2++; if (!p2)exit(1);}
		p2++;
		*n=p2-p+1;
		*(p2)=0;
		int erro;
		const char *err;
		pat.re=pcre_compile(p+1,0,&err,&erro,NULL);
		pat.rex=pcre_study(pat.re,0,&err);
	}else{
		char *p2=p;
		while (*p2!=' '&&*p2)p2++;
		*p2=0;
		pat.wild=strdup(p);
		*n=p2-p+1;
	}
	return pat;
}
char *dates;

int main(int argc,char *argv[]){
	strcat(outptr, "#\"%*\"\n");
	int i; char *p,s[1000];s[0]=0;int n;
	for ( i=1;i<argc;i++) {strcat(s,argv[i]);strcat(s," ");}	s[strlen(s)-1]=0;
	p=s;
	while(*p){
		char c;
		int i=0;
		#define PATTERN(name) if (sscanf(p," -" #name "%c%n",&c,&n )) {p+=n;\
			pat[i]=getpattern(p,&n);p+=n; \
			continue;} i++;
#include "searchtype.h"
		switch (*p){
			case '{':
				p++;char *op=outptr;
				while (*p!='}')
					*op++=*p++;
				*op=0;p++;
				break;
			case ' ':
				p++;break;
			default:
				pat[patternid("name")]=getpattern(p,&n);p+=n;	
		}
			
		}
	char pathbuf[10000];
	dbase *db,*newdb=createdb();
	db=readdb("~");
	searchpath=realpath(".",NULL);
	*(newdb->dirs->name)='/';*pathbuf='/';
	newdb->ldir=nextstruct(newdb->ldir);	
	query(db,newdb,pathbuf,searchpath);
	writedb(newdb,"~");
	return 0;
}
