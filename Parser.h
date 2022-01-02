#include "Scheduler.h"
#pragma once

#ifndef SCHEDULER_H
#define SCHEDULER_H


/*%%%%%%%%%%%%%%%%%%%%%%%%
* JSON files
%%%%%%%%%%%%%%%%%%%%%%%%%% */
#define HARDWARE_JSON "./conf/architecture_specs.json"
#define APPLICATOIN_JSON "./conf/application_specs_big.json"

/*%%%%%%%%%%%%%%%%%%%%%%%%
* Functions
%%%%%%%%%%%%%%%%%%%%%%%%%% */
void all_config();
void laxity_array_constructor();
void laxity_and_DWF();
#endif // !SCHEDULER_H