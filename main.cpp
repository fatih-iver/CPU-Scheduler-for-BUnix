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

void show_by_priority(list<process*>&, process*, ofstream&);
void insert_by_priority(list<process*>&, process*);

int main(int argc, char *argv[])
{

    vector<process*> processes;

    ifstream definition_file(argv[1]);

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
                        break;
                }

            }

            processes.push_back(new_process_pointer);

        }

        definition_file.close();

    } else
        cout << "Unable to open file";
    /**
    cout << "Number of processes: "<<processes.size() << "\n";
    for (vector<process*>::iterator it = processes.begin() ; it != processes.end(); ++it)
        cout << (*it) -> name << " " << (*it) -> priority << " " << (*it) -> instruction_file << " " << (*it) -> arrival_time << "\n" ;
    **/

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
                            {
                            long int execute_time = atol(element.c_str());
                            (*it) -> execution_time += execute_time;
                            (*it) -> instruction_times.push_back(execute_time);
                            break;
                            }
                        default:
                            break;
                    }

                }

            }

            (*it) -> current_instruction_index = 0;
            (*it) -> maximum_instruction_index = (*it) -> instruction_times.size();
            (*it) -> leaving_time = -1;
            code_file.close();

        } else
            cout << "Unable to open file";

    }

    /**
    for (std::vector<process>::iterator m_it = processes.begin() ; m_it != processes.end(); ++m_it){
        for (std::vector<string>::iterator s_it = m_it -> instruction_names.begin() ; s_it != m_it -> instruction_names.end(); ++s_it)
            cout << *s_it <<  " ";
        cout << "\n";
        for (std::vector<long int>::iterator s_it = m_it -> instruction_times.begin() ; s_it != m_it -> instruction_times.end(); ++s_it)
            cout << *s_it <<  " ";
        cout << "\n";
    }
    **/
    /**
    for (std::vector<process*>::iterator it = processes.begin() ; it != processes.end(); it++){
        cout << "(" << (*it) -> arrival_time << ")" << (*it) -> name << " "<< (*it) ->current_instruction_index << " ";
    }

    cout << "\n";
    **/
    vector<process*> backup_processes = processes;
    sort(processes.begin(), processes.end(), less_than_key());

    /**
    for (std::vector<process*>::iterator it = processes.begin() ; it != processes.end(); it++){
        cout << "(" << (*it) -> arrival_time << ")" << " "<< (*it) ->current_instruction_index << " " << (*it) -> name << "-" << (*it) -> maximum_instruction_index << " " << (*it) -> instruction_times[0] << " " ;
    }

    cout << "\n";
    **/


    list<process*> ready_queue;

    process* current_process = 0;

    ofstream output_file;
    output_file.open("output.txt");

    show_by_priority(ready_queue, current_process, output_file);

    while (true){

        if (processes.empty()) {

            if(ready_queue.empty()) {

                if(current_process == 0) {
                    show_by_priority(ready_queue, current_process, output_file);
                    //cout << "Arrivals Queue is Empty, Ready Queue is empty, CPU is idle. Finishing.." << "System Time: " <<  system_time << "\n\n";
                    break;
                } else {
                    system_time += current_process -> instruction_times[current_process -> current_instruction_index];
                    current_process -> current_instruction_index++;
                    if (current_process -> current_instruction_index == current_process -> maximum_instruction_index){
                        current_process -> leaving_time = system_time;
                        current_process = 0;
                        continue;
                    } else {
                        continue;
                    }
                }

            } else {

                if(current_process == 0) {
                        current_process = ready_queue.front();
                        ready_queue.pop_front();
                        show_by_priority(ready_queue, current_process, output_file);
                        continue;
                } else {
                    system_time += current_process -> instruction_times[current_process -> current_instruction_index];
                    current_process -> current_instruction_index++;
                    if (current_process -> current_instruction_index == current_process -> maximum_instruction_index){
                        current_process -> leaving_time = system_time;
                        current_process = 0;
                        continue;
                    } else {
                        continue;
                    }
                }
            }

        } else {

            if (processes.back() -> arrival_time <= system_time) {

                long int number_of_arrivals = 0;

                while(!processes.empty() && processes.back() -> arrival_time <= system_time) {
                    insert_by_priority(ready_queue, processes.back());
                    processes.pop_back();
                    number_of_arrivals++;
                }

                if (current_process == 0) {
                    show_by_priority(ready_queue, current_process, output_file);
                    current_process = ready_queue.front();
                    ready_queue.pop_front();
                    /**
                    if(number_of_arrivals > 1) {
                        show_by_priority(ready_queue, current_process);
                    }
                    **/
                    continue;
                } else {

                    if (ready_queue.front() -> priority < current_process -> priority) {
                        insert_by_priority(ready_queue, current_process);
                        current_process = ready_queue.front();
                        ready_queue.pop_front();
                        show_by_priority(ready_queue, current_process, output_file);
                        continue;
                    } else {
                        show_by_priority(ready_queue, current_process, output_file);
                        continue;
                    }

                }

            } else {

                if(ready_queue.empty()) {

                    if (current_process == 0) {
                        system_time = processes.back() -> arrival_time;
                        continue;
                    } else {
                        system_time += (current_process -> instruction_times[current_process -> current_instruction_index]);
                        current_process -> current_instruction_index++;
                        if (current_process -> current_instruction_index == current_process -> maximum_instruction_index){
                            current_process -> leaving_time = system_time;
                            current_process = 0;
                            continue;
                        } else {
                            continue;
                        }
                    }

                } else {

                    if (current_process == 0) {
                        current_process = ready_queue.front();
                        ready_queue.pop_front();
                        show_by_priority(ready_queue, current_process, output_file);
                        continue;
                    } else {
                        system_time += current_process -> instruction_times[current_process -> current_instruction_index];
                        current_process -> current_instruction_index++;
                        if (current_process -> current_instruction_index == current_process -> maximum_instruction_index){
                            current_process -> leaving_time = system_time;
                            current_process = 0;
                            continue;
                        } else {
                            continue;
                        }
                    }
                }
            }
        }
    }

    cout << "\n";
    output_file << "\n";

    for (vector<process*>::iterator it = backup_processes.begin() ; it != backup_processes.end(); ++it) {
        cout <<  "Turnaround time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time << " ms\n" ;
        output_file <<  "Turnaround time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time << " ms\n" ;
        cout <<  "Waiting time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time - (*it) -> execution_time << " ms\n" ;
        output_file <<  "Waiting time for " << (*it) -> name << " = " << (*it) -> leaving_time - (*it) -> arrival_time - (*it) -> execution_time << " ms\n" ;
    }

    output_file.close();

    return 0;

}

void show_by_priority(list<process*>& ready_queue, process* current_process, ofstream& output_file){

    cout << system_time << ":";
    output_file << system_time << ":";
    cout << "HEAD" << "-";
    output_file << "HEAD" << "-";

    if(current_process != 0) {
        cout << current_process -> name << "[" << current_process -> current_instruction_index + 1 << "]" << "-";
        output_file << current_process -> name << "[" << current_process -> current_instruction_index + 1 << "]" << "-";
    } else {
        if(ready_queue.empty()) {
            cout << "-";
            output_file << "-";
        }
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
