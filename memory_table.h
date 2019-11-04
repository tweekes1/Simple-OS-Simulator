/**
 * Header file for the MemoryTable and MemoryFrame class
 * that is responsible for handling all the memory aspects of this project.
 */

#ifndef MEMORY_TABLE_H
#define MEMORY_TABLE_H

#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

/**
 * Struct that will be used to conceptualize the entries
 * in the MemoryTable class.
 */

struct MemoryFrame {
    int page_num_;
    int process_id_;
    int timestamp_;
    bool active_;

    MemoryFrame() : 
        page_num_{0}, process_id_{0}, timestamp_{0}, active_{false}
        { }

    MemoryFrame(int page_num, int process_id, int timestamp) : 
        page_num_{page_num}, process_id_{process_id}, timestamp_{timestamp}, active_{true} 
        { }
};

/** 
 * MemoryFrame operator overloads. That are needed for various functions in this class.
 */

bool operator== (MemoryFrame& lhs, MemoryFrame& rhs) {
    bool same_page = lhs.page_num_ == rhs.page_num_;
    bool same_pid = lhs.process_id_ == rhs.process_id_;

    return (same_page && same_pid);
}

bool operator < (MemoryFrame& lhs, MemoryFrame& rhs) {
    if (!lhs.active_ || !rhs.active_) {
        return false;
    } 

    return lhs.timestamp_ < rhs.timestamp_;
}


class MemoryTable {
    public: 

        /**
         * Default Constructor. Won't ever be used.
         */
        MemoryTable() : frame_limit_{0}, active_count_{0}, memory_time_{0}
            { }

        /**
         * @table_size: The number of entries that will be allowed into the table.
         * 
         * Initialize the frames_ vector to all inactive empty frames.
         */
        MemoryTable(int table_size) : frame_limit_{table_size}, active_count_{0}, memory_time_{0}
            {
                for (int i = 0; i < frame_limit_; i++) {
                    frames_.push_back(new MemoryFrame());
                }
            }
        /**
         * Destructor that clears all the pointers within the vector.
         */
        ~MemoryTable() {
            for (auto &frame : frames_) {
                delete frame;
                frame = nullptr;
            }
        }

        /**
         * @page_num: value of the page number that the process want to load into memeory.
         * @pid: process identifier.
         * @timestamp: "time" memory is accessed.
         * 
         * A new MemoryFrame object is created that which may be added to the table if a 
         * criteria is met.
         * 
         * First we check to see if newFrame is already in the table (the pid and page# are
         * the same.) If true we override the entry in the table with newFrame. See @frameInMemory(...)
         * 
         * If the table is full we find the oldest entry in the table and override it with newFrame
         * object.
         * 
         * If both fail then we find the first available frame and insert our newFrame into the table.
         * 
         */
        void insertFrame(int page_num, int pid) {
            memory_time_++;
            MemoryFrame* newFrame = new MemoryFrame(page_num, pid, memory_time_);

            if (frameInMemory(newFrame)) { return; }
            else if (active_count_ == frame_limit_) {
                 MemoryFrame* temp = findOldestFrame();

                 temp->page_num_ = page_num;
                 temp->process_id_ = pid;
                 temp->timestamp_ = memory_time_;
                 
                 return;
            } else {
                for (auto &frame : frames_) {
                    if (!frame->active_) {
                        frame = newFrame;
                        active_count_++;
                        return;
                    }
                }
            }
        }

        /**
         * Prints all the active MemoryFrames. If a frame is inactive, its entry is
         * skipped and not printed.
         */

        void snapshotMemory() {
            cout << left << setw(10) << setfill(' ') << "Frame#";
            cout << left << setw(10) << setfill(' ') << "Page#";
            cout << left << setw(7) << setfill(' ') << "PID";
            cout << left << setw(10) << setfill(' ') << "Timestamp" << endl;  
            cout << left << setw(37) << setfill('=') << "=" << endl;

            for (int i = 0; i < frame_limit_; i++) {
                if (!frames_[i]->active_) { continue; }

                cout << left << "  " << setw(9) << setfill(' ') << i;
                cout << left << setw(10) << setfill(' ') << frames_[i]->page_num_;
                cout << left << setw(6) << setfill(' ') << frames_[i]->process_id_;
                cout << right << setw(5) << setfill(' ') << frames_[i]->timestamp_ << endl;  
            }

            cout << endl;
        }

        /**
         * @pid: the pid of the function whose resources we are releasing.
         * 
         * Iterate through the entire table and erase all table entries that
         * have same pid.
         */
        void releaseResources(int pid) {
            for (int i = 0; i < frame_limit_; i++) {
                if (frames_[i]->process_id_ == pid) {
                    frames_[i]->active_ = false;
                    active_count_--;
                }
            }
        }

    private: 

        /**
         * @frames_: vector that will hold all of the MemoryFrames.
         * @frame_limit_: the total size of the table
         * @active_count_: the number of entries used in the table.
         * @memory_time: Number that will be incremented everytime a frame is
         * inserted. Will be used for the timestape of the table.
         */
        vector<MemoryFrame*> frames_;
        int frame_limit_;
        int active_count_;
        int memory_time_;

        /**
         * A new MemoryFrame pointer is assigned to the first entry in the table
         * Then iterate through the entire table to find the smallest timestamp in 
         * the table.
         */
        MemoryFrame* findOldestFrame() {
            MemoryFrame* temp = frames_[0];

            for (auto &frame : frames_) {
                if (frame->timestamp_ < temp->timestamp_) {
                    temp = frame;
                }
            }

            return temp;
        }


        /**
         * @frame: the MemoryFrame we wish to see is in memory.
         * 
         * Iterate through table and see if there is an entry equal
         * to @frame.
         */
        bool frameInMemory(MemoryFrame* frame) {
            for (auto &f : frames_) {
                if (*f == *frame && f->active_) {
                    f = frame;
                    return true;
                }
            }

            return false;
        }

};

#endif