#ifndef SIMULATION_H
#define SIMULATION_H

#include "array.h"
#include "mystring.h"

defArray(uint16_t);

typedef struct __station {
    // Fix parameters
    uint16_t name_id;

    uint16_t id;
    uint16_t wait_capacity;
    uint16_t available_from; // 0 - 24
    uint16_t available_to;   // 0 - 24
} station;

typedef station* station_ptr;
defArray(station_ptr);

typedef struct __operation {
    // Fix parameters
    uint16_t name_id;

    uint16_t id;
    uint16_t duration;
    
    array(uint16_t)* can_be_done_at;
} operation;

typedef operation* operation_ptr;
defArray(operation_ptr);

typedef struct __job {
    // Fix parameters
    uint16_t name_id;

    uint16_t id;
    array(uint16_t)* operations_to_do;
} job;

typedef job* job_ptr;
defArray(job_ptr);

typedef struct __instance {
    const string_view* input_file;

    array(string_view)* stn_names;
    array(string_view)* op_names;
    array(string_view)* job_names;

    array(station_ptr)* stations;
    array(operation_ptr)* operations;
    array(job_ptr)* jobs;
} simulation;

simulation* simulation_new(const string_view* input_file);
void simulation_print_fix_data(simulation* sim);
void simulation_free(simulation* sim);

#endif // SIMULATION_H