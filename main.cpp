#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>

using namespace std;

typedef struct {

    string name;
    string instruction_file;
    long int priority;
    long int arrival_time;
    long int leaving_time;
    vector<string> instruction_names;
    vector<long int> instruction_times;
    vector<long int>::const_iterator instruction_iterator;

} process;

int main(int argc, char *argv[])
{

    vector<process> processes;

    ifstream definition_file(argv[1]);

    if (definition_file.is_open()){

        string line;

        while(getline(definition_file, line)){

            process new_process;

            int elementCounter = 0;

            istringstream iss(line);

            for(string element; iss >> element; ){

                switch(elementCounter++) {

                    case 0:
                        new_process.name = element;
                        break;
                    case 1:
                        new_process.priority = atol(element.c_str());
                        break;
                    case 2:
                        new_process.instruction_file = element;
                        break;
                    case 3:
                        new_process.arrival_time = atol(element.c_str());
                        break;
                    default:
                        break;
                }

            }

            processes.push_back(new_process);

        }

        definition_file.close();

    } else
        cout << "Unable to open file";

    /**
    cout << processes.size() << "\n";
    for (vector<process>::iterator it = processes.begin() ; it != processes.end(); ++it)
        cout << it -> name << " " << it -> priority << " " << it -> instruction_file << " " << it -> arrival_time << "\n" ;
    **/

    for (vector<process>::iterator it = processes.begin() ; it != processes.end(); ++it) {

        string filename_with_extension = it -> instruction_file + ".txt";

        char cstr[filename_with_extension.size() + 1];
        strcpy(cstr, filename_with_extension.c_str());	// or pass &s[0]

        ifstream code_file(cstr);

        if (code_file.is_open()){

            string line;

            while(getline(code_file, line)){

                istringstream iss(line);

                int elementCounter = 0;

                for(string element; iss >> element; ){

                    switch(elementCounter++) {

                        case 0:
                            it -> instruction_names.push_back(element);
                            break;
                        case 1:
                            it -> instruction_times.push_back(atol(element.c_str()));
                            break;
                        default:
                            break;
                    }

                }

                it -> instruction_iterator = it -> instruction_times.begin();
                it -> leaving_time = -1;

            }

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







    return 0;

}
