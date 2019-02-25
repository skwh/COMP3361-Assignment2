/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: evan
 *
 * Created on February 16, 2019, 2:18 PM
 */

#include "Process.h"

#include <cstdlib>
#include <iostream>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " file_name " << std::endl;
        return 1;
    }
    
    mem::MMU memory(128);
    FrameAllocator allocator(128);
    Process trace(argv[0]);
    trace.Exec();
    
    return 0;
}

