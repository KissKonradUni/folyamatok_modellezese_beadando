#ifndef SIMULATION_H
#define SIMULATION_H

#include "array.h"
#include "mystring.h"

typedef struct __station {
    // Fix parameters
    const string_view* name;

    const uint16_t id;
    const uint16_t wait_capacity;
    const uint8_t  available_from; // 0 - 24
    const uint8_t  available_to;   // 0 - 24
} station;

typedef station* station_ptr;
defArray(station_ptr);

typedef struct __operation {
    // Fix parameters
    const string_view name;

    const uint16_t id;
    const uint16_t duration;
    
    array(station_ptr)* can_be_done_at;
} operation;

typedef operation* operation_ptr;
defArray(operation_ptr);

typedef struct __job {
    // Fix parameters
    const string_view name;

    const uint16_t id;
    array(operation_ptr)* operations_to_do;
} job;

typedef job* job_ptr;
defArray(job_ptr);

typedef struct __instance {
    const string_view* input_file;

    array(string_view)* names;

    array(station_ptr)* stations;
    array(operation_ptr)* operations;
    array(job_ptr)* jobs;
} simulation;

simulation* simulation_new(const string_view* input_file);
void simulation_process_lines(simulation* sim, array(string_view)* lines);
void simulation_free(simulation* sim);

#endif // SIMULATION_H