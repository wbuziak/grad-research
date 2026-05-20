// Will Buziak
// Process stats file from command line and output jgraph
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <unordered_map>
using namespace std;

vector <string> parse_line(string line) {
  // parse each line
  istringstream ss(line);
  vector <string> words;
  string word;
  bool collect;

  collect = false;
  words.clear();

  while (getline(ss, word, ',')) {
    if (!collect) {
      if (word == "Blackscholes" || word == "Canneal" || word == "Bodytrack" || word == "Fluidanimate") {
        collect = true;
        words.push_back(word);
      }
    }
    else { words.push_back(word); }
  }

  return words;
}

int main(int argc, char** argv)
{
  string fname, line, benchmark;
  ifstream file;
  ofstream ofile;
  vector <string> words;
  unordered_multimap <string, string> numCycles, ipc, insts, l1i_acc, l1i_hits, l1i_miss;
  unordered_multimap <string, string> l1d_acc, l1d_hits, l1d_miss, l2_acc, l2_hits, l2_miss;
  unordered_multimap <string, string>::iterator mit;
  int count;
  double Cycles, Ipc, Insts, L1i_acc, L1i_hits, L1i_miss, L1d_acc, L1d_hits, L1d_miss;
  double L2_acc, L2_hits, L2_miss;

  if (argc != 2) { fprintf(stderr, "USAGE:\n./processGraph /path/To/stats.csv\n"); exit(1); }

  // grab filename
  fname = argv[1];

  // open file
  file.open(fname);
  if (!file) { fprintf(stderr, "%s failed to open\n", fname.c_str()); exit(1); }

  printf("Processing %s\n\n", fname.c_str());
  
  // Process each line
  Cycles = 0;
  while (getline(file, line)) {
     // FIX: Strip the invisible carriage return if it exists
     if (!line.empty() && line.back() == '\r') {
         line.pop_back(); 
     }

     if (line.empty()) { continue; }

     words.clear();
     words = parse_line(line);
        
     if (!words.empty()) {
        numCycles.insert(make_pair(words[1], words[7]));
        ipc.insert(make_pair(words[1], words[8]));
        insts.insert(make_pair(words[1], words[9]));
        l1i_acc.insert(make_pair(words[1], words[10]));
        l1i_hits.insert(make_pair(words[1], words[11]));
        l1i_miss.insert(make_pair(words[1], words[12]));
        l1d_acc.insert(make_pair(words[1], words[13]));
        l1d_hits.insert(make_pair(words[1], words[14]));
        l1d_miss.insert(make_pair(words[1], words[15]));
        l2_acc.insert(make_pair(words[1], words[16]));
        l2_hits.insert(make_pair(words[1], words[17]));
        l2_miss.insert(make_pair(words[1], words[18]));

      // Find max values for normalization
      if (Cycles < strtod(words[7].c_str(), NULL)) Cycles = strtod(words[7].c_str(), NULL);
      if (Ipc < strtod(words[8].c_str(), NULL)) Ipc = strtod(words[8].c_str(), NULL);
      if (Insts < strtod(words[9].c_str(), NULL)) Insts = strtod(words[9].c_str(), NULL);
      if (L1i_acc < strtod(words[10].c_str(), NULL)) L1i_acc = strtod(words[10].c_str(), NULL);
      if (L1i_hits < strtod(words[11].c_str(), NULL)) L1i_hits = strtod(words[11].c_str(), NULL);
      if (L1i_miss < strtod(words[12].c_str(), NULL)) L1i_miss = strtod(words[12].c_str(), NULL);
      if (L1d_acc < strtod(words[13].c_str(), NULL)) L1d_acc = strtod(words[13].c_str(), NULL);
      if (L1d_hits < strtod(words[14].c_str(), NULL)) L1d_hits = strtod(words[14].c_str(), NULL);
      if (L1d_miss < strtod(words[15].c_str(), NULL)) L1d_miss = strtod(words[15].c_str(), NULL);
      if (L2_acc < strtod(words[16].c_str(), NULL)) L2_acc = strtod(words[16].c_str(), NULL);
      if (L2_hits < strtod(words[17].c_str(), NULL)) L2_hits = strtod(words[17].c_str(), NULL);
      if (L2_miss < strtod(words[18].c_str(), NULL)) L2_miss = strtod(words[18].c_str(), NULL);
    }

  }

  // create graphs

  // =============numCycles============== //
  // create a jgr file
  ofile.open("jgr/numCycles.jgr");
  if (!ofile.is_open()) { printf("failed to open numCycles.jgr\n");  return 1; }
  // newgraph set x axis bounds
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";

  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n"; 
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n"; // slant xaxis labels

  // y axis bounds
  ofile << "yaxis min 0 max 1 size 5\n  label : numCycles (Normalized)\n  grid_lines grid_gray .7\n\n";
  // legend
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n"; // do this so xaxis is not grey

  // color bars

  // place bars
  count = 1;
  for (mit = numCycles.begin(); mit != numCycles.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) {
        ofile << "  label : No Security\n\n  pts\n  ";
      } else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) {
        ofile << "  label : Configurable Security\n\n  pts\n  ";
      } else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) {
        ofile << "  label : Integrity Tree\n\n  pts\n  ";
      } else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) {
        ofile << "  label : MCX - Never\n\n  pts\n  ";
      } else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / Cycles << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

// =============ipc============== //
  ofile.open("jgr/ipc.jgr");
  if (!ofile.is_open()) { printf("failed to open ipc.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : IPC (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = ipc.begin(); mit != ipc.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / Ipc << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

  // =============commit instructions============== //
  ofile.open("jgr/insts.jgr");
  if (!ofile.is_open()) { printf("failed to open insts.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : Commit Instructions (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = insts.begin(); mit != insts.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / Insts << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

  // =============L1I - Accesses============== //
  ofile.open("jgr/l1i-acc.jgr");
  if (!ofile.is_open()) { printf("failed to open l1i-acc.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : L1 I-Cache Accesses (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = l1i_acc.begin(); mit != l1i_acc.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / L1i_acc << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

  // =============L1I - Hits============== //
  ofile.open("jgr/l1i-hits.jgr");
  if (!ofile.is_open()) { printf("failed to open l1i-hits.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : L1 I-Cache Hits (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = l1i_hits.begin(); mit != l1i_hits.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / L1i_hits << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

  // =============L1I - Misses============== //
  ofile.open("jgr/l1i-miss.jgr");
  if (!ofile.is_open()) { printf("failed to open l1i-miss.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : L1 I-Cache Misses (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = l1i_miss.begin(); mit != l1i_miss.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / L1i_miss << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

// =============L1D - Accesses============== //
  ofile.open("jgr/l1d-acc.jgr");
  if (!ofile.is_open()) { printf("failed to open l1d-acc.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : L1 D-Cache Accesses (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = l1d_acc.begin(); mit != l1d_acc.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / L1d_acc << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

  // =============L1D - Hits============== //
  ofile.open("jgr/l1d-hits.jgr");
  if (!ofile.is_open()) { printf("failed to open l1d-hits.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : L1 D-Cache Hits (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = l1d_hits.begin(); mit != l1d_hits.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / L1d_hits << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

  // =============L1D - Misses============== //
  ofile.open("jgr/l1d-miss.jgr");
  if (!ofile.is_open()) { printf("failed to open l1d-miss.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : L1 D-Cache Misses (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = l1d_miss.begin(); mit != l1d_miss.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / L1d_miss << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

  // =============L2 - Accesses============== //
  ofile.open("jgr/l2-acc.jgr");
  if (!ofile.is_open()) { printf("failed to open l2-acc.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : L2 Cache Accesses (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = l2_acc.begin(); mit != l2_acc.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / L2_acc << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

  // =============L2 - Hits============== //
  ofile.open("jgr/l2-hits.jgr");
  if (!ofile.is_open()) { printf("failed to open l2-hits.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : L2 Cache Hits (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = l2_hits.begin(); mit != l2_hits.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / L2_hits << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();

  // =============L2 - Misses============== //
  ofile.open("jgr/l2-miss.jgr");
  if (!ofile.is_open()) { printf("failed to open l2-miss.jgr\n");  return 1; }
  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max 20.9 mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";
  ofile << "  hash_label at 2 : Canneal\n";
  ofile << "  hash_label at 7 : Bodytrack\n";
  ofile << "  hash_label at 12 : Fluidanimate\n";
  ofile << "  hash_label at 17 : Blackscholes\n\n";
  ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : L2 Cache Misses (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 10.9 0\n\n";

  count = 1;
  for (mit = l2_miss.begin(); mit != l2_miss.end(); mit++) {
    if (count % 5 == 0) {
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " ";
      count++;
    }
    if (count % 5 == 1) {
      ofile << "newcurve marktype xbar cfill 0 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : No Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    if (count % 5 == 2) {
      ofile << "newcurve marktype xbar cfill 1 1 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Configurable Security\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 3) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 0\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : Integrity Tree\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    else if (count % 5 == 4) {
      ofile << "\nnewcurve marktype xbar cfill 1 0 1\n\n  marksize .8 .025\n\n";
      if (count < 5) ofile << "  label : MCX - Never\n\n  pts\n  "; else ofile << "  pts \n  ";
    }
    ofile << count << " " << strtod(mit->second.c_str(), NULL) / L2_miss << "\n\n";
    count++;
  }
  ofile << "\n";
  ofile.close();


  printf("jgraph -P jgr/numCycles.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/numCycles.jpg\n");
  printf("jgraph -P jgr/ipc.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/ipc.jpg\n");
  printf("jgraph -P jgr/insts.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/insts.jpg\n");
  printf("jgraph -P jgr/l1i-acc.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1i-acc.jpg\n");
  printf("jgraph -P jgr/l1i-hits.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1i-hits.jpg\n");
  printf("jgraph -P jgr/l1i-miss.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1i-miss.jpg\n");
  printf("jgraph -P jgr/l1d-acc.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1d-acc.jpg\n");
  printf("jgraph -P jgr/l1d-hits.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1d-hits.jpg\n");
  printf("jgraph -P jgr/l1d-miss.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1d-miss.jpg\n");
  printf("jgraph -P jgr/l2-acc.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l2-acc.jpg\n");
  printf("jgraph -P jgr/l2-hits.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l2-hits.jpg\n");
  printf("jgraph -P jgr/l2-miss.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l2-miss.jpg\n");
  return 0;
}
