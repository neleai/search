#include "db.h"
int main(){
	dbase* db=makedb(".");
	writedb(db,"/var/lib/search");
	return 0;
}
