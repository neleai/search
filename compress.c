#include "compress.h"
#include <string.h>
char cbuf[1000];
void compress(char *str,char *buf){char *s=str,*b=buf;
	if (*s!=*b)return;
	int i=0;while (*s && *s++==*b++ )i++;
	*cbuf=128+i;
	strcpy(cbuf+1,s);
	strcpy(buf,str);
	strcpy( str,cbuf);
}
void decompress(char *str,char *buf ){
	if (*((uchar *) str)>127)
		strcpy(buf+*((uchar *)str)-127,str+1);
	else
		strcpy(buf,str);
}
