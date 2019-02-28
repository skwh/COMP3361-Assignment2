/*
 * Process implementation 
 * 
 * File:   Process.cpp
 * Author: Mike Goss <mikegoss@cs.du.edu>
 * COMP3361 Winter 2019 Lab 4 Sample Solution
 */

#include "Process.h"
#include "FrameAllocator.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

using mem::Addr;

using std::cin;
using std::cout;
using std::cerr;
using std::getline;
using std::istringstream;
using std::string;
using std::vector;

Process::Process(std::string file_name_, mem::MMU* mmu, FrameAllocator* allocator) 
: file_name(file_name_), line_number(0) {
  // Open the trace file.  Abort program if can't open.
  trace.open(file_name, std::ios_base::in);
  if (!trace.is_open()) {
    cerr << "ERROR: failed to open trace file: " << file_name << "\n";
    exit(2);
  }
  
  memory = mmu;
  alloc = allocator;
}

Process::~Process() {
  trace.close();
}

void Process::Exec(void) {
  // Read and process commands
  std::string line;                // text line read
  std::string cmd;                 // command from line
  vector<uint32_t> cmdArgs;   // arguments from line
  
  // Select the command to execute
  while (ParseCommand(line, cmd, cmdArgs)) {
    if (cmd == "alloc" ) {
      CmdAlloc(line, cmd, cmdArgs);    // allocate memory
    } else if (cmd == "cmp") {
      CmdCmp(line, cmd, cmdArgs);        // get and compare multiple bytes
    } else if (cmd == "set") {
      CmdSet(line, cmd, cmdArgs);        // put bytes
    } else if (cmd == "fill") {
      CmdFill(line, cmd, cmdArgs);       // fill bytes with value
    } else if (cmd == "dup") {
      CmdDup(line, cmd, cmdArgs);        // duplicate bytes to dest from source
    } else if (cmd == "print") {
      CmdPrint(line, cmd, cmdArgs);      // dump byte values to output
    } else if (cmd != "*") {
      cerr << "ERROR: invalid command\n";
      exit(2);
    }
  }
}

bool Process::ParseCommand(
    std::string &line, std::string &cmd, vector<uint32_t> &cmdArgs) {
  cmdArgs.clear();
  line.clear();
  
  // Read next line
  if (std::getline(trace, line)) {
    ++line_number;
    cout << std::dec << line_number << ":" << line << "\n";
    
    // No further processing if comment or empty line
    if (line.size() == 0 || line[0] == '*') {
      cmd = "*";
      return true;
    }
    
    // Make a string stream from command line
    istringstream lineStream(line);
    
    // Get address
    uint32_t addr = 0;
    if (!(lineStream >> std::hex >> addr)) {
      if (lineStream.eof()) {
        // Blank line, treat as comment
        cmd = "*";
        return true;
      } else {
        cerr << "ERROR: badly formed address in trace file: "
                << file_name << " at line " << line_number << "\n";
        exit(2);
      }
    }
    cmdArgs.push_back(addr);
    
    // Get command
    if (!(lineStream >> cmd)) {
      cerr << "ERROR: no command name following address in trace file: "
              << file_name << " at line " << line_number << "\n";
      exit(2);
    }
    
    // Get any additional arguments
    Addr arg;
    while (lineStream >> std::hex >> arg) {
      cmdArgs.push_back(arg);
    }
    return true;
  } else if (trace.eof()) {
      return false;
  } else {
    cerr << "ERROR: getline failed on trace file: " << file_name 
            << " at line " << line_number << "\n";
    exit(2);
  }
}

void Process::CmdAlloc(const std::string& line, 
                       const std::string& cmd, 
                       const std::vector<uint32_t>& cmdArgs) {
    Addr vaddr = cmdArgs.at(0);
    uint32_t count = cmdArgs.at(1);
    if (vaddr % alloc->kPageSize === 0) {
        // TODO: allocate pages
    } else {
        cerr << "ERROR: alloc vaddr " << vaddr << " is not a multiple of the page size " << alloc->kPageSize << "\n";
        exit(2);
    }
}
/*
void Process::CmdMemsize(const std::string &line,
                         const std::string &cmd,
                         const vector<uint32_t> &cmdArgs) {
  if (cmdArgs.size() == 1) {
    // Round up to next multiple of page size
    Addr pages = (cmdArgs.at(0) + mem::kPageSize - 1) / mem::kPageSize;
    // Allocate the specified memory size
    memory = std::make_unique<mem::MMU>(pages);
  } else {
    cerr << "ERROR: badly formatted memsize command\n";
    exit(2);
  }
}
*/

void Process::CmdCmp(const std::string &line,
                     const std::string &cmd,
                     const vector<uint32_t> &cmdArgs) {
  if (cmdArgs.size() != 3) {
    cerr << "ERROR: badly formatted cmp command\n";
    exit(2);
  }
  Addr addr1 = cmdArgs.at(0);
  Addr addr2 = cmdArgs.at(1);
  uint32_t count = cmdArgs.at(2);

  // Compare specified byte values
  for (uint32_t i = 0; i < count; ++i) {
    Addr a1 = addr1 + i;
    uint8_t v1 = 0;
    memory->movb(&v1, a1);
    Addr a2 = addr2 + i;
    uint8_t v2 = 0;
    memory->movb(&v2, a2);
    if(v1 != v2) {
      cout << std::setfill('0') << std::hex
              << "cmp error"
              << ", addr1 = "  << std::setw(7) << a1
              << ", value = " << std::setw(2) << static_cast<uint32_t>(v1)
              << ", addr2 = "  << std::setw(7) << a2
              << ", value = " << std::setw(2) << static_cast<uint32_t>(v2) << "\n";
    }
  }
}

void Process::CmdSet(const std::string &line,
                     const std::string &cmd,
                     const vector<uint32_t> &cmdArgs) {
  // Store multiple bytes starting at specified address
  Addr addr = cmdArgs.at(0);
  for (int i = 1; i < cmdArgs.size(); ++i) {
    uint8_t b = cmdArgs.at(i);
    memory->movb(addr++, &b);
  }
}

void Process::CmdDup(const std::string &line,
                     const std::string &cmd,
                     const vector<uint32_t> &cmdArgs) {
  if (cmdArgs.size() != 3) {
    cerr << "ERROR: badly formatted dup command\n";
    exit(2);
  }
  
  // Copy specified number of bytes to destination from source
  Addr dst = cmdArgs.at(1);
  Addr src = cmdArgs.at(0);
  uint32_t count = cmdArgs.at(2);
  
  // Buffer for copy (copy a block at a time for efficiency)
  uint8_t buffer[1024];
  while (count > 0) {
    uint32_t block_size = std::min(count, sizeof(buffer));
    memory->movb(buffer, src, block_size);
    src += block_size;
    memory->movb(dst, buffer, block_size);
    dst += block_size;
    count -= block_size;
  }
}

void Process::CmdFill(const std::string &line,
                     const std::string &cmd,
                     const vector<uint32_t> &cmdArgs) {
  // Fill destination range with specified value
  uint8_t value = cmdArgs.at(1);
  uint32_t count = cmdArgs.at(2);
  Addr addr = cmdArgs.at(0);
  
  // Use buffer for efficiency
  uint8_t buffer[1024];
  memset(buffer, value, std::min(count, sizeof(buffer)));
  
  // Write data to memory
  while (count > 0) {
    uint32_t block_size = std::min(count, sizeof(buffer));
    memory->movb(addr, buffer, block_size);
    addr += block_size;
    count -= block_size;
  }
}

void Process::CmdPrint(const std::string &line,
                     const std::string &cmd,
                     const vector<uint32_t> &cmdArgs) {
  Addr addr = cmdArgs.at(0);
  uint32_t count = cmdArgs.at(1);

  // Output the specified number of bytes starting at the address
  for (int i = 0; i < count; ++i) {
    if ((i % 16) == 0) { // Write new line with address every 16 bytes
      if (i > 0) cout << "\n";  // not before first line
      cout << std::hex << std::setw(7) << std::setfill('0') << addr << ":";
    }
    uint8_t b;
    memory->movb(&b, addr++);
    cout << " " << std::setfill('0') << std::setw(2) << static_cast<uint32_t> (b);
  }
  cout << "\n";
}
