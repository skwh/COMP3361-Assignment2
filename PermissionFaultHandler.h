/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PermissionFaultHandler.h
 * Author: jason
 *
 * Created on March 1, 2019, 2:30 PM
 */

#ifndef PERMISSIONFAULTHANDLER_H
#define PERMISSIONFAULTHANDLER_H

#include <iostream>
#include <MMU.h>

class PermissionFaultHandler : public mem::MMU::FaultHandler {
    public:
        bool Run(const mem::PMCB& pmcb) {
            std::cout << "Write Permission Fault at address " << pmcb.next_vaddress << "\n";
            return false;
        }
};


#endif /* PERMISSIONFAULTHANDLER_H */

