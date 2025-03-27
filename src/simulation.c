#include "simulation.h"

implArray(station_ptr);
implArray(operation_ptr);
implArray(job_ptr);

simulation* simulation_new(const string_view* input_file) {
    simulation* sim = malloc(sizeof(simulation));

    sim->input_file = input_file;
    sim->names      = array_new(string_view  , 0);
    sim->stations   = array_new(station_ptr  , 0);
    sim->operations = array_new(operation_ptr, 0);
    sim->jobs       = array_new(job_ptr      , 0);

    array(string_view)* lines = string_view_split(input_file, '\n');
    simulation_process_lines(sim, lines);
    array_free(lines);

    return sim;
}

void simulation_process_lines(simulation* sim, array(string_view)* lines) {

}

void simulation_free(simulation* sim) {
    array_free(sim->jobs);
    array_free(sim->operations);
    array_free(sim->stations);
    array_free(sim->names);

    free(sim);
}