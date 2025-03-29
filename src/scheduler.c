#include "scheduler.h"

#include <string.h>

implArray(active_job_ptr);
implArray(station_queue_ptr);
implArray(execution_record_ptr);
implArray(job_history_ptr);
implArray(station_history_ptr);

scheduler* scheduler_new(simulation* sim) {
    assert(sim != NULL);
    
    scheduler* sch = malloc(sizeof(scheduler));
    assert(sch != NULL);

    sch->sim = sim;

    sch->current_time = sim->start_time;
    sch->end_time     = sim->end_time;

    sch->longest_tardiness    = 0;
    //sch->longest_waiting_time = 0;

    sch->total_tardiness    = 0;
    //sch->total_waiting_time = 0;

    sch->tardy_jobs = 0;

    sch->station_queues    = array_new(station_queue_ptr   , sim->stations->length);
    sch->active_jobs       = array_new(active_job_ptr      , sim->jobs->length    );
    
    sch->record_count      = 0;
    sch->execution_records = array_new(execution_record_ptr, sim->jobs->length * sim->operations->length); // Rough estimate
    sch->job_histories     = array_new(job_history_ptr     , sim->jobs->length    );
    sch->station_histories = array_new(station_history_ptr , sim->stations->length);

    foreach(i, sch->station_queues) {
        station_queue* queue = malloc(sizeof(station_queue));
        assert(queue != NULL);
        
        queue->station    = sim->stations->data[i];
        queue->job        = NULL;
        queue->jobs_queue = array_new(active_job_ptr, queue->station->wait_capacity);
        memset(queue->jobs_queue->data, 0, sizeof(active_job_ptr) * queue->jobs_queue->length);

        sch->station_queues->data[i] = queue;
    }

    foreach(i, sch->active_jobs) {
        active_job* job = malloc(sizeof(active_job));
        assert(job != NULL);
        
        job->job                  = sim->jobs->data[i];
        job->current_operation_id = 0;
        job->operation_ends_at    = UINT16_MAX; // Not started yet
        job->assigned_station     = NULL;

        sch->active_jobs->data[i] = job;
    }

    // We leave the execution records as NULL

    foreach(i, sch->job_histories) {
        job_history* history = malloc(sizeof(job_history));
        assert(history != NULL);
        
        history->job_id          = sim->jobs->data[i]->id;

        // We allocate enough space for the maximum number of
        // operations that can be executed by this job
        // (This is a rough estimate, may be reallocated later)
        history->records         = array_new(execution_record_ptr, sim->jobs->data[i]->operations_to_do->length);
        memset(history->records->data, 0, sizeof(execution_record_ptr) * history->records->length);

        history->finished_time   = UINT16_MAX;
        history->tardiness       = 0;

        sch->job_histories->data[i] = history;
    }

    foreach(i, sch->station_histories) {
        station_history* history = malloc(sizeof(station_history));
        assert(history != NULL);
        
        history->station_id     = sim->stations->data[i]->id;

        // We allocate a rough estimate, that is most likely
        // larger than the maximum number of operations.
        // (We want to avoid reallocating these arrays)
        history->records        = array_new(execution_record_ptr, sim->jobs->length * sim->operations->length);
        memset(history->records->data, 0, sizeof(execution_record_ptr) * history->records->length);

        history->busy_time      = 0;

        sch->station_histories->data[i] = history;
    }
    
    return sch;
}

void scheduler_free(scheduler* sch) {
    assert(sch != NULL);

    foreach(i, sch->station_queues) {
        station_queue* queue = sch->station_queues->data[i];
        array_free(queue->jobs_queue);
        free(queue);
    }
    array_free(sch->station_queues);

    foreach(i, sch->active_jobs) {
        active_job* job = sch->active_jobs->data[i];
        free(job);
    }
    array_free(sch->active_jobs);

    for (int i = 0; i < sch->record_count; i++) {
        execution_record* record = sch->execution_records->data[i];
        free(record);
    }
    array_free(sch->execution_records);

    foreach(i, sch->job_histories) {
        job_history* history = sch->job_histories->data[i];
        array_free(history->records);
        free(history);
    }
    array_free(sch->job_histories);

    foreach(i, sch->station_histories) {
        station_history* history = sch->station_histories->data[i];
        array_free(history->records);
        free(history);
    }
    array_free(sch->station_histories);

    free(sch);
}

void scheduler_create_execution_record(scheduler* sch, uint16_t job_id, uint16_t op_id, uint16_t stn_id) {
    // Check if we have free space in the array
    if (sch->record_count >= sch->execution_records->length) {
        // Resize the array
        array_resize(execution_record_ptr, &sch->execution_records, sch->execution_records->length * 2);
    }

    // Create the execution record
    execution_record* record = malloc(sizeof(execution_record));
    assert(record != NULL);
    
    record->job_id     = job_id;
    record->op_id      = op_id;
    record->station_id = stn_id;

    record->start_time = sch->current_time;
    record->end_time   = UINT16_MAX;

    // Add the record to the array
    sch->execution_records->data[sch->record_count] = record;
    sch->record_count++;

    // Add the record to the station history
    station_history* stn_history = sch->station_histories->data[stn_id];
    assert(stn_history != NULL);
    // Check if we have free space in the array
    uint8_t first_non_null = 0;
    foreach(i, stn_history->records) {
        if (stn_history->records->data[i] == NULL) {
            first_non_null = i;
            break;
        }
    }
    if (first_non_null >= stn_history->records->length) {
        // Resize the array
        array_resize(execution_record_ptr, &stn_history->records, stn_history->records->length * 2);
    }
    // Add the record to the array
    stn_history->records->data[first_non_null] = record;
    
    // Add the record to the job history
    job_history* job_history = sch->job_histories->data[job_id];
    assert(job_history != NULL);
    // Check if we have free space in the array
    first_non_null = 0;
    foreach(i, job_history->records) {
        if (job_history->records->data[i] == NULL) {
            first_non_null = i;
            break;
        }
    }
    if (first_non_null >= job_history->records->length) {
        // Resize the array
        array_resize(execution_record_ptr, &job_history->records, job_history->records->length * 2);
    }
    // Add the record to the array
    job_history->records->data[first_non_null] = record;
}

void scheduler_push_to_station_queue(scheduler* sch, active_job* job, station_queue* stn_queue) {
    assert(sch != NULL);
    assert(job != NULL);
    assert(stn_queue != NULL);

    // Check if the station is available
    if (stn_queue->job == NULL) {
        // The station is idle
        stn_queue->job = job;
        return;
    }
    // The station is busy
    // Search for the first empty slot in the queue
    for (uint8_t i = 0; i < stn_queue->jobs_queue->length; i++) {
        if (stn_queue->jobs_queue->data[i] == NULL) {
            // Found an empty slot
            stn_queue->jobs_queue->data[i] = job;
            return;
        }
    }
    // The queue is full
    printf("  %sError%s: Trying to push on to a full queue\n", ANSI_RED, ANSI_RESET);
    assert(0);
}

/**
 * @brief Checks if a station is available.
 * 
 * @param sch Pointer to the scheduler object
 * @param station_id ID of the station to check
 * @return station_state The state of the station (0 and 1 can be used, 2 or 3 should not be used)
 */
station_state scheduler_is_station_available(scheduler* sch, uint16_t station_id) {
    assert(sch != NULL);
    
    station_queue* stn_queue = sch->station_queues->data[station_id];
    assert(stn_queue != NULL);

    station* station = stn_queue->station;
    assert(station != NULL);

    // Check if the station is available
    if (sch->current_time < station->available_from || sch->current_time > station->available_to) {
        // The station is not available
        return STATION_UNAVAILABLE;
    }

    // Check if the station has a full queue
    array(active_job_ptr)* queue_arr = stn_queue->jobs_queue;
    uint8_t count = 0;
    foreach(i, queue_arr)
        if (queue_arr->data[i] != NULL)
            count++;

    if (count >= sch->station_queues->data[station_id]->station->wait_capacity) {
        // The queue is full
        return STATION_FULL;
    }

    // Check if the station is busy
    if (stn_queue->job != NULL) {
        // The station is busy
        return STATION_BUSY;
    }

    // The station is idle
    return STATION_IDLE;
}

/**
 * @brief Gets the execution record by filtering.
 * @remark You may set any id to UINT16_MAX to ignore it.
 * 
 * @param sch Pointer to the scheduler object
 * @param job_id
 * @param op_id 
 * @param stn_id 
 * @return execution_record* Pointer to the execution record (NULL if not found)
 */
execution_record* scheduler_get_execution_record(scheduler* sch, uint16_t job_id, uint16_t op_id, uint16_t stn_id) {
    assert(sch != NULL);
    
    // Search for the execution record
    for (uint16_t i = 0; i < sch->record_count; i++) {
        execution_record* record = sch->execution_records->data[i];

        // Check if the record matches the filter
        if ((job_id == UINT16_MAX || record->job_id     == job_id) &&
            (op_id  == UINT16_MAX || record->op_id      == op_id ) &&
            (stn_id == UINT16_MAX || record->station_id == stn_id)) {
            // The record matches the filter
            return record;
        }
    }

    return NULL;
}

void scheduler_iterate_fcfs(scheduler* sch) {
    // Go through the active jobs and update them
    // If idle     -> do nothing yet.
    // If busy     -> do nothing if the job isn't finished
    // If finished -> update the history and the record
    // If complete -> do nothing
    foreach(i, sch->active_jobs) {
        active_job* job = sch->active_jobs->data[i];
        assert(job != NULL);

        // Check if the job is busy
        if (job->operation_ends_at == UINT16_MAX) {
            // The job is idle
            continue;
        }
        // Check if the job is complete
        if (job->current_operation_id >= job->job->operations_to_do->length) {
            // The job has not finished all operations
            continue;
        }
        // Check if the job has finished
        if (job->operation_ends_at > sch->current_time) {
            // The job is busy
            continue;
        }
        // Check if the job is waiting in queue
        station_queue* stn_queue = sch->station_queues->data[job->assigned_station->id];
        assert(stn_queue != NULL);
        if (job->assigned_station != NULL && stn_queue->job != job) {
            // The job is waiting in queue
            continue;
        }

        // The job has finished the current operation
        // Search for the execution record
        execution_record* record;
        record = scheduler_get_execution_record(sch, job->job->id, job->current_operation_id, job->assigned_station->id);
        assert(record != NULL);
        // Update it
        record->end_time = sch->current_time;

        // Set the job as idle
        job->operation_ends_at = UINT16_MAX;
        job->assigned_station  = NULL;
        job->current_operation_id += 1;

        // Check if the job has finished all operations
        if (job->current_operation_id < job->job->operations_to_do->length) {
            // The job has not finished all operations
            continue;
        }
        job_history* history = sch->job_histories->data[i];
        assert(history != NULL);

        // Update the history
        history->finished_time   = sch->current_time;
        history->tardiness       = max(0, sch->current_time - job->job->due_time);
        if (history->tardiness > 0) {
            // The job is tardy
            sch->total_tardiness    += history->tardiness;
            sch->tardy_jobs++;
            sch->longest_tardiness   = max(sch->longest_tardiness, history->tardiness);
        }
    }

    // Go through the station queues and update them
    // If idle     -> do nothing yet.
    // If busy     -> do nothing if the job isn't finished
    // If finished -> update the history and the record, get the next job
    foreach(i, sch->station_queues) {
        station_queue* queue = sch->station_queues->data[i];
        assert(queue != NULL);

        // Check if the station is busy
        if (queue->job == NULL) {
            // The station is idle
            continue;
        }
        // Check if the station is busy
        if (queue->job->operation_ends_at != UINT16_MAX && queue->job->operation_ends_at > sch->current_time) {
            // The station is busy
            continue;
        }
        
        // The station has finished the current operation
        // Search for the execution record
        execution_record* record;
        record = scheduler_get_execution_record(sch, queue->job->job->id, queue->job->current_operation_id - 1, queue->station->id);
        assert(record != NULL);

        // Update it (Might be unnecessary because we already updated it above)
        record->end_time = sch->current_time;

        station_history* history = sch->station_histories->data[i];
        assert(history != NULL);
        // Update the history
        history->busy_time += sch->current_time - record->start_time;

        // Get the next job in the queue
        if (queue->jobs_queue->data[0] == NULL) {
            // The queue is empty
            // Remains idle
            queue->job = NULL;
            continue;
        }

        // Otherwise
        // Set the next job
        queue->job = queue->jobs_queue->data[0];
        operation* op = sch->sim->operations->data[queue->job->current_operation_id];
        queue->job->operation_ends_at = sch->current_time + op->duration;
        // Shift the queue
        memcpy(queue->jobs_queue->data, queue->jobs_queue->data + 1, sizeof(active_job_ptr) * (queue->jobs_queue->length - 1));
        // Set the last job to NULL
        queue->jobs_queue->data[queue->jobs_queue->length - 1] = NULL;

        // Update the history of this job to account for the queue
        record = scheduler_get_execution_record(sch, queue->job->job->id, op->id, queue->station->id);
        assert(record != NULL);
        record->start_time = sch->current_time;
        record->end_time   = UINT16_MAX;
    }

    // Go through the active jobs and try to assign them to a station
    foreach(i, sch->active_jobs) {
        active_job* job = sch->active_jobs->data[i];
        assert(job != NULL);

        // Check if the job is idle
        if (job->operation_ends_at != UINT16_MAX) {
            // The job is busy
            continue;
        }
        // Check if the job has finished all operations
        if (job->current_operation_id >= job->job->operations_to_do->length) {
            // The job has finished all operations
            continue;
        }
        // Check if the job is waiting in queue
        if (job->assigned_station != NULL) {
            // The job is waiting in queue
            continue;
        }

        // The job is idle
        // Get the operation that the job is going to do
        uint16_t op_id = job->job->operations_to_do->data[job->current_operation_id];
        operation* op = sch->sim->operations->data[op_id];
        assert(op != NULL);

        // Get all the stations that can do the operation
        array(uint16_t)* can_be_done_at = op->can_be_done_at;
        assert(can_be_done_at != NULL);

        // Get the first station that can do the operation
        station* assigned_station = NULL;
        station_queue* stn_queue  = NULL;
        foreach(j, can_be_done_at) {
            uint16_t stn_id = can_be_done_at->data[j];

            // Check if the station is available
            station_state state = scheduler_is_station_available(sch, stn_id);
            if (state == STATION_IDLE || state == STATION_BUSY) {
                // The station is available
                assigned_station = sch->station_queues->data[stn_id]->station;
                stn_queue        = sch->station_queues->data[stn_id];
                break;
            }
        }
        // Check if a station was found
        if (assigned_station == NULL) {
            // No station is available
            // The job remains idle until a station frees up
            continue;
        }
        // The job is idle and a station is available
        // Assign the job to the station
        job->assigned_station = assigned_station;
        job->operation_ends_at = sch->current_time + op->duration; // At best
        // Push the job to the station queue
        scheduler_push_to_station_queue(sch, job, stn_queue);
        // Get the operation that the job is going to do
        op = sch->sim->operations->data[op_id];
        assert(op != NULL);

        // Create an execution record
        scheduler_create_execution_record(sch, job->job->id, job->current_operation_id, assigned_station->id);
    }
}

void scheduler_simulate_fcfs(scheduler* sch) {
    for (; sch->current_time < sch->sim->end_time; sch->current_time++) {
        scheduler_iterate_fcfs(sch);
    }
}

void scheduler_print_results(scheduler* sch) {
    assert(sch != NULL);
    
    printf("\n%s%s  Scheduler Results  %s\n", ANSI_BOLD, ANSI_YELLOW, ANSI_RESET);
    
    // Print general statistics
    printf("\n> %sGeneral statistics%s: \n", ANSI_BLUE, ANSI_RESET);
    printf("  Simulation time range: %02hu:%02hu - %02hu:%02hu\n", 
        min_to_time(sch->sim->start_time),
        min_to_time(sch->sim->end_time)
    );
    
    // Calculate and print utilization stats
    uint16_t total_station_time = 0;
    uint16_t total_busy_time = 0;
    foreach(i, sch->station_histories) {
        station_history* history = sch->station_histories->data[i];
        total_busy_time += history->busy_time;
        total_station_time += (sch->sim->end_time - sch->sim->start_time);
    }
    float utilization = (float)total_busy_time / (float)total_station_time * 100.0f;
    
    printf("  Tardiness statistics:\n");
    printf("  - Total tardiness   : %02hu:%02hu\n", min_to_time(sch->total_tardiness));
    printf("  - Longest tardiness : %02hu:%02hu\n", min_to_time(sch->longest_tardiness));
    printf("  - Tardy jobs        : %d / %d\n", sch->tardy_jobs, sch->active_jobs->length);
    printf("  Resource utilization: %.2f%%\n", utilization);
    
    // Print job completion times
    printf("\n> %sJob completion times%s: \n", ANSI_BLUE, ANSI_RESET);
    printf("  | ID | %-23s | Due Time |   Completed   | Tardiness |\n", "Name");
    printf("  |----|-------------------------|----------|---------------|-----------|\n");
    
    foreach(i, sch->job_histories) {
        job_history* history = sch->job_histories->data[i];
        job* job = sch->sim->jobs->data[i];
        
        char completed_str[20] = "Not completed";
        if (history->finished_time != UINT16_MAX) {
            snprintf(completed_str, sizeof(completed_str), "%02hu:%02hu", 
                min_to_time(history->finished_time)
            );
        }
        
        // Colorize tardy jobs
        const char* color_start = "";
        const char* color_end = "";
        if (history->tardiness > 0 || history->finished_time == UINT16_MAX) {
            color_start = ANSI_RED;
            color_end = ANSI_RESET;
        }
        
        printf("  | %2d | %-23.*s |    %02hu:%02hu | %s%13s%s | %9d |\n", 
            job->id, 
            (int)sch->sim->job_names->data[job->name_id].length,
            sch->sim->job_names->data[job->name_id].c_str,
            min_to_time(job->due_time),
            color_start,
            completed_str,
            color_end,
            history->tardiness
        );
    }
    
    // Print station utilization
    printf("\n> %sStation utilization:%s\n", ANSI_BLUE, ANSI_RESET);
    printf("  | ID | %-23s | Busy Time | Utilization |\n", "Name");
    printf("  |----|-------------------------|-----------|-------------|\n");
    
    foreach(i, sch->station_histories) {
        station_history* history = sch->station_histories->data[i];
        station* station = sch->sim->stations->data[i];
        
        uint16_t available_time = station->available_to - station->available_from;
        float station_utilization = (float)history->busy_time / (float)available_time * 100.0f;
        
        printf("  | %2d | %-23.*s |     %02hu:%02hu |  %9.02f%% |\n", 
               station->id,
               (int)sch->sim->stn_names->data[station->name_id].length,
               sch->sim->stn_names->data[station->name_id].c_str,
               min_to_time(history->busy_time),
               station_utilization);
    }
    
    // Print Gantt chart (simplified version)
    printf("\n> %sOperation history%s:\n", ANSI_BLUE, ANSI_RESET);
    printf("  | Job ID | Operation ID | Station ID | Start |  End  | Duration | Description...\n");
    printf("  |--------|--------------|------------|-------|-------|----------|----------------\n");
    
    for (int i = 0; i < sch->record_count; i++) {
        execution_record* record = sch->execution_records->data[i];
        
        // Skip incomplete records
        if (record == NULL)
            continue;
            
        uint16_t duration = record->end_time - record->start_time;
        
        char end_time_text[20] = " -  ";
        if (record->end_time != UINT16_MAX) {
            snprintf(end_time_text, sizeof(end_time_text), "%02hu:%02hu", 
                min_to_time(record->end_time)
            );
        }

        printf("  |  %5d | %12d | %10d | %02hu:%02hu | %5s | %8d |",
            record->job_id,
            record->op_id,
            record->station_id,
            min_to_time(record->start_time),
            end_time_text,
            duration);
        printf(" Do %s%-23.*s%s's %s%23.*s%s at %s%.*s%s\n",
            ANSI_YELLOW,
            sch->sim->job_names->data[record->job_id].length,
            sch->sim->job_names->data[record->job_id].c_str,
            ANSI_RESET,
            ANSI_YELLOW,
            sch->sim->op_names->data[record->op_id].length,
            sch->sim->op_names->data[record->op_id].c_str,
            ANSI_RESET,
            ANSI_YELLOW,
            sch->sim->stn_names->data[record->station_id].length,
            sch->sim->stn_names->data[record->station_id].c_str,
            ANSI_RESET
        );
    }
    
    printf("\n");
}