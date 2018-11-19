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

long int system_time = 0;

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

struct less_than_key{
    inline bool operator() (const process* process1, const process* process2){
        return process1 -> arrival_time > process2 -> arrival_time;
    }
};

void read_definition_file(vector<process*>&, char*);
void read_process_files(vector<process*>&);
void show_by_priority(list<process*>&, ofstream&);
void insert_by_priority(list<process*>&, process*);


int main(int argc, char *argv[])
{
    char* definition_file_pointer = argv[1];

    vector<process*> processes;

    read_definition_file(processes, definition_file_pointer);

    read_process_files(processes);

    vector<process*> processes_backup = processes;

    sort(processes.begin(), processes.end(), less_than_key());

    process* current_process = 0;

    list<process*> ready_queue;

    ofstream output_file;
    output_file.open("output.txt");

    show_by_priority(ready_queue, output_file);

    while (true) {

        while(!processes.empty() && processes.back() -> arrival_time <= system_time) {
            insert_by_priority(ready_queue, processes.back());
            processes.pop_back();
            show_by_priority(ready_queue, output_file);
        }

        if(!ready_queue.empty()) {

            current_process = ready_queue.front();
            system_time += current_process -> instruction_times[current_process -> current_instruction_index];
            current_process -> execution_time += current_process -> instruction_times[current_process -> current_instruction_index++];
            if (current_process -> current_instruction_index == current_process -> maximum_instruction_index){
                current_process -> leaving_time = system_time;
                ready_queue.pop_front();
                show_by_priority(ready_queue, output_file);
            }

        } else {

            if(!processes.empty()) {
                system_time =  processes.back() -> arrival_time;
            } else {
                break;
            }
        }

    }

    cout << "\n";
    output_file << "\n";

    for (vector<process*>::iterator it = processes_backup.begin() ; it != processes_backup.end(); ++it) {
        cout <<  "Turnaround time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time << " ms\n" ;
        output_file <<  "Turnaround time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time << " ms\n" ;
        cout <<  "Waiting time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time - (*it) -> execution_time << " ms\n" ;
        output_file <<  "Waiting time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time - (*it) -> execution_time << " ms\n" ;
    }

    output_file.close();

    return 0;

}

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

void show_by_priority(list<process*>& ready_queue, ofstream& output_file){

    cout << system_time << ":";
    output_file << system_time << ":";
    cout << "HEAD" << "-";
    output_file << "HEAD" << "-";

    if(ready_queue.empty()) {
        cout << "-";
        output_file << "-";
    }

    for(list<process*>::iterator ready_queue_iterator = ready_queue.begin(); ready_queue_iterator != ready_queue.end(); ready_queue_iterator++){
        cout << (*ready_queue_iterator) -> name << "[" << (*ready_queue_iterator) -> current_instruction_index + 1 << "]" << "-";
        output_file << (*ready_queue_iterator) -> name << "[" << (*ready_queue_iterator) -> current_instruction_index + 1 << "]" << "-";
    }


    cout << "TAIL" << "\n";
    output_file << "TAIL" << "\n";
}

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
