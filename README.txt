Running the program:
    To run this program run the following commands:
        make clean
        make all
        make runsimulator

    I modified a Makefile from a previous school assignment
    and credit the person who gave it to me, Professor Ioannis Stamos

Information about the program:
    The objective of this program is to simulate the 
    basic internals of an operating system.
    Every function in this project was written by me and is given
    a brief description of it does. The initial set up of the operating system
    (inputting the specs) assumes proper imput. If you enter any char that is
    not a number the program will end execution with a floating point error.
    
    ONLY INPUT NUMBERS WHEN FIRST RUNNING THE SIMULATOR.
    ALSO DO NOT MAKE THE PAGE SIZE GREATER THAN THE MEMORY SIZE.

Programe Structure:
    I have divided the project into 5 separate parts:
        process.h- Header file for the Process class code.
        
        memory_table.h- Header file for the MemoryTable class code 
        along with the MemoryFrame struct code.

        disks.h- Header file for the Disk class code and the DiskEntry
        struct code.

        operating_system.h- Header file for the OperatingSytem class code
        this is the file that pieces all the previous parts together to 
        create the OS simulator.

        simulator.cc- File that will actually run the OS simulator. Prompts user
        for OS specs and then runs the actual OS simulator.

        There is also a helpers.h file that contains 2 helper functions that
        simualtor.cc needs forinput validation.

Bugs:
    Not a critical bug but when you enter all the information for the operating system
    the prompt carrot ('>') is outputted twice on the first command entry. Does not affect
    the actual simulator just an weird bug.