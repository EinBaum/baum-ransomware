#include "baumcommon.h"

char baumcommon_verbose = 0;
char baumcommon_test = 0;
char baumcommon_force = 0;

void baumcommon_setverbose(char b) {
	baumcommon_verbose = b;
}
void baumcommon_settest(char b) {
	baumcommon_test = b;
}
void baumcommon_setforce(char b) {
	baumcommon_force = b;
}
