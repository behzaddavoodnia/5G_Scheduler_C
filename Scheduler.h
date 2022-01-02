#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>
#include <math.h>
#include "config.h"
#include "JSON.h"

#define START_TIME start_time()
#define END_TIME_FOR(x) end_time(x)
#define END_TIME end_time_without_message()


/*%%%%%%%%%%%%%%%%%%%%%%%%
* Global variables
%%%%%%%%%%%%%%%%%%%%%%%%%% */
float sum_of_all_configs[TOTAL_JOBS][TOTAL_CLUSTERS / 2]; // Stores sum of all three (four considering epsilon) DWF parameters
float alpha[TOTAL_JOBS][TOTAL_CLUSTERS / 2]; // Stores communication per each cluster
float beta[TOTAL_JOBS][TOTAL_CLUSTERS / 2]; // Stores affinity per each cluster
float gamma[TOTAL_JOBS][TOTAL_CLUSTERS / 2]; //	Stores utilization per each cluster

typedef struct branch {
	//enum { conditional, unconditional, iterational } branch_type;
	int branch_value;
	int com_size;
	//Upper and lower bound limitation for number of instances of child jobs of an iteration
	int lower_band;
	int upper_bound;
	char branch_type[MAX_KEY_LENGTH];
	char branch_name[MAX_KEY_LENGTH];
	struct job* branch_job;
}branch;

typedef struct job
{
	// Job and childs names
	char job_name[MAX_KEY_LENGTH];
	char next_job_names_in_DAG[MAX_RELATIVE_NODES][MAX_KEY_LENGTH];

	// Job section
	int job_type; // To check job instance
	int estimated_exe_time; // EET for recent (current) job in usec
	int job_id; // ID of this job
	float job_size; // Size of this job in kb
	enum { event_base, periodic } triggering; // Triggering type of job

	// Childs and parents section
	int next_jobs_com_time_in_DAG[MAX_RELATIVE_NODES]; // Communication time to next job
	struct job* recent_parents[MAX_RELATIVE_NODES]; // Parrents that directy connected to this job
	struct job* childs_of_this_job[MAX_RELATIVE_NODES]; // Next jobs (an array of pointers to the jobs) of current job in a DAG
	struct job* predecessors_of_this_job[MAX_RELATIVE_NODES]; // Previous jobs (an array of pointers to the jobs) of current job in a DAG

	// DAG section
	struct job* prev_job_pointer_in_DAG; // Link of previous job in this DAG
	struct job* next_job_pointer_in_DAG; // Link of next job in this DAG
	int hosting_DAG_id; // DAG ID which contains this job
	struct DAG* parent_DAG; // Address of DAG contains this job

	// Cell section
	int hosting_cell_id; // Cell ID which contains this job

	// DWF section
	int cluster_host_id; // ID of cluster that executed this jobs 
	bool is_terminated; // Is it terminated before
	struct cluster* host_cluster; // Address of cluster

	// Laxity section
	float laxity; // Stores calculated laxity for this job
	bool EET_is_updated; // If total EET is updated, it gets true value otherwise false
	bool CT_is_updated;  // If total CT is updated, it gets true value otherwise false
	bool Lj_is_updated;  // If total Lj is updated, it gets true value otherwise false
	int total_CT; // Stores total CT from this job to DAG root
	int total_EET; // Stores total EET from this job to DAG root
	struct job* prev_jobs_in_queue; // Link of previous job in queue
	struct job* next_jobs_in_queue; // Link of next job in queue

	// Longest path section
	char longest_path[MAX_KEY_LENGTH * 5]; // Stores the longest path from this job to Leaves
	int longest_path_time; // Total Time of longest path to leaves 
	bool longest_path_is_updated; /* If it was true, the longest path and also longest path
	// + total time (CT and EET) is calculated, otherwise its not. */
	struct branch* branch_list[MAX_RELATIVE_NODES];
}job;

typedef struct DAG
{
	char DAG_name[MAX_KEY_LENGTH]; // Name of this job
	int DAG_id; // id of this DAG
	int number_of_jobs; // Total jobs in this service
	int arrival_time; // Arrival time in ms of this job from start
	int default_cluster; // Default cluster for this DAG
	int deadline; // Deadline of DAG
	int DAG_type;
	struct DAG* next_DAG; // Link of next DAG in DAG list
	struct DAG* prev_DAG; // Link of previous DAG in DAG list
	job* head_of_jobs; // Head of job list in DAG
	job* tail_of_jobs; // Tail of job list in DAG
}DAG;

typedef struct core
{
	int L1_cache_size; // Size of L1 cache
	int L2_cache_size; // Size of L2 cache
	int core_id; // ID of this core
	struct core* next_core; // Address of next core
} core;

typedef struct cluster
{
	int cluster_id; // ID of this cluster
	int L3_cache_size; // Size of L3 cache
	struct cluster* next_cluster; // Link to next cluster
	core* list_of_cores; // Address of first core in core lists
	job* job_history; // Address of last executed job in this cluster
	int job_type_history[JOB_TYPE_HISTORY_SIZE]; // Stores history of job types
	short job_type_history_counter[JOB_TYPE_HISTORY_SIZE]; // Counter that show us how many times special +
	// + job instances executed in this cluster
	int history_index; // 
	int total_jobs_in_mem; // Total jobs available in cluster history
	float intra_com_size; // Communication size for inner cluster running
	int inter_com_size[TOTAL_CLUSTERS]; // Communication size for running on another cluster
	float cluster_util;
} cluster;

typedef struct die
{
	long long int die_memory_size; // Size of die memory in kb
	int number_of_clusters; // Total clusters in this die
	cluster* head_of_clusters; // Link of head of cluster lists
	struct die* next_die; // Next die in dice list
} die;

typedef struct board
{
	int total_dice; // Total dice in this board
	die* head_of_dice; // Link of head of dice list
} board;


/* * Constructor-like function */
void sched_constructor();


/*%%%%%%%%%%%%%%%%%%%%%%%%
/* * Calculating elapse time
%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* * Starting timer for calculating a section execution time  */
void start_time();

/* * End timer for a section and print message */
void end_time(STRING event_name);

/* * End timer for a section */
LONGLONG end_time_without_message();

/*%%%%%%%%%%%%%%%%%%%%%%%%
* Other relative functions
%%%%%%%%%%%%%%%%%%%%%%%%%% */
char recent_path[MAX_KEY_LENGTH * 5];

/* * Calculating minimum number in an array */
float min_number(float inp[], int size);

/* * Creating bidirectional DAGs list */
void reverse_order_DAG();

/* * Calculate maximum length of path from each job to final job in a DAG (B_level) */
void B_level(); //~~~

/* * Calculate longest path for an input job */
STRING paths(job* head_job); //~~~

/* * Update longest path */
void update_longest_path(STRING recent_path, job* head_of_jobs); //~~~

/* * Creating bidirectional list of jobs in each DAG */
void reverse_order_job();

/* * Creating bidirectional list of jobs */
void create_bidirectional_job_queue();

/* * Like C++ setw function */
STRING _set_w(STRING input, int length);

/* * Return job name from path */
STRING job_substr(STRING str); //~~~

/* * Update childs pointer for all jobs */
void child_pointer_update();

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
* Job functions and global variables
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
job* tail_of_job_list; // A pointer to the tail previous created job
job* head_of_job_list; // A pointer to the head of previous created job

job* laxity_array[TOTAL_JOBS]; //

/* * Return a pointer to one job with input name */
job* find_job_in_DAG(STRING job_name, job* head_of_jobs_in_DAG);

/* * Return id of job name */
int get_id_from_job_name(const STRING job_name);

/* * Update next jobs data in input_job */
void assign_next_jobs(const STRING input, job* input_job);//~~~

/* * Create random next job for conditional and iterational */
void create_random_next_jobs();//~~~

//Assign branch to job 
void assign_branch_to_job(branch* next_branch, job* input_job, int total_substr);

/* * Create job from job string */
job* create_job(const STRING input_job, STRING job_name, int hosting_DAG_id, job* prev, DAG* parent_DAG);

/* * Return true if a pointer to input_job is available */
bool has_job_pointer(job* input_job_pointer, job* job_pointer_array[MAX_RELATIVE_NODES]);//~~~

/* * Add parents to parent list */
void add_parent(job* src, job* dst, bool is_recent_parent);//~~~

/* * Add predecessor of source to destination if destination doesn't have predecessors */
void copy_predecessors(job* dst, job* src);//~~~

/* * Get communication time between parent and child */
int get_ct(job* parent, job* child);//~~~

/* * Calculate total EET for using in laxity calculation */
int calculate_total_EET(job* recent_job);//~~~

/* * Return address of a job with job_name */
job* child_address(STRING child_name, job* recent_job);//~~~

/* * Updating predecessors list of all jobs */
void predecessors_pointer_update();//~~~

/* * Calculate laxity for input job */
float laxity_calculation(job* recent_job);//~~~

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
* DAG functions and global variables
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
DAG* head_of_DAG_list; // A pointer to the previously created DAG

/* * Get DAGs from application JSON string */
STRING get_DAGs(const STRING txt);

/* * Create a DAG using DAG string */
DAG* create_DAG(const STRING input_DAG, STRING DAG_name);

/* * Function to swap the the position of two elements */
void swap(job* arr[], int a, int b);

/* * Converting binary tree to heap data structure */
void heapify(job* arr[], int n, int i);

/* * Sort job array according to job laxities using heap sort */
void heap_sort(job* arr[], int n);



/*%%%%%%%%%%%%%%%%%%%%%%%%
* Hardware functions and global variables
%%%%%%%%%%%%%%%%%%%%%%%%%% */
board* prev_created_board; // A pointer to the previously created die

/* * Create board from json input text */
board* create_board(const STRING input_board);

die* prev_created_die; // A pointer to the previously created die

/* * Create a die from board dice */
die* create_die(const STRING input_die, int die_index);

cluster* prev_created_cluster; // A pointer to the previous created cluster
cluster* first_cluster;
cluster* cluster_array[TOTAL_CLUSTERS];

/* * Create a cluster from cluster string and return its value */
cluster* create_cluster(const STRING input_cluster);

/* *  */
//void assign_next_clusters(const STRING input);

core* prev_created_core; // A pointer to the previous created core

/* * Create a core from core string and return its address*/
core* create_core(const STRING input_core);

/*Calc total CT*/
int calculate_total_CT(job* recent_job);

void sched_constructor();



