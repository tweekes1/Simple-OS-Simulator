/**
 * Main OS class that controls all other parts of the system
 * through encapsulation.
 */

#ifndef OPERATING_SYSTEM_H
#define OPERATING_SYSTEM_H

#include <iostream>
#include <list>
#include <memory>

#include "disks.h"
#include "process.h"
#include "memory_table.h"

using namespace std;

class OperatingSystem {
    public: 

        /**
         * Default constructor initializes everything to the default value of
         * 4GB for memory_size_ and 1GB for page_size_ and 2 for the disk_count_
         * 
         */
        OperatingSystem() : 
            memory_size_{4000000000}, page_size_{1000000000}, 
            disk_count_{2}, process_count_{1}
            { 
                for (int i = 0; i < disk_count_; i++) {
                    disks_.push_back(new Disk());
                }
            } 

        /**
         * @memory_size: the amount of memory that will be available to the OS (in bytes)
         * @page_size: the size of the memory page.
         * @disk_count: the amount of disks the system has.
         * @num_frames_: is the number of frames that the OS has. Value is calculated within the 
         * constructor.
         * 
         */
        OperatingSystem(long memory_size, int page_size, int disk_count) : 
            memory_size_{memory_size}, page_size_{page_size}, 
            disk_count_{disk_count}, process_count_{1}
            {
                num_frames_ = memory_size_ / page_size;
                memory_table_ = new MemoryTable(num_frames_);

                for (int i = 0; i < disk_count; i++) {
                    disks_.push_back(new Disk());
                }                
            }

        /**
         * Function that is called whenever the user needs help on 
         * how to use the OS.
         */

        void help() {
            cout << "\n=================================HELP================================" << endl;
            cout << "'A' to generate process" << endl;
            cout << "'Q' to preempt current running process" << endl;
            cout << "'fork' to generate child process for current running proc" << endl;
            cout << "'exit' to end execution of current running process" << endl;
            cout << "'wait' to pause execution for current running process" << endl;
            cout << "'d <disk_num> <filename>' to have current proc use disk<disk_num>" << endl;
            cout << "'D <disk_num>' to finish work on disk <disk_num>" << endl;
            cout << "'m <address>' to have current process use memory at <address>" << endl;
            cout << "'S r' to display a snapshot of the ready queue" << endl;
            cout << "'S i' to display a snapshot of the disks" << endl;
            cout << "'S m' to display a snapshow of the memory table" << endl;
            cout << "'help' to display this message agains" << endl;
            cout << "'bsod' to end the simulation" << endl;
            cout << "=====================================================================\n" << endl;
        }

        /**
         * Increments the @process_coint_ and then pushes back 
         * a new process to the ready queue. 
         * 
         * Function used for the 'A' command.
         */
        void spawnProcess() {
            process_count_++;
            ready_queue_.push_back(new Process(process_count_, 1));
        }

        /**
         * The current running process (front of @ready_queue_) has a new process 
         * a new process generated.
         * 
         * A pointer to the running process (@parent_proc) is created. Then @process_count_
         * is incremented and @parent_proc generates a child process see more in "process.h"
         * Then add a new process to the back of the @ready_queue_. 
         * 
         * Function used for the 'fork' command.
         */
        void forkProcess() {
            if (ready_queue_.empty()) {
                cout << "COMMAND FAILED: CPU IS IDLE" << endl;
                cout << "Enter Different Command\n" << endl;
                return;
            }

            Process* parent_proc = ready_queue_.front();
            
            process_count_++;
            parent_proc->spawnChildProc(process_count_);
            ready_queue_.push_back(new Process(process_count_, parent_proc->getPID()));

            parent_proc = nullptr;  
        }

        /**
         * The current running process (front of the @ready_queue_) is popped
         * from the front and then pushed to the back of the @ready_queue_.
         * 
         * Function used for the 'Q' command.
         */
        void preemptProcess() {
            Process* running_process = ready_queue_.front();

            ready_queue_.pop_front();
            ready_queue_.push_back(running_process);

            running_process = nullptr;
        }

        /**
         * Function will tell the user that the CPU is idle if there is
         * no new processes using or waiting for the CPU. If there are then 
         * the recursive private fucntion terminateProcess(...) is called. 
         * 
         * Function used for the 'exit' command.
         */
        void exitProcess() {
            if (ready_queue_.empty()) {
                cout << "COMMAND FAILED: CPU IS IDLE" << endl;
                cout << "Enter Different Command\n" << endl;
                return; 
            }

            terminateProcess(ready_queue_.front());
        }


        /**
         * This function will tell the user that the command failed 
         * if there is no processes using or waiting for the CPU OR 
         * if the process has no child to wait for.
         * 
         * If a process has zombie children do not send it to the
         * @waiting_for_child_ vector but instead it remains using the CPU 
         * since one of its children has finished execution. 
         * 
         * Function used for 'wait' command.
         */

        void waitProcess() {
            if (ready_queue_.empty()) {
                cout << "COMMAND FAILED: CPU IS IDLE" << endl;
                cout << "Enter Different Command\n" << endl;
                return; 
            }

            if (!ready_queue_.front()->hasChildren() && !ready_queue_.front()->hasZombies()) {
                cout << "COMMAND FAILED: " << ready_queue_.front() << " HAS NO CHILDREN" << endl;
                cout << "Enter Different Command\n" << endl;
                return; 
            }

            if (ready_queue_.front()->hasZombies()) {
                ready_queue_.front()->useZombie();
            } else {
                Process* temp = ready_queue_.front();
                ready_queue_.pop_front();
                waiting_for_child_.push_back(temp);
            }
        }

        /**
         * @logical_address: the "logical_address" that the current running process wants to use.
         * 
         * Function creates an entry into the @memory_table_ using the page number calculated from
         * @logical_address / @page_size_. 
         * 
         * Function used for 'm <address>' command.
         */
        void useMemory(int logical_address) {
            if (ready_queue_.empty()) {
                cout << "COMMAND FAILED: CPU IS IDLE" << endl;
                cout << "Enter Different Command\n" << endl;
                return;
            }

            int page_num = logical_address / page_size_;
            memory_table_->insertFrame(page_num, ready_queue_.front()->getPID());
        }

        /**
         * @disk_number: The disk that will be used accessed. 
         * @filename: name of the file that process will use on the disk.
         * 
         * If the user enters an invalid number (greater than @disk_count_ or negative) 
         * report to the user and return to the caller. Otherwise create a new into that
         * disk's queue. 
         * 
         * Function used for the command 'd <disk_number> <filename>'
         */
        void useDisk(int disk_number, string filename) {
            if (ready_queue_.empty()) {
                cout << "COMMAND FAILED: CPU IS IDLE" << endl;
                cout << "Enter Different Command\n" << endl;
                return;
            }

            if (disk_number < 0 || disk_number >= disk_count_) {
                cout << "ERROR: Disk " << disk_number << " does not exist" << endl;
                cout << endl;
            } else {
                disks_[disk_number]->addEntry(ready_queue_.front(), filename);
                ready_queue_.pop_front();
            }
        }

        /**
         * @disk_number: the number of the disk that has finished it works.
         * 
         * This function calls finishUsage() function of the Disk class. Which returns
         * the pointer to the process that was using the disk and send it to the 
         * end of the @ready_queue. (see disks.h for more info.) 
         * 
         * Function used for 'D <disknumber>' command
         */
        void finishDiskUsage(int disk_number) {
            if (disk_number < 0 || disk_number >= disk_count_) {
                cout << "ERROR: Disk " << disk_number << " does not exist" << endl;
                cout << endl;
            } else {
                ready_queue_.push_back(disks_[disk_number]->finishUsage());
            }
        }

        /**
         * Will display the currently running process along with all 
         * processes waiting in the @ready_queue_. If there are no 
         * processes running (@ready_queue is empty) it reports that the 
         * CPU is idle. Otherwise there the process using the CPU and the 
         * processes in the @ready_queue_ are also displayed.
         * 
         * Function used for 'S r' command.
         */
        void snapshotCPU() {
            if (ready_queue_.empty()) {
                cout << "CPU: IDLE" << endl;
            } else {
                cout << "CPU: " << *(ready_queue_.front()) << endl;

                if (ready_queue_.size() == 1) {
                    cout << "Ready Queue: EMPTY" << endl;
                }

                if (ready_queue_.size() > 1) {
                    cout << "Ready-Queue: " << endl;
                    
                   for (auto &process : ready_queue_) {
                       if (process == ready_queue_.front()) { continue; }
                        cout << "\t    " << *process << endl;
                    }
                }
            } 

            if (waiting_for_child_.size() > 0) {
                cout << "Waiting-for-child:" << endl;
                for (auto &process : waiting_for_child_) {
                    cout << "\t    " << *process << endl;
                }
            }

            cout << endl;
        }

        /**
         * A publicly exposed version of @class: MemoryTable 
         * snapshotMemory() (see MemoryTable for more info). 
         * 
         * Function used for 'S m' command.
         */
        void snapshotMemory() {
            memory_table_->snapshotMemory();
        }

        /**
         * A publicly facing method that will show the staus of all the disks of the
         * OS.
         * 
         * Function used for 'S i' command.
         */
        void snapshotIO() {
            for (int i = 0; i < disk_count_; i++) {
                cout << "Disk " << i << ": ";
                disks_[i]->showEntries();
            }
        }
  
    private: 
        /**
         * @memory_size_: the amount of available memory the system has
         * @page_size_: the size of each memory page.
         * @disk_count_: Number of disks available to the systems.
         * @process_count_: the count that will be used to make new PIDs,
         * also ensures that each ID is unique.
         * @ready_queue_: the Operating System's ready queue.
         * @waiting_for_child_: vector that holds all processes that 
         * are waiting on their children to finish execution.
         * @disk_: a vector of all the disks that the operating system has.
         * @num_frames: The number of frames the OS has.
         */
        long memory_size_;
        int page_size_;
        int disk_count_;
        int process_count_;
        list<Process*> ready_queue_;
        vector<Process*> waiting_for_child_;
        vector<Disk*> disks_;
        MemoryTable* memory_table_;
        int num_frames_;

        /**
         * @pid: the pid of the process to be found within the @ready_queue. 
         * 
         * Iterate through the @ready_queue and if a Process has the same pid
         * as the one passed as a parameter fucntion returns the pointer to that Process.
         * 
         * If the Process is not in the @ready_queue_ then search the @disks_ and if the process
         * is not there then search the @waiting_for_children_ vector.
         */
        Process* findProcess(int pid) {
            for (auto &process : ready_queue_) {
                if (process->getPID() == pid) {
                    return process;
                }
            }

            for (auto &disk : disks_) {
                if (disk->seachDisks(pid) != nullptr) {
                    return disk->seachDisks(pid);
                }
            }

            for (auto &process : waiting_for_child_) {
                if (process->getPID() == pid) {
                    return process;
                }
            }            

            return nullptr;
        }

        /**
         * @pid: The process whose iterator that need to be found.
         * 
         * Similar to function findProcess(...) but instead the function
         * will return the iterator to the function. This is used to help 
         * remove items from the @ready_queue_ since stl_list items can 
         * only be erased via iterators.
         */
        list<Process*>::iterator findProcessItr(int pid) {
            for (list<Process*>::iterator itr = ready_queue_.begin(); 
                 itr != ready_queue_.end(); ++itr) {
                     if ((*itr)->getPID() == pid) {
                         return itr;
                     }
               }    

            return ready_queue_.end();
        }

        /**
         * @terminated: A pointer to the process whose execution will end.
         * 
         * First check to see if @terminated has children. If there are children
         * we signal to the children that the parent is dead and then terminate
         * the children processes. (cascading termination)
         * 
         * Then release all the resources that @terminated may have been using. 
         * Memory or disk usage. 
         * 
         * Then a check to see if @terminated's parent is not dead (if you terminate
         * a child process but the parent is still alive.) If the parent is still alive
         * check to see if it is waiting on its child to end termination. If true "wakeup"
         * the parent (send it back to the @ready_queue_) otherwise the parent loses a child and
         * gains a zombie. 
         * 
         * Finally if the @terminated is in the @ready_queue_ remove it from the @ready_queue.
         * 
         */

        void terminateProcess(Process* terminated) {
            if (terminated->hasChildren()) {
                for (int pid : terminated->getChildren()) {
                    findProcess(pid)->signalParentDeath();
                    terminateProcess(findProcess(pid));
                }
            }

            memory_table_->releaseResources(terminated->getPID());
            for (auto &disk : disks_) {
                disk->releaseResources(terminated->getPID());
            }

            if (!terminated->parentDead()) {
                if (waitingForChild(terminated->getParentPID())) {
                    wakeupParent(terminated->getParentPID(), terminated->getPID());
                } else {
                    findProcess(terminated->getParentPID())->addZombie();
                    findProcess(terminated->getParentPID())->removeChild(terminated->getPID());
                }
            }

            if (inReadyQueue(terminated->getPID())) {
                ready_queue_.erase(findProcessItr(terminated->getPID()));
            }
        }

        /** 
         * @parent_pid: pid of process we are looking for.
         * 
         * Searches the @waiting_for_child_ vector for a process with a matching PID.
         */
        bool waitingForChild(int parent_pid) {
            for (auto &parent : waiting_for_child_) {
                if (parent->getPID() == parent_pid) {
                    return true;
                }
            }

            return false;
        }

        /**
         * @parent_pid: the pid of the parent that needs to wake up.
         * @child_pid: the pid of the child who is waking up the parent.
         * 
         * Iterate through the @waiting_for_child_ vector using an iterator. Once 
         * the parent process is found the function pushes that back to the @ready_queue_
         * remove the child from the parent process' @children_ vector, and finally removes
         * the @waiting_for_children_ vector entry.
         */
        void wakeupParent(int parent_pid, int child_pid) {
            for (vector<Process*>::iterator itr = waiting_for_child_.begin(); 
                itr != waiting_for_child_.end(); ++itr) {
                    if ((*itr)->getPID() == parent_pid) {
                        ready_queue_.push_back(*itr);
                        
                        (*itr)->removeChild(child_pid);
                        waiting_for_child_.erase(itr);

                        return;
                     }
                 }
        }

        /**
         * @pid: pid of the process we want to find in memory.
         * 
         * Iterate through @ready_queue_ if found then return true 
         * otherwise return false.
         */
        bool inReadyQueue(int pid) {
            for (auto &process : ready_queue_) {
                if (process->getPID() == pid) {
                    return true;
                }
            }

            return false;
        }

        /**
         * @pid: pid of the process we want to find in memory.
         * 
         * Iterate through @waiting_for_child_ if found then return true 
         * otherwise return false.
         */
        bool inWaitingVector(int pid) {
            for (auto &process : waiting_for_child_) {
                if (process->getPID() == pid) {
                    return true;
                }
            }

            return false;
        }
};

#endif