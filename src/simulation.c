#include "simulation.h"

const string_view comment          = const_str("#");
const string_view header           = const_str("@");
const string_view header_global    = const_str("@Global");
const string_view header_jobs      = const_str("@Jobs");
const string_view header_job_names = const_str("@JobNames");
const string_view header_ops       = const_str("@Operations");
const string_view header_op_names  = const_str("@OperationNames");
const string_view header_stns      = const_str("@Stations");
const string_view header_stn_names = const_str("@StationNames");

const string_view global_start_time = const_str("StartTime");
const string_view global_end_time   = const_str("EndTime");

implArray(uint16_t);
implArray(station_ptr);
implArray(operation_ptr);
implArray(job_ptr);

#define COUNT_VALUES() \
    uint32_t valid_lines = 0; \
    for (uint32_t i = *index + 1; i < lines->length; i++) { \
        string_view* line = &lines->data[i]; \
        if (line->length == 0) \
            break; \
        if (string_view_starts_with(line, &comment)) \
            continue; \
        if (string_view_starts_with(line, &header)) \
            break; \
        valid_lines++; \
    }

#define PROCESS_LOOP(expression) \
    uint32_t line_count = 0; \
    for (uint32_t i = *index + 1; i < lines->length; i++) { \
        string_view* line = &lines->data[i]; \
        if (line_count == valid_lines) \
            break; \
        expression; \
        line_count++; \
    }

void simulation_process_global(simulation* sim, array(string_view)* lines, uint32_t* index) {
    COUNT_VALUES();
    
    PROCESS_LOOP({
        if (string_view_starts_with(line, &global_start_time)) {
            array(string_view)* parts = string_view_split(line, ';');
            assert(parts->length == 2);
            int success = sscanf(parts->data[1].c_str, "%hu", &sim->start_time);
            assert(success == 1);
        } else if (string_view_starts_with(line, &global_end_time)) {
            array(string_view)* parts = string_view_split(line, ';');
            assert(parts->length == 2);
            int success = sscanf(parts->data[1].c_str, "%hu", &sim->end_time);
            assert(success == 1);
        } else {
            printf("Unknown global parameter: %.*s\n", line->length, line->c_str);
        }
    });
}

void simulation_process_jobs(simulation* sim, array(string_view)* lines, uint32_t* index) {
    COUNT_VALUES();
    
    if (sim->jobs == NULL)
        sim->jobs = array_new(job_ptr, valid_lines);

    PROCESS_LOOP({
        job* j = malloc(sizeof(job));
        j->id = line_count;

        array(string_view)* parts = string_view_split(line, ';');
        assert(parts->length == 2);
        int success = sscanf(parts->data[0].c_str, "%hu", &j->due_time);
        assert(success == 1);

        array(string_view)* op_ids = string_view_split(&parts->data[1], ' ');
        assert(op_ids->length > 0);
        j->operations_to_do = array_new(uint16_t, op_ids->length);
        foreach(k, op_ids) {
            uint16_t op_id;
            int success = sscanf(op_ids->data[k].c_str, "%hu", &op_id);
            assert(success == 1);
            j->operations_to_do->data[k] = op_id;
        }

        j->name_id = line_count;
        sim->jobs->data[line_count] = j;
    });
}

void simulation_process_job_names(simulation* sim, array(string_view)* lines, uint32_t* index) {
    COUNT_VALUES();
    
    if (sim->job_names == NULL)
        sim->job_names = array_new(string_view, valid_lines);

    PROCESS_LOOP({
        sim->job_names->data[line_count].c_str = line->c_str;
        sim->job_names->data[line_count].length = line->length;
    });
}

void simulation_process_ops(simulation* sim, array(string_view)* lines, uint32_t* index) {
    COUNT_VALUES();
    
    if (sim->operations == NULL)
        sim->operations = array_new(operation_ptr, valid_lines);

    PROCESS_LOOP({
        operation* op = malloc(sizeof(operation));
        op->id = line_count;
        
        array(string_view)* parts = string_view_split(line, ';');
        assert(parts->length == 2);

        int success = sscanf(parts->data[0].c_str, "%hu", &op->duration);
        assert(success == 1);

        array(string_view)* stn_ids = string_view_split(&parts->data[1], ' ');
        op->can_be_done_at = array_new(uint16_t, stn_ids->length);
        foreach(j, stn_ids) {
            uint16_t stn_id;
            success = sscanf(stn_ids->data[j].c_str, "%hu", &stn_id);
            assert(success == 1);
            op->can_be_done_at->data[j] = stn_id;
        }

        op->name_id = line_count;
        sim->operations->data[line_count] = op;
    });
}

void simulation_process_op_names(simulation* sim, array(string_view)* lines, uint32_t* index) {
    COUNT_VALUES();
    
    if (sim->op_names == NULL)
        sim->op_names = array_new(string_view, valid_lines);

    PROCESS_LOOP({
        sim->op_names->data[line_count].c_str = line->c_str;
        sim->op_names->data[line_count].length = line->length;
    });
}

void simulation_process_stns(simulation* sim, array(string_view)* lines, uint32_t* index) {
    COUNT_VALUES();
    
    if (sim->stations == NULL)
        sim->stations = array_new(station_ptr, valid_lines);
    
    PROCESS_LOOP({
        station* stn = malloc(sizeof(station));
        stn->id = line_count;
        
        int success = sscanf(line->c_str, "%hu;%hu-%hu", &stn->wait_capacity, &stn->available_from, &stn->available_to);
        assert(success == 3);
        
        stn->name_id = line_count;
        sim->stations->data[line_count] = stn;
    });
}

void simulation_process_stn_names(simulation* sim, array(string_view)* lines, uint32_t* index) {
    COUNT_VALUES();
    
    if (sim->stn_names == NULL)
        sim->stn_names = array_new(string_view, valid_lines);

    PROCESS_LOOP({
        sim->stn_names->data[line_count].c_str = line->c_str;
        sim->stn_names->data[line_count].length = line->length;
    });
}

#define PROCESS_HEADER(name) \
    if (string_view_equals(line, &header_##name)) { \
        simulation_process_##name(sim, lines, &i);  \
        continue;                                   \
    }

void simulation_process_lines(simulation* sim, array(string_view)* lines) {
    foreach(i, lines) {
        string_view* line = &lines->data[i];

        if (line->length == 0)
            continue;
        
        if (string_view_starts_with(line, &comment))
            continue;

        if (!string_view_starts_with(line, &header))
            continue;

        PROCESS_HEADER(global);
        PROCESS_HEADER(jobs);
        PROCESS_HEADER(job_names);
        PROCESS_HEADER(ops);
        PROCESS_HEADER(op_names);
        PROCESS_HEADER(stns);
        PROCESS_HEADER(stn_names);
    }
}

simulation* simulation_new(const string_view* input_file) {
    simulation* sim = malloc(sizeof(simulation));

    sim->input_file = input_file;
    
    sim->stn_names  = NULL;
    sim->op_names   = NULL;
    sim->job_names  = NULL;

    sim->stations   = NULL;
    sim->operations = NULL;
    sim->jobs       = NULL;

    array(string_view)* lines = string_view_split(input_file, '\n');
    simulation_process_lines(sim, lines);
    array_free(lines);

    return sim;
}

void simulation_print_fix_data(simulation *sim) {
    // Print the data
    printf("> %sGlobal parameters%s:\n", ANSI_GREEN, ANSI_RESET);
    printf("  | Start time | %02hu |\n", sim->start_time);
    printf("  | End time   | %02hu |\n", sim->end_time);

    printf("\n> %sStations%s:\n", ANSI_GREEN, ANSI_RESET);
    printf("  | ID | Name            | Cap. | Avail.  |\n");
    printf("  |----|-----------------|------|---------|\n");
    foreach(i, sim->stations) {
        station* stn = sim->stations->data[i];
        printf("  | %02hu | %-16.*s|  %02hu  | %02hu - %02hu |\n", i, sim->stn_names->data[stn->name_id].length, sim->stn_names->data[stn->name_id].c_str, stn->wait_capacity, stn->available_from, stn->available_to);
    }

    printf("\n> %sOperations%s:\n", ANSI_GREEN, ANSI_RESET);
    printf("  | ID | Name                    | Dur. | Stations...\n");
    printf("  |----|-------------------------|------|-------------\n");
    foreach(i, sim->operations) {
        operation* op = sim->operations->data[i];
        printf("  | %02hu | %-24.*s|  %02hu  | ", i, sim->op_names->data[op->name_id].length, sim->op_names->data[op->name_id].c_str, op->duration);
        foreach(j, op->can_be_done_at) {
            printf("%02hu", op->can_be_done_at->data[j]);
            if (j < op->can_be_done_at->length - 1)
                printf("/");
        }
        printf(" \n");
    }

    printf("\n> %sJobs%s:\n", ANSI_GREEN, ANSI_RESET);
    printf("  | ID | Due to | Operations...\n");
    printf("  |----|--------|---------------\n");
    foreach(i, sim->jobs) {
        job* j = sim->jobs->data[i];
        printf("  | %02hu |   %02hu   | ", i, j->due_time);
        foreach(k, j->operations_to_do) {
            printf("%02hu", j->operations_to_do->data[k]);
            if (k < j->operations_to_do->length - 1)
                printf("->");
        }
        printf(" \n");
    }
}

void simulation_free(simulation* sim) {
    array_free(sim->jobs);
    array_free(sim->operations);
    array_free(sim->stations);
    
    array_free(sim->job_names);
    array_free(sim->op_names);
    array_free(sim->stn_names);

    free(sim);
}