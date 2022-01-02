#include "Scheduler.h"
#include <Windows.h>


void sched_constructor() {
	// Assign NULL to head of linked lists
	head_of_DAG_list = NULL;
	tail_of_job_list = NULL;
	prev_created_cluster = NULL;
	srand(time(NULL));
}


//********************** timer section **********************
//====================== start timer ======================
/* * For calculating time */
//static clock_t program_time;
LARGE_INTEGER start, end, freq;
bool freq_calculated = false;

void start_time()
{
	if (freq_calculated == false)
	{
		QueryPerformanceFrequency(&freq);
		freq_calculated = true;
	}
	QueryPerformanceCounter(&start);
}

//====================== end timer ======================
void end_time(STRING event_name)
{
	LONGLONG t = END_TIME;
	printf("time calculate for -> %s <- is %llu microsecond\n", event_name, t * 1000000 / freq.QuadPart);
}

LONGLONG end_time_without_message() {
	QueryPerformanceCounter(&end);
	return (end.QuadPart - start.QuadPart);
}
//********************** timer section **********************

//====================== DALLI_scheduler ======================

/*%%%%%%%%%%%%%%%%%%%%%%%%
* Other relative functions
%%%%%%%%%%%%%%%%%%%%%%%%%% */

//====================== min_number ======================
float min_number(float inp[], int size) {
	float minimum_number = FLT_MAX;
	for (int i = 0; i < size; i++) {
		if (*(inp + i) < minimum_number)
			minimum_number = *(inp + i);
	}
	return minimum_number;
}

//====================== reverse_order_DAG ======================
void reverse_order_DAG() {
	DAG* current = head_of_DAG_list;
	DAG* prev = NULL, * next = NULL;

	while (current != NULL) {
		// Keep previous pointer
		prev = current->prev_DAG;

		// Reverse current node's pointer
		current->prev_DAG = prev;
		current->next_DAG = next;

		// Move pointers one position ahead.
		next = current;
		current = current->prev_DAG;
	}
	head_of_DAG_list = next;
}

void B_level() {
	DAG* DAGptr = head_of_DAG_list;
	job* jobptr;
	// For traversing all DAGs
	while (DAGptr) {
		//printf("\n\n********* %s *********\n\n", DAGptr->DAG_name);
		jobptr = DAGptr->head_of_jobs;
		// For traversing all jobs in this DAG
		while (jobptr) {
			strcpy(recent_path, jobptr->job_name);
			// Update longest path
			paths(jobptr);
			//printf(" Longest path (B_level) for ****%s**** is ==> %s\n and total time(sum of CTs and EETs) is % d\n\n", jobptr->job_name, jobptr->longest_path, jobptr->longest_path_time);
			jobptr = jobptr->next_job_pointer_in_DAG;
		}
		// Print at the end of all jobs in a DAG
		//printf("###### End %s ######\n\n", DAGptr->DAG_name);
		DAGptr = DAGptr->next_DAG;
	}
};

//====================== paths ======================
STRING paths(job* recent_job) {
	char tmpstr[MAX_KEY_LENGTH * 5];
	// If path previously calculate return its value
	if (recent_job->longest_path_is_updated == true)
		return recent_job->longest_path;
	// This section is for leaves
	if (recent_job->childs_of_this_job[0] == NULL) {
		strcpy(recent_job->longest_path, recent_job->job_name);
		recent_job->longest_path_time = recent_job->estimated_exe_time;
		recent_job->longest_path_is_updated = true;
		update_longest_path(recent_job->job_name, recent_job);
		return recent_job->job_name;
	}
	// Recursively calculate longest path for childs
	else {
		for (int i = 0; i < MAX_RELATIVE_NODES; i++) {
			if (recent_job->childs_of_this_job[i] == NULL)
				break;
			strcpy(tmpstr, "");
			strcat(tmpstr, recent_job->job_name);
			strcat(tmpstr, " -> ");
			strcat(tmpstr, paths(recent_job->childs_of_this_job[i]));
			if (strcmp(job_substr(tmpstr), recent_path) == 0) {
				update_longest_path(tmpstr, recent_job);
			}
		}
	}
	return tmpstr;
}

//======================  ======================
void update_longest_path(STRING recent_path, job* head_of_jobs) {
	char* job_ptr = recent_path;
	job* parent = head_of_jobs, * child = NULL;
	int job_ptr_counter = 0;
	int CT = 0;
	char job_name[MAX_KEY_LENGTH];
	int path_weight = 0;
	int job_key_counter;
	// Get childs address from longest path
	while (job_ptr[job_ptr_counter] != '\0') {
		if (job_ptr[job_ptr_counter] == 'J') {
			job_key_counter = 0;
			while (job_ptr[job_ptr_counter] != ' ') {
				job_name[job_key_counter++] = job_ptr[job_ptr_counter++];
				if (job_ptr[job_ptr_counter] == '\0')
					break;
			}
			job_name[job_key_counter] = '\0';
			// Get address of each job in longest path
			child = child_address(job_name, parent);
			if (parent == child)
				CT = 0;
			else
				CT = get_ct(parent, child);
			// Update longest path for each job
			path_weight += child->estimated_exe_time;
			path_weight += CT;
			job_ptr_counter--;
		}
		parent = child;
		job_ptr_counter++;
	}
	// Update longest path and its value in job struct
	if (path_weight > head_of_jobs->longest_path_time) {
		head_of_jobs->longest_path_time = path_weight;
		strcpy(head_of_jobs->longest_path, recent_path);
		head_of_jobs->longest_path_is_updated = true;
	}
}

//====================== reverse_order_job ======================
void reverse_order_job() {
	DAG* current_DAG = head_of_DAG_list;
	job* prev = NULL;
	// Traversing all DAGs
	while (current_DAG != NULL) {
		job* current_job = current_DAG->tail_of_jobs;
		current_job->next_job_pointer_in_DAG;
		// Traversing all jobs
		while (current_job->prev_job_pointer_in_DAG != NULL) {
			// Assign previous job address to prev_job_pointer_in_dag
			prev = current_job->prev_job_pointer_in_DAG;
			prev->next_job_pointer_in_DAG = current_job;
			current_job = current_job->prev_jobs_in_queue;
		}
		// Assign NULL for previous link in last job in service
		current_job->prev_job_pointer_in_DAG = NULL;
		current_DAG->head_of_jobs = current_job;
		current_DAG = current_DAG->next_DAG;
	}
}

//====================== create_bidirectional_job_queue ======================
void create_bidirectional_job_queue() {
	job* jobptr = tail_of_job_list;
	jobptr->next_jobs_in_queue = NULL;
	// Traversing all jobs in queue
	while (jobptr != NULL)
	{
		if (!(jobptr->prev_jobs_in_queue)) {
			// Updating head of job queue
			head_of_job_list = jobptr;
			break;
		}
		// Next job of previous job of this job, is this job! :)
		jobptr->prev_jobs_in_queue->next_jobs_in_queue = jobptr;
		jobptr = jobptr->prev_jobs_in_queue;
	}
}

//====================== _set_w ======================
STRING _set_w(STRING input, int length) {
	STRING str;
	str = NEW_AND_INITIALIZE(STRING, length);
	int i = 0;
	// For printing input characters
	for (i; i < length; i++) {
		if (input[i] == '\0')
			break;
		str[i] = input[i];
	}
	// Filling to complete length with space
	for (int j = i; j < length; j++) {
		str[j] = ' ';
	}
	str[length] = '\0';
	return str;
}

//====================== job_substr ======================
STRING job_substr(STRING job_names) {
	STRING strptr = job_names;
	// Reading jobs from path
	for (int i = 0; i < MAX_KEY_LENGTH; i++) {
		if (*strptr == ' ') {
			GLOBAL_KEY_STRING[i] = '\0';
			break;
		}
		GLOBAL_KEY_STRING[i] = *strptr;
		strptr++;
	}
	return GLOBAL_KEY_STRING;
}

//====================== child_pointer_update ======================
void child_pointer_update() {
	DAG* current_DAG = head_of_DAG_list;
	// Traversing DAGs
	while (current_DAG != NULL) {
		job* current_job = current_DAG->head_of_jobs;
		// Traversing all jobs in DAG
		while (current_job->next_job_pointer_in_DAG != NULL) {
			for (int i = 0; i < MAX_RELATIVE_NODES; i++)
			{
				// Assign childs address to childs array
				if (current_job->next_jobs_com_time_in_DAG[i] == -1)
					break;
				current_job->childs_of_this_job[i] = find_job_in_DAG(current_job->next_job_names_in_DAG[i],
					current_job->parent_DAG->head_of_jobs);
			}
			if (!current_job->next_job_pointer_in_DAG)
				break;
			current_job = current_job->next_job_pointer_in_DAG;
		}
		current_DAG = current_DAG->next_DAG;
	}
}

//====================== rand_gen ======================
double rand_gen() {
	// return a uniformly distributed random value
	return ((double)(rand()) + 1.) / ((double)(RAND_MAX)+1.);
}

//====================== semi_normal_dist_rand ======================
double semi_normal_dist_rand() {
	double x = rand_gen();
	if (rand_gen() > 0.5)
		x = 0.5 * (x + 0.5);
	if (rand_gen() > 0.6)
		x = 0.5 * (x + 0.5);
	if (rand_gen() > 0.7)
		x = 0.5 * (x + 0.5);
	if (rand_gen() > 0.8)
		x = 0.5 * (x + 0.5);
	if (rand_gen() > 0.9)
		x = 0.5 * (x + 0.5);
	return x;
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
* Job functions and global variables
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
//====================== find_job_in_DAG ======================
job* find_job_in_DAG(STRING job_name, job* head_of_jobs_in_DAG) {
	job* current_job = head_of_jobs_in_DAG;
	// Traversing from head of job in DAG to return address with input job name
	while (current_job != NULL) {
		if (strcmp(job_name, current_job->job_name) == 0)
			return current_job;
		current_job = current_job->next_job_pointer_in_DAG;
	}
	return NULL;
}

//====================== get_id_from_job_name ======================
int get_id_from_job_name(const STRING job_name) {
	char job_id[10];
	// Extract id from job name
	for (int i = 3; i < 10; i++) {
		job_id[i - 3] = *(job_name + i);
		if (*(job_name + i) == '\0')
			break;
	}
	return atoi(job_id);
}

//====================== assign_next_jobs ======================
void assign_next_jobs(const STRING input, job* input_job) {
	int next_job_counter = 0;
	const int total_substr = pattern_count("Branch", input);
	char branch_pattern[MAX_KEY_LENGTH];
	strcpy(branch_pattern, "");
	const STRING tmp_ptr;
	const STRING next_job_string = NEW_AND_INITIALIZE(STRING, (obj_char_count(input)));
	strcpy(next_job_string, input);
	branch* recent_branch;
	STRING tmp_branch;
	for (int i = 1; i <= total_substr; i++) {
		recent_branch = NEW_AND_INITIALIZE(branch*, 16);
		tmp_ptr = input;
		snprintf(branch_pattern, MAX_KEY_LENGTH, "Branch%d", i);
		tmp_ptr = get_obj(branch_pattern, next_job_string);
		tmp_branch = get_obj("job_name", get_val(tmp_ptr));
		strcpy(recent_branch->branch_name, _remove_quotation(get_val(tmp_branch)));
		tmp_branch = get_obj("Branch_type", tmp_ptr);
		strcpy(recent_branch->branch_type, _remove_quotation(get_val(tmp_branch)));
		tmp_branch = get_obj("Branch_value", tmp_ptr);
		recent_branch->branch_value = atoi(get_val(tmp_branch));
		tmp_branch = get_obj("comm_size", tmp_ptr);
		recent_branch->com_size = atoi(get_val(tmp_branch));
		tmp_branch = get_obj("lower_bound", tmp_ptr);
		recent_branch->lower_band = atoi(get_val(tmp_branch));
		tmp_branch = get_obj("upper_bound", tmp_ptr);
		recent_branch->upper_bound = atoi(get_val(tmp_branch));
		recent_branch->branch_job = tail_of_job_list;
		assign_branch_to_job(recent_branch, input_job, next_job_counter);
		next_job_counter++;
	}
	for (int i = next_job_counter; i < MAX_RELATIVE_NODES; i++) {
		strcpy(input_job->next_job_names_in_DAG[i], "");
		input_job->next_jobs_com_time_in_DAG[i] = -1;
	}
}

//====================== create_random_next_jobs ======================
void create_random_next_jobs() {

}

//====================== assign_branch_to_job ======================
void assign_branch_to_job(branch* next_branch, job* input_job, int next_job_counter) {

	next_branch->branch_job = input_job;
	input_job->branch_list[next_job_counter] = next_branch;
	input_job->next_jobs_com_time_in_DAG[next_job_counter] = next_branch->com_size;
	next_branch->branch_name[0] = 'J';
	strcpy(input_job->next_job_names_in_DAG[next_job_counter], next_branch->branch_name);

}

//====================== create_job ======================
job* create_job(const STRING input_job, STRING job_name, int hosting_DAG_id,
	job* prev_in_DAG, DAG* parent_DAG) {
	// Keep previous job address
	job* prev_job = tail_of_job_list;
	tail_of_job_list = NEW_AND_INITIALIZE(job*, 400);
	tail_of_job_list->parent_DAG = parent_DAG;
	tail_of_job_list->EET_is_updated = false;
	tail_of_job_list->CT_is_updated = false;
	tail_of_job_list->Lj_is_updated = false;
	tail_of_job_list->job_id = get_id_from_job_name(job_name);
	const STRING tmp_ptr = NEW_AND_INITIALIZE(STRING, (obj_char_count(input_job)));
	strcpy(tmp_ptr, input_job);
	strcpy(tail_of_job_list->job_name, job_name);
	// Reading keys and related value from JSON string of a job
	tail_of_job_list->prev_jobs_in_queue = prev_job;
	STRING tmp_obj = get_obj("Type", tmp_ptr);
	tail_of_job_list->job_type = atoi(get_val(tmp_obj));
	tmp_obj = get_obj("Jsize", tmp_ptr);
	tail_of_job_list->job_size = (float)atof(get_val(tmp_obj));
	tmp_obj = get_obj("Cell_id", tmp_ptr);
	tail_of_job_list->hosting_cell_id = atoi(get_val(tmp_obj));
	tmp_obj = get_obj("Estimated_execution_time", tmp_ptr);
	tail_of_job_list->estimated_exe_time = atoi(get_val(tmp_obj));
	tmp_obj = get_obj("Triggering", tmp_ptr);
	if (strcmp(get_val(tmp_obj), "\"periodic\"") == 0)
		tail_of_job_list->triggering = periodic;
	else
		tail_of_job_list->triggering = event_base;
	tmp_obj = tmp_ptr;
	if (pattern_count("nextJ", tmp_obj) != 0) {
		tmp_obj = get_obj("nextJ", tmp_ptr);
		tmp_obj = get_val(tmp_obj);
		assign_next_jobs(tmp_obj, tail_of_job_list);
	}
	else {
		for (int i = 0; i < MAX_RELATIVE_NODES; i++)
		{
			strcpy(tail_of_job_list->next_job_names_in_DAG[i], "");
			tail_of_job_list->next_jobs_com_time_in_DAG[i] = -1;
		}
	}
	 //Initializing parents and childs array
	for (int i = 0; i < MAX_RELATIVE_NODES; i++) {
		tail_of_job_list->predecessors_of_this_job[i] = NULL;
		tail_of_job_list->childs_of_this_job[i] = NULL;
		tail_of_job_list->predecessors_of_this_job[i] = NULL;
		tail_of_job_list->next_job_pointer_in_DAG = NULL;
		tail_of_job_list->recent_parents[i] = NULL;
	}

	tail_of_job_list->hosting_DAG_id = hosting_DAG_id;
	tail_of_job_list->prev_job_pointer_in_DAG = prev_in_DAG;
	tail_of_job_list->cluster_host_id = -1;// rand() % TOTAL_CLUSTERS;
	tail_of_job_list->host_cluster = NULL;
	tail_of_job_list->longest_path_time = 0;
	tail_of_job_list->longest_path_is_updated = false;
	tail_of_job_list->is_terminated = false;
	return tail_of_job_list;
}

//====================== has_job_pointer ======================
bool has_job_pointer(job* input_job_pointer, job* job_pointer_array[MAX_RELATIVE_NODES]) {
	// Return true if job available in jobs array
	for (int i = 0; i < MAX_RELATIVE_NODES; i++)
		if (job_pointer_array[i] == input_job_pointer)
			return true;
	return false;
}

//====================== add_parent ======================
void add_parent(job* src, job* dst, bool is_recent_parent) {
	// Traversing to reach first empty node is_recent_parent specifies it is parent or predecessor
	for (int i = 0; i < MAX_RELATIVE_NODES; i++) {
		// Update recent parents
		if (is_recent_parent) {
			if (dst->recent_parents[i] == NULL)
			{
				dst->recent_parents[i] = src;
				break;
			}
		}
		// Update predecessors
		else {
			if (dst->predecessors_of_this_job[i] == NULL)
			{
				dst->predecessors_of_this_job[i] = src;
				break;
			}
		}
	}
}

//====================== copy_predecessors ======================
void copy_predecessors(job* dst, job* src) {
	// Copy predecessors of source to destination
	for (int i = 0; i < MAX_RELATIVE_NODES; i++) {
		if (src->predecessors_of_this_job[i] == NULL)
			break;
		else {
			if (!has_job_pointer(src->predecessors_of_this_job[i], dst->predecessors_of_this_job))
				add_parent(src->predecessors_of_this_job[i], dst, false);

		}
	}
}

//====================== get_ct ======================
int get_ct(job* parent, job* child) {
	// Get communication time between a parent and one of its childs
	for (int i = 0; i < MAX_RELATIVE_NODES; i++) {
		if (strcmp(parent->next_job_names_in_DAG[i], child->job_name) == 0)
			return parent->next_jobs_com_time_in_DAG[i];
	}
}

//====================== calculate_total_EET ======================
int calculate_total_EET(job* recent_job) {
	// Assign EET of recent job to total_EET
	int total_EET = recent_job->estimated_exe_time;
	// Return EET if EET is updated
	if (recent_job->EET_is_updated)
		return recent_job->total_EET;
	// Add all predecessors EET value to total_EET
	if (recent_job->recent_parents[0] != NULL) {
		for (int i = 0; i < MAX_RELATIVE_NODES; i++)
		{
			if (recent_job->predecessors_of_this_job[i] == NULL)
				break;
			total_EET += recent_job->predecessors_of_this_job[i]->estimated_exe_time;
		}
	}
	// Update recent_job EET and related flag
	recent_job->EET_is_updated = true;
	recent_job->total_EET = total_EET;
	return total_EET;
}

//====================== child_address ======================
job* child_address(STRING child_name, job* recent_job) {
	// Return job with specific job name
	job* head_of_jobs = recent_job->parent_DAG->head_of_jobs;
	while (head_of_jobs != NULL) {
		if (strcmp(child_name, head_of_jobs->job_name) == 0)
			return head_of_jobs;
		head_of_jobs = head_of_jobs->next_job_pointer_in_DAG;
	}
	return NULL;
}

//====================== predecessors_pointer_update ======================
void predecessors_pointer_update() {
	DAG* current_DAG = head_of_DAG_list;
	// Traversing all DAGs
	while (current_DAG != NULL) {
		job* current_job = current_DAG->head_of_jobs;
		// Traversing all jobs in a DAG
		while (current_job != NULL) {
			// Update predecessors
			for (int i = 0; i < MAX_RELATIVE_NODES; i++)
			{
				if (current_job->childs_of_this_job[i] == NULL)
					break;
				job* child = current_job->childs_of_this_job[i];
				copy_predecessors(child, current_job);
				add_parent(current_job, child, false);
				add_parent(current_job, child, true);
			}
			if (!current_job->next_job_pointer_in_DAG)
				break;
			current_job = current_job->next_job_pointer_in_DAG;
		}
		current_DAG = current_DAG->next_DAG;
	}
}

//====================== laxity_calculation ======================
float  laxity_calculation(job* recent_job) {
	float Lj = 0.0;
	// If Lj is updated return its value
	if (recent_job->Lj_is_updated)
		return recent_job->laxity;
	//Deadlines are absolute so we dont need to add arrival time
	//Lj = recent_job->parent_DAG->deadline - (calculate_total_EET(recent_job) + calculate_total_CT(recent_job));
	Lj = recent_job->parent_DAG->deadline - recent_job->longest_path_time;
	// Updating Lj and its relating flag
	recent_job->Lj_is_updated = true;
	recent_job->laxity = Lj;
	return Lj;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%
* DAG functions and global variables
%%%%%%%%%%%%%%%%%%%%%%%%%% */
//====================== get_DAGs ======================
STRING get_DAGs(const STRING txt) {
	const int total_DAG = pattern_count("DAG", txt);
	STRING value = NEW_AND_INITIALIZE(STRING, MAX_OBJECT_LENGTH);
	// Traversing to JSON string to find all DAGs
	for (int i = 0; i < total_DAG; i++) {
		char DAG_pattern[MAX_KEY_LENGTH];
		snprintf(DAG_pattern, MAX_KEY_LENGTH, "DAG%d", i);
		value = get_val(get_obj(DAG_pattern, txt));
		// Create finding DAG in JSON string
		create_DAG(value, DAG_pattern);
	}
	return value;
}

//====================== create_DAG ======================
DAG* create_DAG(const STRING input_DAG, STRING DAG_name) {
	job* prev_in_DAG = NULL;
	static int job_counter = 0;
	bool first_job = true;
	const STRING tmp_ptr = NEW_AND_INITIALIZE(STRING, (obj_char_count(input_DAG)));
	strcpy(tmp_ptr, input_DAG);
	DAG* prev_DAG = head_of_DAG_list;
	head_of_DAG_list = NEW_AND_INITIALIZE(DAG*, 16);
	head_of_DAG_list->prev_DAG = prev_DAG;
	head_of_DAG_list->number_of_jobs = pattern_count("Job", tmp_ptr);
	STRING tmp_obj = get_obj("ArrivalTime", tmp_ptr);
	head_of_DAG_list->arrival_time = atoi(get_val(tmp_obj));
	tmp_obj = get_obj("Default_cluster", tmp_ptr);
	head_of_DAG_list->default_cluster = atoi(get_val(tmp_obj));
	tmp_obj = get_obj("ID", tmp_ptr);
	head_of_DAG_list->DAG_id = atoi(get_val(tmp_obj));
	tmp_obj = get_obj("Deadline", tmp_ptr);
	head_of_DAG_list->deadline = atoi(get_val(tmp_obj));
	tmp_obj = get_obj("DType", tmp_ptr);
	head_of_DAG_list->DAG_type = atoi(get_val(tmp_obj));
	char job_pattern[MAX_KEY_LENGTH];
	// Extract all jobs from DAG
	for (int i = 0; i < head_of_DAG_list->number_of_jobs; i++)
	{
		snprintf(job_pattern, MAX_KEY_LENGTH, "Job%d", job_counter);
		tmp_obj = get_obj(job_pattern, tmp_ptr);
		if (first_job) {
			prev_in_DAG = create_job(get_val(tmp_obj), job_pattern, head_of_DAG_list->DAG_id, prev_in_DAG, head_of_DAG_list);
			first_job = false;
		}
		else
			prev_in_DAG = create_job(get_val(tmp_obj), job_pattern, head_of_DAG_list->DAG_id, prev_in_DAG, head_of_DAG_list);
		job_counter++;
	}
	head_of_DAG_list->tail_of_jobs = prev_in_DAG;
	strcpy(head_of_DAG_list->DAG_name, DAG_name);
	return head_of_DAG_list;
}

/*==================================================*/
int ytt = 0;

//====================== swap ======================
void swap(job* arr[], int a, int b){
	job* temp = arr[a];
	arr[a] = arr[b];
	arr[b] = temp;
}

//====================== heap_sort ======================
void heapify(job* arr[], int n, int i) {
	// Find largest among root, left child and right child
	int largest = i;
	int left = 2 * i + 1;
	int right = 2 * i + 2;
	if (left < n && arr[left]->laxity > arr[largest]->laxity)
		largest = left;

	if (right < n && arr[right]->laxity > arr[largest]->laxity)
		largest = right;

	// Swap and continue heapifying if root is not largest
	if (largest != i) {
		swap(arr, i, largest);
		heapify(arr, n, largest);
	}
}

//====================== heap_sort ======================
void heap_sort(job *arr[], int n) {
	ytt++;
	// Build max heap
	for (int i = n / 2 - 1; i >= 0; i--)
		heapify(arr, n, i);

	// Heap sort
	for (int i = n - 1; i >= 0; i--) {
		swap(arr, 0, i);

		// Heapify root element to get highest element at root again
		heapify(arr, i, 0);
	}
	//printf("%d\n", ytt);
}



/* %%%%%%%%%%%%%%%%%%%%%%%%
* Create Architecture functions and global variables
%%%%%%%%%%%%%%%%%%%%%%%%%% */
//====================== create_board ======================
board* create_board(const STRING input_board) {
	prev_created_board = NEW_AND_INITIALIZE(board*, 1);
	prev_created_board->total_dice = pattern_count("Die", input_board);
	const STRING tmp_ptr = NEW_AND_INITIALIZE(STRING, (obj_char_count(input_board)));
	strcpy(tmp_ptr, input_board);
	// Create dice from this board and create dice list
	for (int i = 0; i < prev_created_board->total_dice; i++) {
		char cluster_pattern[MAX_KEY_LENGTH];
		snprintf(cluster_pattern, MAX_KEY_LENGTH, "Die%d", i);
		STRING tmp_obj = get_obj(cluster_pattern, tmp_ptr);
		tmp_obj = get_val(tmp_obj);
		prev_created_board->head_of_dice = create_die(tmp_obj, i);
	}
	die* die_head = prev_created_board->head_of_dice;
	first_cluster = die_head->head_of_clusters;
	int cluster_index = 15;
	while (die_head != NULL) {
		cluster* cluster_head = die_head->head_of_clusters;
		while (cluster_head->next_cluster != NULL) {
			cluster_array[cluster_index--] = cluster_head;
			cluster_head = cluster_head->next_cluster;
		}
		cluster_array[cluster_index--] = cluster_head;
		die_head = die_head->next_die;
		if (die_head == NULL)
			break;
		cluster_head->next_cluster = die_head->head_of_clusters;
	}
	return prev_created_board;
}

//====================== create_die ======================
die* create_die(const STRING input_die, int die_index) {
	const STRING tmp_ptr = NEW_AND_INITIALIZE(STRING, (obj_char_count(input_die)));
	strcpy(tmp_ptr, input_die);
	die* prev_die = prev_created_die;
	prev_created_die = NEW_AND_INITIALIZE(die*, 4);
	prev_created_die->next_die = prev_die;
	STRING tmp_obj = get_obj("die_memory_size", tmp_ptr);
	prev_created_die->die_memory_size = atoll (get_val(tmp_obj));
	prev_created_die->number_of_clusters = pattern_count("Cluster", tmp_obj);
	const int total_cluster = pattern_count("Cluster", input_die);
	STRING value = NEW_AND_INITIALIZE(STRING, total_cluster);
	for (int i = 0; i < TOTAL_CLUSTERS_PER_DIE; i++) {
		char cluster_pattern[MAX_KEY_LENGTH];
		snprintf(cluster_pattern, MAX_KEY_LENGTH, "Cluster%d", i + TOTAL_CLUSTERS_PER_DIE * die_index);
		tmp_obj = get_obj(cluster_pattern, tmp_ptr);
		if (i == 0)
		{
			prev_created_cluster = NULL;
			create_cluster(get_val(tmp_obj));
		}
		else if (i == 7) {
			prev_created_die->head_of_clusters = create_cluster(get_val(tmp_obj));
		}

		else {
			create_cluster(get_val(tmp_obj));
		}
	}
	return prev_created_die;
}


//====================== create_cluster ======================
cluster* create_cluster(const STRING input_cluster) {
	const STRING tmp_ptr = NEW_AND_INITIALIZE(STRING, obj_char_count(input_cluster));
	strcpy(tmp_ptr, input_cluster);
	cluster* prev_cluster = prev_created_cluster;
	prev_created_cluster = NEW_AND_INITIALIZE(cluster*, 16);
	STRING tmp_obj = get_obj("cluster_ID", tmp_ptr);
	prev_created_cluster->cluster_id = atoi(get_val(tmp_obj));
	tmp_obj = get_obj("L3_cache_size", tmp_ptr);
	prev_created_cluster->L3_cache_size = atoi(get_val(tmp_obj));
	prev_created_cluster->next_cluster = prev_cluster;
	int total_cores = pattern_count("Core", tmp_ptr);
	STRING value = NEW_AND_INITIALIZE(STRING, total_cores);
	// Create all cores available in cluster JSON string and creating core list
	for (int i = 0; i < total_cores; i++) {
		char core_pattern[MAX_KEY_LENGTH];
		snprintf(core_pattern, MAX_KEY_LENGTH, "Core%d", i);
		tmp_obj = get_obj(core_pattern, tmp_ptr);
		value = get_val(get_obj(core_pattern, tmp_ptr));
		if (i == 0)
		{
			prev_created_core = NULL;
			create_core(get_val(tmp_obj));
		}
		else if (i == 3) {
			prev_created_cluster->list_of_cores = create_core(get_val(tmp_obj));
		}
		else {
			create_core(get_val(tmp_obj));
		}
	}
	// Initilizing cluster
	prev_created_cluster->history_index = 0;
	prev_created_cluster->intra_com_size = 2;
	prev_created_cluster->total_jobs_in_mem = 0;
	// Initilizing inter communication size
	for (int i = 0; i < TOTAL_CLUSTERS; i++)
		prev_created_cluster->inter_com_size[i] = 3;
	// Initilizing job history
	for (int i = 0; i < JOB_TYPE_HISTORY_SIZE; i++)
	{
		prev_created_cluster->job_type_history_counter[i] = 0;
		prev_created_cluster->job_type_history[i] = -1;
	}
	prev_created_cluster->cluster_util = 0;
	return prev_created_cluster;
}

//====================== create_core ======================
core* create_core(const STRING input_core) {
	core* prev_core = prev_created_core;
	prev_created_core = NEW_AND_INITIALIZE(core*, 16);
	const STRING tmp_ptr = NEW_AND_INITIALIZE(STRING, (obj_char_count(input_core)));
	strcpy(tmp_ptr, input_core);
	// Assign core specs
	STRING tmp_obj = get_obj("L1_cache_size", tmp_ptr);
	prev_created_core->L1_cache_size = atoi(get_val(tmp_obj));
	tmp_obj = get_obj("L2_cache_size", tmp_ptr);
	prev_created_core->L2_cache_size = atoi(get_val(tmp_obj));
	tmp_obj = get_obj("core_ID", tmp_ptr);
	prev_created_core->core_id = atoi(get_val(tmp_obj));
	prev_created_core->next_core = prev_core;
	return prev_created_core;
}

//====================== calculate_total_CT ======================
int calculate_total_CT(job* recent_job) {
	int total_CT = 0;
	// Return CT if it is now updated
	if (recent_job->CT_is_updated)
		return recent_job->total_CT;
	// If recent job has no parents
	if (recent_job->recent_parents[0] == NULL)
	{
		recent_job->total_CT = 0;
		recent_job->CT_is_updated = true;
		return 0;
	}
	// If total CT not updated and has parent
	else {
		for (int i = 0; i < MAX_RELATIVE_NODES; i++)
		{
			if (recent_job->recent_parents[i] == NULL)
				break;
			total_CT += calculate_total_CT(recent_job->recent_parents[i]);
			total_CT += get_ct(recent_job->recent_parents[i], recent_job);
		}
	}
	recent_job->CT_is_updated = true;
	recent_job->total_CT = total_CT;
	return total_CT;
}

//====================== print_DAG ======================
void print_DAG(DAG* head) {
	// 
	DAG* tmp = head;
	while (tmp != NULL) {
		printf("Default_Cluster is: %d\n", tmp->arrival_time);
		tmp = tmp->next_DAG;
	}
}



