#include "Scheduler.h"

int DWF_parameters_counter = 0;

float calculate_alpha(job* recent_job, cluster *recent_cluster);

float calculate_beta(job* recent_job, const cluster *recent_cluster);

float calculate_gamma(int util_index);

int get_CT_from_parent_name(job *recent_job, char *parent_name);

int get_CT_from_parent(job *recent_job, job *parent_name);

void assign_job_id_to_cluster_history(job *job_to_assign, cluster *cluster_to_assign);

void terminate_parents(job* recent_job);

bool is_job_on_cluster_history(job *recent_job, cluster *recent_cluster);

void DWF_constructor();

void DWF_calculation(job* input_job);

void end_DWF(job* recent_job, cluster* recent_cluster);

float util_avg();
