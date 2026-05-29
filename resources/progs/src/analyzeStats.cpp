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
  long long insts; // commit instructions
  long meta_hits, meta_accesses, meta_misses, mem_reads;
  long long reqs_processed;
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
  meta_hits = 1;
  meta_accesses = 1;
  meta_misses = 1;
  ipc = 0;
  insts = 0;
  reqs_processed = 1;
  mem_reads = 0;

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
          ipc += stod(subs.c_str());
        }

        if (prev == "board.processor.switch0.core.commitStats0.numInsts"
          || prev == "board.processor.switch1.core.commitStats0.numInsts"
          || prev == "board.processor.switch2.core.commitStats0.numInsts"
          || prev == "board.processor.switch3.core.commitStats0.numInsts") {
          
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          insts += strtoll(subs.c_str(), nullptr, 10);
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

        // Requests processed
        if (prev == "board.memory.secure_memory.requests_processed") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          reqs_processed = atoi(subs.c_str());
        }

        // Metadata
        if (prev == "board.memory.metadata_cache.overallAccesses::total") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          meta_accesses = atoi(subs.c_str());
        }
        if (prev == "board.memory.metadata_cache.overallHits::total") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          meta_hits = atoi(subs.c_str());
        }
        if (prev == "board.memory.metadata_cache.overallMisses::total") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          meta_misses = atoi(subs.c_str());
        }

        // Memory reads
        if (prev == "board.memory.mem_ctrl0.dram.numReads::total") {
          printf("%s,     %s\n", prev.c_str(), subs.c_str()); 
          mem_reads = atoi(subs.c_str());
        }
        prev = subs;
     }
  }

  fin.close();

  // print final stats

  printf("\n\n===========\n Final Statistics: \n===========\n\n");

  printf("numCycles IPC Commit-Instructions L1I-Accesses L1I-Hits L1I-Misses L1D-Accesses L1D-Hits L1D-Misses L2-Accesses L2-Hits L2-Misses Requests-Processed Meta-Accesses Meta-Hits Meta-Misses Memory-Accesses\n\n");
  printf("%ld %.4f %lld %ld %ld %ld %ld %ld %ld %ld %ld %ld %lld %ld %ld %ld %ld\n", numCycles, ipc / 4, insts, l1i_accesses, l1i_hits, l1i_misses, l1d_accesses, l1d_hits, l1d_misses, l2_accesses, l2_hits, l2_misses, reqs_processed, meta_accesses, meta_hits, meta_misses, mem_reads);
  return 1;
}
