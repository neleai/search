#include "db.h"
int main(){
	dbase* db=makedb("/pub/mp3");
	writedb(db,"/var/lib/search");
	return 0;
}
