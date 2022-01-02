#include "DWF.h"

/* Clusters utilization array */
float clusters_util[TOTAL_CLUSTERS / 2];

void DWF_constructor()
{
	for (int i = 0; i < TOTAL_CLUSTERS / 2; i++)
		clusters_util[i] = 0.0f;
}

/* DWF calculation function*/
void DWF_calculation(job* input_job) {

	for (int i = 0; i < TOTAL_CLUSTERS / 2; i++) {
		alpha[DWF_parameters_counter][i] = 0;
		beta[DWF_parameters_counter][i] = 0;
		gamma[DWF_parameters_counter][i] = 0;
	}
	//printf("\n");
	for (int i = 0; i < (TOTAL_CLUSTERS / 2); i++) {
		sum_of_all_configs[DWF_parameters_counter][i] = 0;
		alpha[DWF_parameters_counter][i] = calculate_alpha(input_job, cluster_array[i]);
		beta[DWF_parameters_counter][i] = calculate_beta(input_job, cluster_array[i]);
		gamma[DWF_parameters_counter][i] = calculate_gamma(i);
		sum_of_all_configs[DWF_parameters_counter][i] += (alpha[DWF_parameters_counter][i] * C1
			+ beta[DWF_parameters_counter][i] * C2 + gamma[DWF_parameters_counter][i] * C3);
		if (input_job->parent_DAG->default_cluster == i)
			sum_of_all_configs[DWF_parameters_counter][i] += Epsilon;
		//printf("  %f \n", sum_of_all_configs[i]);
	}
	float max_conf = 0;
	int index_of_max = 0;
	for (int i = 0; i < TOTAL_CLUSTERS / 2; i++)
	{
		if (sum_of_all_configs[DWF_parameters_counter][i] > max_conf)
		{
			max_conf = sum_of_all_configs[DWF_parameters_counter][i];
			index_of_max = i;
		}
	}
	//printf("%d index and %f max_conf\n", index_of_max, max_conf);
	//for (int i = 0; i < TOTAL_CLUSTERS; i++)
	//	printf("%d\n", max_conf);
	end_DWF(input_job, cluster_array[index_of_max]);
	//printf("\n %s assign to cluster %d and max config is %f\n", input_job->job_name, index_of_max, max_conf);
	//printf("\n=====================================================\n");
	DWF_parameters_counter++;
}

/* Average utilization function */
float util_avg()
{
	float avg = 0;
	for (int i = 0; i < (TOTAL_CLUSTERS / 2); i++)
		avg += clusters_util[i];
	avg /= (TOTAL_CLUSTERS / 2);
	return avg;
}

float calculate_alpha(job* recent_job, cluster* recent_cluster)
{
	float intra_size = recent_cluster->intra_com_size;
	float alpha = 0;
	for (int i = 0; i < MAX_RELATIVE_NODES; i++)
	{
		if (recent_job->recent_parents[i] == NULL)
			break;
		if (recent_job->recent_parents[i]->cluster_host_id == recent_cluster->cluster_id)
			alpha += intra_size;
		else
			alpha -= recent_cluster->inter_com_size[recent_job->recent_parents[i]->cluster_host_id];
	}
	return alpha < 0 ? 0 : alpha;
}

float calculate_beta(job* recent_job, const cluster* recent_cluster)
{
	float is_on_cluster = is_job_on_cluster_history(recent_job, recent_cluster) ? 1 : 0;
	return recent_job->job_size * is_on_cluster * recent_cluster->job_type_history_counter[recent_job->job_type];
}

float calculate_gamma(int util_index)
{
	float gamma = util_avg() - clusters_util[util_index];
	if (gamma < 0)
		return 0;
	else
		return gamma;
}

void end_DWF(job* recent_job, cluster* recent_cluster) {
	recent_job->cluster_host_id = recent_cluster->cluster_id;
	assign_job_id_to_cluster_history(recent_job, recent_cluster);
	clusters_util[recent_cluster->cluster_id] += recent_job->job_size;
}

int get_CT_from_parent_name(job* recent_job, char* parent_name)
{
	job* par = NEW_AND_INITIALIZE(job*, MAX_RELATIVE_NODES);
	for (int i = 0; i < MAX_RELATIVE_NODES; i++)
		if (strcmp(recent_job->recent_parents[i]->job_name, parent_name) == 0)
		{
			par = recent_job->recent_parents[i];
			break;
		}
	for (int i = 0; i < MAX_RELATIVE_NODES; i++)
		if (strcmp(par->childs_of_this_job[i], recent_job->job_name) == 0)
			return par->next_jobs_com_time_in_DAG[i];
}

int get_CT_from_parent(job* recent_job, job* parent_name)
{
	for (int i = 0; i < MAX_RELATIVE_NODES; i++)
		if (strcmp(parent_name->childs_of_this_job[i], recent_job->job_name) == 0)
			return parent_name->next_jobs_com_time_in_DAG[i];
}

void assign_job_id_to_cluster_history(job* job_to_assign, cluster* cluster_to_assign)
{
	int history_index = cluster_to_assign->history_index;
	history_index++;
	cluster_to_assign->job_type_history_counter[job_to_assign->job_type]++;
	cluster_to_assign->job_type_history[history_index] = job_to_assign->job_type;
	cluster_to_assign->history_index = history_index;
	if (cluster_to_assign->total_jobs_in_mem < JOBS_IN_HISTORY) {
		int total_jobs_in_mem = ++cluster_to_assign->total_jobs_in_mem;
		cluster_to_assign->total_jobs_in_mem = total_jobs_in_mem;
	}
	job_to_assign->cluster_host_id = cluster_to_assign->cluster_id;
	job_to_assign->host_cluster = cluster_to_assign;
	cluster_to_assign->cluster_util += job_to_assign->job_size;
	cluster_to_assign->job_history = job_to_assign;
	if (job_to_assign->recent_parents[0] != NULL)
		terminate_parents(job_to_assign);
	//int recent_history = ++cluster_to_assign->history_index;
	//recent_history %= JOB_TYPE_HISTORY_SIZE;
	//cluster_to_assign->history_index = recent_history;
	///*if (cluster_to_assign->total_jobs_in_mem <= JOBS_IN_HISTORY) {
	//	cluster_to_assign->job_type_history_counter[job_to_assign->job_id]++;
	//	cluster_to_assign->total_jobs_in_mem++;
	//}
	//else {
	//	if (recent_history == 0) {
	//		cluster_to_assign->job_type_history_counter[cluster_to_assign->job_type_history[job_type_history_size - 1]]--;
	//	}
	//	else {
	//		cluster_to_assign->job_type_history_counter[cluster_to_assign->job_type_history[recent_history - 1]]--;
	//	}
	//}*/
	//
	//cluster_to_assign->job_type_history[job_to_assign->job_id]++ ;
	//int total_job_in_mem = cluster_to_assign->total_jobs_in_mem;
	//if (total_job_in_mem <= JOBS_IN_HISTORY) {
	//	cluster_to_assign->job_type_history_counter[job_to_assign->job_id]++;
	//	cluster_to_assign->total_jobs_in_mem++;
	//}
	//else {
	//	int indx;
	//	if (recent_history == 0)
	//		indx = cluster_to_assign->job_type_history[JOBS_IN_HISTORY-1];
	//	else
	//		indx = cluster_to_assign->job_type_history[job_to_assign->job_id - 1];
	//	cluster_to_assign->job_type_history_counter[indx]--;
	//	cluster_to_assign->job_type_history_counter[job_to_assign->job_id]++;
	//}
	//cluster_to_assign->job_type_history[job_to_assign->job_id]++;
}

void terminate_parents(job* recent_job) {
	for (int i = 0; i < MAX_RELATIVE_NODES; i++) {
		job* recent_parent = recent_job->recent_parents[i];
		if (recent_parent == NULL)
			return;
		if (recent_parent->is_terminated)
			continue;
		recent_parent->is_terminated = true;
		cluster* parents_cluster = recent_parent->host_cluster;
		parents_cluster->cluster_util -= recent_parent->job_size;
		parents_cluster->job_type_history_counter[recent_parent->job_type]--;
		parents_cluster->total_jobs_in_mem--;
	}
}

bool is_job_on_cluster_history(job* recent_job, cluster* recent_cluster)
{
	int id = recent_job->job_type;
	if (recent_cluster->job_type_history[id] ==-1 )
		return 0;
	return 1;
}






