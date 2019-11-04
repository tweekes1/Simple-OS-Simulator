/**
 * Class file for the main data structure used for this OS
 * simulator.
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <memory>
#include <vector>

using namespace std;

/**
 * Class for the main data structure that will simulate processes in this
 * OS simulator.
 */
class Process {
    public:

        /** 
         * Default constructor
         */
        Process() : 
            pid_{0}, parent_pid_{1}
            { }
        
        /**
         * @pid: the pid of the new process.
         * @parent_pid: the pid of the parent.
         */
        Process(int pid, int parent_pid) : 
            pid_{pid}, parent_pid_{parent_pid}
            { }

        /**
         * Returns the process's PID
         */
        int getPID() {
            return pid_;
        }

        /**
         * Returns the process's parent PID
         */
        int getParentPID() {
            return parent_pid_;
        }

        /**
         * returns @children_ vector.
         */
        vector<int>& getChildren() {
            return children_;
        }

        /**
         * Returns if the @available_zombies_ counter is
         * greater than 0.
         */
        bool hasZombies() {
            return available_zombies_ > 0;
        }

        /** 
         * Returns whether the @children_ vector is greater than 0.
         */
        bool hasChildren() {
            return children_.size() > 0;
        }
        
        /**
         * Increments the @available_zombies_ counter.
         */
        void addZombie() {
            available_zombies_++;
        }

        /**
         * Decrements the @available_zombies_ counter.
         */
        void useZombie() {
            available_zombies_--;
        }

        /**
         * @child_pid: the pid that will be removed from the @children_
         * vector.
         * 
         * Iterate through @children_ vector and removes the entry that matches
         * @pid.
         */
        void removeChild(int child_pid) {
            for (vector<int>::iterator itr = children_.begin(); 
                 itr != children_.end(); ++itr) {
                     if ((*itr) == child_pid) {
                         children_.erase(itr);
                         return;
                     }
                 }
        }

        /**
         * @child_pid: pid that will be used inserted to the @children_ vector.
         */
        void spawnChildProc(int child_pid) {
            children_.push_back(child_pid);
        }

        void signalParentDeath() {
            parent_dead_ = true;
        }

        bool parentDead() {
            if (parent_pid_ == 1) {
                return true;
            }

            return parent_dead_;
        }

        /** 
         * @out: output stream.
         * @process: process that will be printed.
         * 
         * Overloaded output operator so the processes can be printed.
         */
        friend ostream& operator<< (ostream& out, Process &process) {
            out << "Process " << process.pid_;
            
            return out;
        }

        /**
         * @rhs: right hand process
         * @lhs: left hand process
         * 
         * Overloaded equality operator. If both PIDs are the same
         * then processes are the same.
         */
        friend bool operator== (Process rhs, Process lhs) {
            return rhs.pid_ == lhs.pid_;
        }

    private:
        /**
         * @pid_: pid of the process
         * @parent_pid_: pid of the parent.
         * @available_zombies_: counter to kepe track of the process's
         * abailable number of zombies.
         * @children_: vector that holds all the pid of this process's children.
         */
        int pid_;
        int parent_pid_;
        int available_zombies_;
        bool parent_dead_;
        vector<int> children_;
};


#endif