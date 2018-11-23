#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include <numeric>
#include <cstdlib>
#include <algorithm>
#include <cstddef>

using namespace std;

// Global System Time
long int system_time = 0;
long int last_arrivals_time = -1;

// Process Definition
typedef struct {

    string name;
    string instruction_file;
    long int priority;
    long int arrival_time;
    long int leaving_time;
    vector<string> instruction_names;
    vector<long int> instruction_times;
    long int execution_time;
    long int current_instruction_index;
    long int maximum_instruction_index;

} process;

// Read the definition file to create process objects
void read_definition_file(vector<process*>&, char*);
// Read each process's file to get instructions
void read_process_files(vector<process*>&);
// Show ready queue by considering their priority and arrival time
void show_by_priority(list<process*>&, ofstream&);
// Insert a process to the appropriate place in ready queue
void insert_by_priority(list<process*>&, process*);
// Given a process, show waiting and turnaround time
void show_statistics(vector<process*>&, ofstream&);


int main(int argc, char *argv[])
{
    char* definition_file_pointer = argv[1];
    // Arrival Process's List
    vector<process*> processes;

    read_definition_file(processes, definition_file_pointer);

    read_process_files(processes);
    // Backup List for Processes
    vector<process*> processes_backup = processes;

    reverse(processes.begin(), processes.end());

    process* current_process = 0;

    // Ready Queue
    list<process*> ready_queue;

    ofstream output_file;
    output_file.open("output.txt");

    show_by_priority(ready_queue, output_file);
    while (true) {

        // Check if there is an arrival, if so take arriving process to the ready queue
        if(!processes.empty() && processes.back() -> arrival_time <= system_time) {
            while(!processes.empty() && processes.back() -> arrival_time <= system_time) {
                insert_by_priority(ready_queue, processes.back());
                processes.pop_back();
            }
            show_by_priority(ready_queue, output_file);
        }

        // Check if there is a process in ready queue, if so, process it's instruction
        if(!ready_queue.empty()) {
            current_process = ready_queue.front();
            system_time += current_process -> instruction_times[current_process -> current_instruction_index];
            current_process -> execution_time += current_process -> instruction_times[current_process -> current_instruction_index++];
            // If "exit" instruction is executed, remove the process from the ready queue
            if (current_process -> current_instruction_index == current_process -> maximum_instruction_index){
                current_process -> leaving_time = system_time;
                ready_queue.pop_front();
                if(!processes.empty() && processes.back() -> arrival_time == system_time) {
                    continue;
                }
                show_by_priority(ready_queue, output_file);

            }

        } else {
            // If ready queue and arrival queue is empty, then there is no more thing do to, terminate system.
            if(!processes.empty()) {
                system_time =  processes.back() -> arrival_time;
            } else {
                break;
            }
        }

    }

    show_statistics(processes_backup, output_file);

    output_file.close();

    return 0;

}
// Parse the definition file and create process objects
void read_definition_file(vector<process*>& processes, char* definition_file_pointer) {

    ifstream definition_file(definition_file_pointer);

    if (definition_file.is_open()){

        string line;

        while(getline(definition_file, line)){

            process* new_process_pointer = new process();

            int elementCounter = 0;

            istringstream iss(line);

            for(string element; iss >> element; ){

                switch(elementCounter++) {

                    case 0:
                        new_process_pointer -> name = element;
                        break;
                    case 1:
                        new_process_pointer -> priority = atol(element.c_str());
                        break;
                    case 2:
                        new_process_pointer -> instruction_file = element;
                        break;
                    case 3:
                        new_process_pointer -> arrival_time = atol(element.c_str());
                        break;
                    default:
                        throw "File-Format Module CANNOT parse the file!";;

                }

            }

            processes.push_back(new_process_pointer);

        }

        definition_file.close();

    } else
        cout << "Unable to open file";
}
// Read process code files to get instruction names and times
void read_process_files(vector<process*>& processes) {

    for (vector<process*>::iterator it = processes.begin() ; it != processes.end(); ++it) {

        string filename_with_extension = (*it) -> instruction_file + ".txt";

        char cstr[filename_with_extension.size() + 1];
        strcpy(cstr, filename_with_extension.c_str());	// or pass &s[0]

        ifstream code_file(cstr);

        if (code_file.is_open()){

            string line;

            (*it) -> execution_time = 0;

            while(getline(code_file, line)){

                istringstream iss(line);

                int elementCounter = 0;

                for(string element; iss >> element; ){

                    switch(elementCounter++) {

                        case 0:
                            (*it) -> instruction_names.push_back(element);
                            break;
                        case 1:
                            (*it) -> instruction_times.push_back(atol(element.c_str()));
                            break;
                        default:
                            throw "File-Format Module CANNOT parse the file!";;
                    }

                }

            }

            (*it) -> current_instruction_index = 0;
            (*it) -> maximum_instruction_index = (*it) -> instruction_times.size();
            (*it) -> execution_time = 0;
            (*it) -> leaving_time = -1;

            code_file.close();

        } else
            cout << "Unable to open file";

    }

}
// Iterate through ready queue and write it output to the output file.
void show_by_priority(list<process*>& ready_queue, ofstream& output_file){

    //cout << system_time << ":";
    output_file << system_time << ":";
    //cout << "HEAD" << "-";
    output_file << "HEAD" << "-";

    if(ready_queue.empty()) {
        //cout << "-";
        output_file << "-";
    }

    for(list<process*>::iterator ready_queue_iterator = ready_queue.begin(); ready_queue_iterator != ready_queue.end(); ready_queue_iterator++){
        //cout << (*ready_queue_iterator) -> name << "[" << (*ready_queue_iterator) -> current_instruction_index + 1 << "]" << "-";
        output_file << (*ready_queue_iterator) -> name << "[" << (*ready_queue_iterator) -> current_instruction_index + 1 << "]" << "-";
    }

    //cout << "TAIL" << "\n";
    output_file << "TAIL" << "\n";
}

// Find the appropriate place for the given process based on priority and arrival time, then call list's insert method
void insert_by_priority(list<process*>& ready_queue, process* new_process_pointer) {

    list<process*>::iterator ready_queue_iterator;

    for(ready_queue_iterator = ready_queue.begin(); ready_queue_iterator != ready_queue.end(); ready_queue_iterator++){
        if ((*ready_queue_iterator) -> priority > new_process_pointer -> priority) {
            break;
        } else if ((*ready_queue_iterator) -> priority == new_process_pointer -> priority) {
            if ((*ready_queue_iterator) -> arrival_time > new_process_pointer -> arrival_time) {
                break;
            }
        }
    }

    ready_queue.insert(ready_queue_iterator, new_process_pointer);
}
// Show turnaround and waiting time for processes
void show_statistics(vector<process*>& processes_backup, ofstream& output_file) {

    //cout << "\n";
    output_file << "\n";

    for (vector<process*>::iterator it = processes_backup.begin() ; it != processes_backup.end(); ++it) {
        //cout <<  "Turnaround time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time << " ms\n" ;
        output_file <<  "Turnaround time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time << " ms\n" ;
        //cout <<  "Waiting time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time - (*it) -> execution_time << " ms\n" ;
        output_file <<  "Waiting time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time - (*it) -> execution_time << "\n" ;
    }

}


