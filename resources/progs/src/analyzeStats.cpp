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
  long numCycles, l2_accesses, l2_misses, l2_hits; // accumulate over all cores
  long l1i_hits, l1i_accesses, l1i_misses, l1d_accesses, l1d_hits, l1d_misses; 
  long insts; // commit instructions
  long meta_hits, meta_accesses, meta_misses;
  double ipc;

  if (argc != 2 && argc != 3 && argc != 4) { printf("USAGE:\n\n./bin/analyze stat-file\n./bin/analyze stat-file output-name.csv\n\n"); exit(1); }

  fin.open(argv[1]);


  if (fin.is_open()) {
    printf("\nStat,          Value\n--------------------\n\n");
  }
  else { printf("%s failed to open\n", argv[1]); }

  numCycles = 0; // accumulate statistics over all cores
  l1i_accesses = 0;
  l1i_misses = 0;
  l1i_hits = 0;
  l1d_hits = 0;
  l1d_accesses = 0;
  l1d_misses = 0;
  l2_accesses = 0;
  l2_misses = 0;
  l2_hits = 0;
  meta_hits = 0;
  meta_accesses = 0;
  meta_misses = 0;
  ipc = 0;
  insts = 0;

  while (getline(fin, line)) {     
      istringstream iss(line);

      while (iss >> subs) {
        // numCycles
        if (prev == "board.processor.switch0.core.numCycles" 
          || prev == "board.processor.switch1.core.numCycles" 
          || prev == "board.processor.switch2.core.numCycles"
          || prev == "board.processor.switch3.core.numCycles") { 
            printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
            numCycles += atoi(subs.c_str());
        }

        // IPC
        if (prev == "board.processor.switch0.core.ipc"
          || prev == "board.processor.switch1.core.ipc"
          || prev == "board.processor.switch2.core.ipc"
          || prev == "board.processor.switch3.core.ipc") {
          
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
        }

        if (prev == "board.processor.switch0.core.commitStats0.numInsts"
          || prev == "board.processor.switch1.core.commitStats0.numInsts"
          || prev == "board.processor.switch2.core.commitStats0.numInsts"
          || prev == "board.processor.switch3.core.commitStats0.numInsts") {
          
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          insts += atoi(subs.c_str());
        }

        // L1
        if (prev == "board.cache_hierarchy.l1dcaches0.overallHits::total"
          || prev == "board.cache_hierarchy.l1dcaches1.overallHits::total"
          || prev == "board.cache_hierarchy.l1dcaches2.overallHits::total"
          || prev == "board.cache_hierarchy.l1dcaches3.overallHits::total") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          l1d_hits += atoi(subs.c_str());
        }
        if (prev == "board.cache_hierarchy.l1icaches0.overallHits::total"
          || prev == "board.cache_hierarchy.l1icaches1.overallHits::total"
          || prev == "board.cache_hierarchy.l1icaches2.overallHits::total"
          || prev == "board.cache_hierarchy.l1icaches3.overallHits::total") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          l1i_hits += atoi(subs.c_str());
        }
        if (prev == "board.cache_hierarchy.l1dcaches0.overallMisses::total"
          || prev == "board.cache_hierarchy.l1dcaches1.overallMisses::total"
          || prev == "board.cache_hierarchy.l1dcaches2.overallMisses::total"
          || prev == "board.cache_hierarchy.l1dcaches3.overallMisses::total") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          l1d_misses += atoi(subs.c_str());
        }
        if (prev == "board.cache_hierarchy.l1icaches0.overallMisses::total"
          || prev == "board.cache_hierarchy.l1icaches1.overallMisses::total"
          || prev == "board.cache_hierarchy.l1icaches2.overallMisses::total"
          || prev == "board.cache_hierarchy.l1icaches3.overallMisses::total") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          l1i_misses += atoi(subs.c_str());
        }
        if (prev == "board.cache_hierarchy.l1dcaches0.overallAccesses::total"
          || prev == "board.cache_hierarchy.l1dcaches1.overallAccesses::total"
          || prev == "board.cache_hierarchy.l1dcaches2.overallAccesses::total"
          || prev == "board.cache_hierarchy.l1dcaches3.overallAccesses::total") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          l1d_accesses += atoi(subs.c_str());
        }
        if (prev == "board.cache_hierarchy.l1icaches0.overallAccesses::total"
          || prev == "board.cache_hierarchy.l1icaches1.overallAccesses::total"
          || prev == "board.cache_hierarchy.l1icaches2.overallAccesses::total"
          || prev == "board.cache_hierarchy.l1icaches3.overallAccesses::total") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          l1i_accesses += atoi(subs.c_str());
        }

        // L2
        if (prev == "board.cache_hierarchy.l2cache.overallHits::total") { 
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          l2_hits = atoi(subs.c_str()); 
        }
        if (prev == "board.cache_hierarchy.l2cache.overallMisses::total") { 
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          l2_misses = atoi(subs.c_str()); 
        }
        if (prev == "board.cache_hierarchy.l2cache.overallAccesses::total") { 
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          l2_accesses = atoi(subs.c_str()); 
        }

        prev = subs;
     }
  }

  fin.close();

  // print final stats

  printf("\n\n===========\n Final Statistics: \n===========\n\n");

  printf("numCycles:              %ld\n", numCycles);
  //printf("ipc:                  %lf\n", ipc / 4);
  printf("Commit Instructions:    %ld\n", numCycles);
  printf("L1I Accesses:           %ld\n", l1i_accesses);
  printf("L1I Hits:               %ld\n", l1i_hits);
  printf("L1I Misses:             %ld\n", l1i_misses);
  printf("L1D Accesses:           %ld\n", l1d_accesses);
  printf("L1D Hits:               %ld\n", l1d_hits);
  printf("L1D Misses:             %ld\n", l1d_misses);
  printf("L2 Accesses:            %ld\n", l2_accesses);
  printf("L2 Hits:                %ld\n", l2_hits);
  printf("L2 Misses:              %ld\n", l2_misses);
  return 1;
}
