#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "simulation.h"

/**
 * Stores information about a job that is
 * currently being processed.
 * (In the current iteration of the simulation)
 */
typedef struct __active_job {
    job* job;                      // Pointer to the job that is being processed

    uint16_t current_operation_id; // ID of the operation that is currently being processed
    uint16_t operation_ends_at;    // start_time + duration
                                   // (UINT16_MAX if the job is not being processed)
    
    station* assigned_station;     // Pointer to the station that is processing the job
                                   // (May be NULL if the job is not being processed)
} active_job;

typedef active_job* active_job_ptr;
defArray(active_job_ptr);

/**
 * Stores information about a station
 * that is currently processing an operation.
 * (In the current iteration of the simulation)
 */
typedef struct __station_queue {
    station*               station; // Pointer to the station that the queue belongs to
    array(active_job_ptr)* jobs;    // Pointer to the queue of jobs that are waiting to be processed
} station_queue;

typedef station_queue* station_queue_ptr;
defArray(station_queue_ptr);

/**
 * Stores information about an event
 * that has been executed.
 * Helps constructing the history of the simulation.
 */
typedef struct __execution_record {
    uint16_t job_id;     // ID of the job that was processed
    uint16_t op_id;      // ID of the operation that was processed
    uint16_t station_id; // ID of the station that processed the job

    uint16_t start_time; // Time when the operation started
    uint16_t end_time;   // Time when the operation ended
} execution_record;

typedef execution_record* execution_record_ptr;
defArray(execution_record_ptr);

/**
 * Stores information about the history of
 * a job throughout the simulation.
 * @remark Some values might have UINT16_MAX as value
 *         if the job is yet to be processed.
 */
typedef struct __job_history {
    uint16_t job_id;                      // ID of the job

    array(execution_record_ptr)* records; // Pointer to the list of execution records
                                          // that belong to this job
                                          // (Might contain NULL)
    
    uint16_t finished_time;               // Time when the job was finished
    uint16_t tardiness;                   // How late the job was (finished_time - due_time)
    uint16_t waiting_time;                // How long the job waited in the queue
    uint16_t processing_time;             // How long the job was being processed
} job_history;

typedef job_history* job_history_ptr;
defArray(job_history_ptr);

/**
 * Stores information about the history of
 * a station throughout the simulation.
 * @remark Some values might have UINT16_MAX as value
 *         if the station is yet to be processed.
 */
typedef struct __station_history {
    uint16_t station_id;                  // ID of the station

    array(execution_record_ptr)* records; // Pointer to the list of execution records
                                          // that belong to this station
                                          // (Might contain NULL)

    uint16_t busy_time;                   // Time spent processing jobs
    uint16_t idle_time;                   // Time spent idle
    uint16_t out_time;                    // Time spent unavailable
    float utilization;                    // Utilization of the station
} station_history;

typedef station_history* station_history_ptr;
defArray(station_history_ptr);

/**
 * Scheduler structure.
 *
 * Contains all the information that was
 * accumulated during the simulation.
 *
 * It is used to generate the final report
 */
typedef struct __scheduler {
    // Static data
    simulation* sim;                                 // Pointer to the simulation object
                                                     // (Holds all the static data.)

    // Statistics
    uint16_t current_time;                           // Current time of the simulation (0 - 24)
    uint16_t end_time;                               // End time of the last job       (0 - 24)

    uint16_t longest_tardiness;                      // How late was the most tardy job
    uint16_t longest_waiting_time;                   // The longest waiting time of a job

    uint16_t total_tardiness;                        // The total tardiness of all jobs
    uint16_t total_waiting_time;                     // The total waiting time of all jobs

    uint16_t tardy_jobs;                             // The number of tardy jobs (tardiness > 0)

    // Simulation data
    array(station_queue_ptr)* station_queues;        // Pointer to the list of station queues
    array(active_job_ptr)*    active_jobs;           // Pointer to the list of active jobs

    // Gannt chart data
    uint16_t record_count;                           // Number of records in the execution records
                                                     // (The array itself is larger and dynamically resized)
    array(execution_record_ptr)* execution_records;  // Pointer to the list of execution records
    array(job_history_ptr)*      job_histories;      // Pointer to the list of job histories
    array(station_history_ptr)*  station_histories;  // Pointer to the list of station histories
} scheduler;

scheduler* scheduler_new(simulation* sim);
void scheduler_free(scheduler* sch);

/**
 * @brief Simulates the scheduler using the
 * First-Come-First-Serve (FCFS) algorithm.
 * (aka. FIFO)
 * @param sch Pointer to the scheduler object
 */
void scheduler_simulate_fcfs(scheduler* sch);

#endif // SCHEDULER_H