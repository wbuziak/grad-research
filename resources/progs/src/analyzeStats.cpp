#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
using namespace std;

int main(int argc, char** argv) {
  ifstream fin;
  string line, subs, prev;
  bool verbose;

  if (argc != 2 && argc != 3 && argc != 4) { printf("USAGE:\n\n./bin/analyze stat-file\n./bin/analyze stat-file output-name.csv\n  Optional: -a flag for verbose cores\n"); exit(1); }

  verbose = false;
  if (argc == 3) {
    if (strcmp(argv[2], "-a") == 0) { verbose = true; }
  }
  if (argc == 4) {
    if (strcmp(argv[3], "-a") == 0) { verbose = true; }
  }

  fin.open(argv[1]);
  if (fin.is_open()) {
    printf("\nStat,          Value\n--------------------\n\n");
    while (getline(fin, line)) {     
      istringstream iss(line);

      while (iss >> subs) {
        // numCycles
        if (prev == "board.processor.start.core.numCycles") { printf("%s,     %s\n", prev.c_str(), subs.c_str()); }
        if (prev == "board.processor.switch.core.numCycles") { printf("%s,     %s\n", prev.c_str(), subs.c_str()); }
        if (prev == "board.processor.switch0.core.numCycles") { printf("%s,     %s\n", prev.c_str(), subs.c_str()); }
        if (verbose) {
          if (prev == "board.processor.switch1.core.numCycles") { printf("%s,     %s\n", prev.c_str(), subs.c_str()); }
          if (prev == "board.processor.switch2.core.numCycles") { printf("%s,     %s\n", prev.c_str(), subs.c_str()); }
          if (prev == "board.processor.switch3.core.numCycles") { printf("%s,     %s\n", prev.c_str(), subs.c_str()); }
        }

        // Instructions
        if (prev == "board.processor.start.core.commitStats0.numInsts") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.processor.switch0.core.commitStats0.numInsts") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.processor.switch.core.commitStats0.numInsts") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (verbose) {
          if (prev == "board.processor.switch3.core.commitStats0.numInsts") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
          if (prev == "board.processor.switch2.core.commitStats0.numInsts") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
          if (prev == "board.processor.switch1.core.commitStats0.numInsts") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        }

        // IPC
        if (prev == "board.processor.switch.core.commitStats0.ipc") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.processor.switch0.core.ipc") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (verbose) {
          if (prev == "board.processor.switch1.core.ipc") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
          if (prev == "board.processor.switch2.core.ipc") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
          if (prev == "board.processor.switch3.core.ipc") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        }

        // Accesses
        //l1
        if (prev == "board.cache_hierarchy.l1i-cache-0.oververboseAccesses::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.cache_hierarchy.l1d-cache-0.oververboseAccesses::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        // l2
        if (prev == "board.cache_hierarchy.l2cache.oververboseAccesses::total") {  printf("%s,  %s\n", prev.c_str(), subs.c_str()); }
        if (prev == "board.cache_hierarchy.l2-cache-0.oververboseAccesses::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        // metadata cache
        if (prev == "board.memory.secure_memory.l3.oververboseAccesses::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.memory.secure_memory.metadata_requests") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.memory.metadata_cache.oververboseAccesses::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}

        // Hits 
        //l1
        if (prev == "board.cache_hierarchy.l1d-cache-0.oververboseHits::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.cache_hierarchy.l1i-cache-0.oververboseHits::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        // l2
        if (prev == "board.cache_hierarchy.l2-cache-0.oververboseHits::total") {  printf("%s,    %s\n", prev.c_str(), subs.c_str()); }
        if (prev == "board.cache_hierarchy.l2cache.oververboseHits::total") {  printf("%s,    %s\n", prev.c_str(), subs.c_str()); }
        // metadata cache
        if (prev == "board.memory.secure_memory.l3.oververboseHits::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.memory.metadata_cache.oververboseHits::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}


        // Misses
        //l1
        if (prev == "board.cache_hierarchy.l1d-cache-0.oververboseMissRate::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.cache_hierarchy.l1i-cache-0.oververboseMissRate::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        // l2
        if (prev == "board.cache_hierarchy.l2cache.oververboseMisses::total") {  printf("%s,    %s\n", prev.c_str(), subs.c_str()); }
        if (prev == "board.cache_hierarchy.l2-cache-0.oververboseMissRate::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        // metadata cache
        if (prev == "board.memory.secure_memory.l3.oververboseMissRate::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.memory.secure_memory.metadata_misses") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}
        if (prev == "board.memory.metadata_cache.oververboseMisses::total") { printf("%s,  %s\n", prev.c_str(), subs.c_str());}

        prev = subs;
      }
    }
  }

  fin.close();
  return 1;
}
