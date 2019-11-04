/**
 * Main execution code for simulator. First initial prompt assumes 
 * perfect input (ONLY NUMBERS). 
 * 
 * This driver program takes the commands from the standard input and if 
 * the first piece of the input matches a particular criteria that needs a
 * number i.e m <num> we take the second value place it into a stringstream
 * and then push that stringstream into an integer value. If the command receives
 * something that is not wanted the command is invalid and the user is informed.
 */
#include <iostream>
#include <string>
#include <sstream>

#include "helpers.h"
#include "operating_system.h"

using namespace std;

int main() {
    stringstream s_stream;
    long memory_{0};
    int page_{0}, disk_{0};

    while (true) {
        cout << "Enter Memory Amount: ";
        cin >> memory_;

        cout << "Enter Page Size: ";
        cin >> page_;

        cout << "Enter Number of Disks: ";
        cin >> disk_;

        break;
    }
    string user_input, first, second, third;

    OperatingSystem os{memory_, page_, disk_};

    os.help();

    while (first != "bsod") {
        cout << "> ";
        getline(cin, user_input);

        stringstream s(user_input), stream; 
        s >> first;

        if (first == "A") {
            os.spawnProcess();
        } else if (first == "Q") {
            os.preemptProcess();
        } else if (first == "fork") {
            os.forkProcess();
        } else if (first == "exit") {
            os.exitProcess();
        } else if (first == "wait") {
            os.waitProcess();
        } else if (first == "S") {
            s >> second;
            if (second == "i") {
                os.snapshotIO();
            } else if (second == "m") {
                os.snapshotMemory();
            } else if (second == "r") {
                os.snapshotCPU();
            } else {
                cout << "Invalid command" << endl;
            }
        } else if (first == "d") {
            s >> second;

            if (isNumber(second)) {
                stream << second;
                int disk_num;

                stream >> disk_num;
                s >> third;

                os.useDisk(disk_num, third);
            } else {
                cout << "Invalid command" << endl;
            }
        } else if (first == "D") {
            s >> second;

            if (isNumber(second)) {
                stream << second;
                int disk_num;
                
                stream >> disk_num;

                os.finishDiskUsage(disk_num);
            } else {
                cout << "Invalid command" << endl;
            }
        } else if (first == "m") {
            s >> second; 

            if (isNumber(second)) {
                stream << second;
                int mem_address;

                stream >> mem_address;

                os.useMemory(mem_address);
            } else if (first == "help") {
                os.help();
            } else {
                cout << "Invalid command" << endl;
            }
        }
    }
}