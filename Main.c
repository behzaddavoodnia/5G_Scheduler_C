#include "config.h"
#include "Scheduler.h"
//#ifndef PARSER_H
//#include "Parser.h"
//#endif // !PARSER_H
#if !defined __PARSER_H__
#define __PARSER_H__
#endif


int main() {


	//t();

	//strcpy(CSV_name, "eshgh_kon.csv");

	sched_constructor();
	all_config();
	B_level();
	laxity_array_constructor();
	laxity_and_DWF();
	CSV_maker();
	exit(1);
	return 0;
}