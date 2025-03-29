#include "scheduler.h"

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
    sch->longest_waiting_time = 0;

    sch->total_tardiness    = 0;
    sch->total_waiting_time = 0;

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
        
        queue->station = sim->stations->data[i];
        queue->jobs    = array_new(active_job_ptr, queue->station->wait_capacity);

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

        history->finished_time   = UINT16_MAX;
        history->tardiness       = 0;
        history->waiting_time    = 0;
        history->processing_time = 0;

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

        history->busy_time      = 0;
        history->idle_time      = 0;
        history->out_time       = 0;
        history->utilization    = 0.0f;

        sch->station_histories->data[i] = history;
    }
    
    return sch;
}

void scheduler_free(scheduler* sch) {
    assert(sch != NULL);

    foreach(i, sch->station_queues) {
        station_queue* queue = sch->station_queues->data[i];
        array_free(queue->jobs);
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