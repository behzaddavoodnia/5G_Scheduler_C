#define C1 0.1 // alpha = parent or communication
#define C2 0.1 // beta = affinity
#define C3 0.8 // gamma = utilization
#define Epsilon 0.0001 // For choosing default cluster
#define JOB_TYPE_HISTORY_SIZE 50 // Total jobs in cluster
#define JOBS_IN_HISTORY 40
#define INTER_CLUSTER_BW 3 // Inter bandwidth between two clusters
#define	INTRA_CLUSTER_BW 2 // Inner bandwidth of a cluster (between cores)
#define MAX_CLUSTER_UTIL 0.8 // Maximum possible utilization of a cluster
#define AVAILABLE_CLUSTER_FOR_ENERGY_SAVING 8 // Total available clusters for energy saving
#define TTI 500 // Time transmission interval in usec
#define SCHEDULING_PERIOD 250 // Period of scheduler
#define PRINT_BUFF 12
#define TOTAL_JOBS 345
#define MAX_RELATIVE_NODES 32
#define TOTAL_CLUSTERS_PER_DIE 8
#define BIG_NUMBER FLT_MAX
#define TOTAL_CLUSTERS 16
#define MAX_KEY_LENGTH 32
#define MAX_VALUE_LENGTH 16384
#define MAX_OBJECT_LENGTH 16384
#define JSON_STRING_SIZE 300000
