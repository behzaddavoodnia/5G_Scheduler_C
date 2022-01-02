#include "CSV.h"
#include "Scheduler.h"

void open_file(char file_to_open[]) {
	strcpy(file_name, file_to_open);
	FILE* fp;
	fp = fopen(file_name, "wb");
}

void add_string_to_CSV(STRING item_to_add, bool is_end_of_line) {
	FILE* fp;
	fp = fopen(file_name, "a+");
	fprintf(fp, item_to_add);
	if (!is_end_of_line)
		fprintf(fp, " ; ");
	fclose(fp);
}

void add_int_to_CSV(int item_to_add, bool is_end_of_line) {
	char s_number[MAX_KEY_LENGTH];
	snprintf(s_number, MAX_KEY_LENGTH, "%d", item_to_add);
	add_string_to_CSV(s_number, is_end_of_line);
}

void add_float_to_CSV(double item_to_add, bool is_end_of_line) {
	char s_number[MAX_KEY_LENGTH];
	gcvt(item_to_add, 10, s_number);
	add_string_to_CSV(s_number, is_end_of_line);
}

void new_line_CSV() {
	FILE* fp;
	fp = fopen(file_name, "a+");
	fprintf(fp, "\n");
	fclose(fp);
}

void cluster_title() {
	add_string_to_CSV("Job name", false);
	char buffer[MAX_KEY_LENGTH];
	for (int i = 0; i < TOTAL_CLUSTERS / 2 - 1; i++) {
		snprintf(buffer, MAX_KEY_LENGTH, "Cluster %d", i);
		add_string_to_CSV(buffer, false);
	}
	snprintf(buffer, MAX_KEY_LENGTH, "Cluster %d", 7);
	add_string_to_CSV(buffer, true);
	new_line_CSV();
}

void CSV_maker() {
	char CSV_name[MAX_KEY_LENGTH];
	strcpy(CSV_name, "Longest path.csv");
	open_file(CSV_name);
	add_string_to_CSV("Job name", false);
	add_string_to_CSV("Longest path", true);
	new_line_CSV();
	job* job_ptr = head_of_job_list;
	while (job_ptr != NULL) {
		add_string_to_CSV(job_ptr->job_name, false);
		add_string_to_CSV(job_ptr->longest_path, true);
		new_line_CSV();
		job_ptr = job_ptr->next_jobs_in_queue;
	}
	
	
	strcpy(CSV_name, "Cluster assignment.csv");
	open_file(CSV_name);
	add_string_to_CSV("Cluster to run", false);
	cluster_title();
	for (int i = 0; i < TOTAL_JOBS; i++) {
		add_int_to_CSV(laxity_array[i]->cluster_host_id, false);
		add_string_to_CSV(laxity_array[i]->job_name, false);
		for (int j = 0; j < TOTAL_CLUSTERS / 2-1; j++)
			add_float_to_CSV(sum_of_all_configs[i][j], false);
		add_float_to_CSV(sum_of_all_configs[i][7], true);
		new_line_CSV();
	}

	strcpy(CSV_name, "Comunication.csv");
	open_file(CSV_name);

	cluster_title();
	for (int i = 0; i < TOTAL_JOBS; i++) {
		add_string_to_CSV(laxity_array[i]->job_name, false);
		for (int j = 0; j < TOTAL_CLUSTERS / 2 - 1; j++) {
			add_float_to_CSV(alpha[i][j], false);
		}
		add_float_to_CSV(alpha[i][7], true);
		new_line_CSV();
	}

	strcpy(CSV_name, "Affinity.csv");
	open_file(CSV_name);
	cluster_title();
	add_string_to_CSV("", true);
	for (int i = 0; i < TOTAL_JOBS; i++) {
		add_string_to_CSV(laxity_array[i]->job_name, false);
		for (int j = 0; j < TOTAL_CLUSTERS / 2 - 1; j++) {
			add_float_to_CSV(beta[i][j], false);
		}
		add_float_to_CSV(beta[i][7]*C2, true);
		new_line_CSV();
	}

	strcpy(CSV_name, "Utilization.csv");
	open_file(CSV_name);
	cluster_title();
	add_string_to_CSV("", true);
	for (int i = 0; i < TOTAL_JOBS; i++) {
		add_string_to_CSV(laxity_array[i]->job_name, false);
		for (int j = 0; j < TOTAL_CLUSTERS / 2 - 1; j++) {
			add_float_to_CSV(gamma[i][j]*C3, false);
		}
		add_float_to_CSV(gamma[i][7], true);
		new_line_CSV();
	}

}
