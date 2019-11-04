#ifndef DISKS_H
#define DISKS_H

#include <iostream>
#include <list>
#include <string>

#include "process.h"

/**
 * Struct to hold the process and filename.
 */
struct DiskEntry {
    Process* process_;
    string filename_;

    DiskEntry() : process_{nullptr}, filename_{""} 
        { }
    
    DiskEntry(Process* process, string filename) : 
        process_{process}, filename_{filename}
        { }      
};

/**
 * Class that will be used to simulate the disk usage for our operating system.
 */
class Disk {
    public:
        /**
         * Default constuctor
         */
        Disk() { } 

        /**
         * Destructor that removes all the memory for all of the entries.
         */
        ~Disk() {
            for (auto &entry : queue_) {
                delete entry;
                entry = nullptr;
            }
        }
        
        /**
         * @process: process that will be using the disk.
         * @filename: the name of the file that the process wants to use.
         * 
         * Creates a new disk entry and then pushes it to the @queue_.
         */
        void addEntry(Process* process, string filename) {
            DiskEntry* toBeAdded = new DiskEntry{process, filename};
            queue_.push_back(toBeAdded);
        }

        /**
         * Simple function the outputs the state of the @queue_ for the disk.
         * if there is nothing being used by the disk then function outputs IDLE.
         * Otherwise it shows the current process using the disk and those waiting
         * to use the disk.
         */
        void showEntries() {
            if (queue_.empty()) {
                cout << "IDLE" << endl;
            } else {
                cout << *queue_.front()->process_ << " ";
                cout << queue_.front()->filename_ << endl;

                if (queue_.size() > 1) {
                    cout << "Waiting:" << endl;
                    for (auto &entry : queue_) {
                        if (entry == queue_.front()) { continue; }
                        
                        cout << "\t " << *entry->process_ << " ";
                        cout << entry->filename_ << endl;
                    }
                }
            }
            cout << endl;
        }

        Process* seachDisks(int pid) {
            for (auto &entry : queue_) {
                if (entry->process_->getPID() == pid) {
                    return entry->process_;
                }
            }

            return nullptr;
        }

        /**
         * The disk is finished being used and the process using the disk 
         * will be returned to the @ready_queue
         */
        Process* finishUsage() {
            DiskEntry* temp = queue_.front();

            queue_.pop_front();

            return temp->process_;          
        }
        /**
         * If a process has been killed prematurely. This function goes
         * through processes using the disk and if the @pid matches 
         * it is removed from the @queue_.
         */
        void releaseResources(int pid) {
            for (list<DiskEntry*>::iterator itr = queue_.begin(); 
                 itr != queue_.end(); ++itr) {
                     if ( (*itr)->process_->getPID() == pid) {
                         queue_.erase(itr);
                         return;
                     }
                 }
        }


    private:
        /**
         * @queue_: the queue for the disk.
         */
        list<DiskEntry*> queue_;

};

#endif