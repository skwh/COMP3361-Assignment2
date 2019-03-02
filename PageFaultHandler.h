/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PageFaultHandler.h
 * Author: jason
 *
 * Created on March 1, 2019, 2:11 PM
 */

#ifndef PAGEFAULTHANDLER_H
#define PAGEFAULTHANDLER_H

#include <MMU.h>
#include <iostream>

class PageFaultHandler : public mem::MMU::FaultHandler {
    public:
        bool Run(const mem::PMCB& pmcb) {
            std::string fault_type;
            switch (pmcb.operation_state) {
                case mem::PMCB::READ_OP:
                    fault_type = "Read";
                    break;
                case mem::PMCB::WRITE_OP:
                    fault_type = "Write";
                    break;
                default:
                    fault_type = "NONE";
            }
            std::cout << fault_type << " Page Fault at address " << std::hex << pmcb.next_vaddress << "\n";
            return false;
        }
};

#endif /* PAGEFAULTHANDLER_H */

