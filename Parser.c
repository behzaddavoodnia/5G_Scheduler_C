#include "Parser.h"


/* * Configuring */
void all_config() {
	char* json_arch_specs = _json_to_str(HARDWARE_JSON); // Hardware specs
	char* json_app_specs = _json_to_str(APPLICATOIN_JSON);// App specs
	remove_white_spaces(json_app_specs);
	remove_white_spaces(json_arch_specs);
	get_DAGs(json_app_specs);
	free(json_app_specs);
	create_board(json_arch_specs);
	free(json_arch_specs);
	reverse_order_DAG();
	reverse_order_job();
	create_bidirectional_job_queue();
	child_pointer_update();
	predecessors_pointer_update();
	DWF_constructor();
}

/* * Construct and initilize laxity array */
void laxity_array_constructor() {
	job* jobptr = head_of_job_list;
	int job_num = 0;
	while (jobptr)
	{
		laxity_array[job_num++] = jobptr;
		calculate_total_CT(jobptr);
		calculate_total_EET(jobptr);
		laxity_calculation(jobptr);
		jobptr = jobptr->next_jobs_in_queue;

	}
	
}

/* * Final DWF and sorting laxity */
void laxity_and_DWF() {
	START_TIME;
	heap_sort(laxity_array, TOTAL_JOBS);
	END_TIME_FOR("HEAP SORT");
	
	START_TIME;
	for (int i = 0; i < TOTAL_JOBS; i++) {
		DWF_calculation(laxity_array[i]);
	}
	END_TIME_FOR("DWF");
}