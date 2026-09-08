// Will Buziak
// Process stats file from command line and output jgraph
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <algorithm>
using namespace std;

vector <string> parse_line(string line) {
  // parse each line without hardcoding the benchmark names
  istringstream ss(line);
  vector <string> words;
  string word;

  words.clear();

  while (getline(ss, word, ',')) {
    words.push_back(word);
  }

  return words;
}

int main(int argc, char** argv)
{
  string fname, line;
  ifstream file;
  ofstream ofile;
  vector <string> words;
  vector <string> benchmarks;
  vector <string> configs;

  // Use a composite key (Benchmark-Config) to map to stats
  unordered_map <string, string> numCycles, ipc, insts, l1i_acc, l1i_hits, l1i_miss;
  unordered_map <string, string> l1d_acc, l1d_hits, l1d_miss, l2_acc, l2_hits, l2_miss;

  int count;
  double Cycles = 0, Ipc = 0, Insts = 0, L1i_acc = 0, L1i_hits = 0, L1i_miss = 0;
  double L1d_acc = 0, L1d_hits = 0, L1d_miss = 0, L2_acc = 0, L2_hits = 0, L2_miss = 0;

  if (argc != 2) {
      fprintf(stderr, "USAGE:\n./processGraph \"/path/To/Results - Results_2.csv\"\n");
      exit(1);
  }

  // grab filename
  fname = argv[1];

  // open file
  file.open(fname.c_str());
  if (!file) { fprintf(stderr, "%s failed to open\n", fname.c_str()); exit(1); }

  printf("Processing %s\n\n", fname.c_str());

  // Process each line
  while (getline(file, line)) {
     // FIX: Strip the invisible carriage return if it exists
     if (!line.empty() && line.back() == '\r') {
         line.pop_back();
     }

     if (line.empty()) { continue; }

     words.clear();
     words = parse_line(line);

     // Check bounds and ensure we aren't reading the header row
     if (words.size() > 18 && words[1] != "Benchmark" && words[0] != "Config" && words[0] != "System") {
        string config = words[0];
        string benchmark = words[1];

        // Track unique benchmarks and configurations dynamically
        if (find(benchmarks.begin(), benchmarks.end(), benchmark) == benchmarks.end()) {
            benchmarks.push_back(benchmark);
        }
        if (find(configs.begin(), configs.end(), config) == configs.end()) {
            configs.push_back(config);
        }

        string key = benchmark + "-" + config;

        numCycles[key] = words[7];
        ipc[key] = words[8];
        insts[key] = words[9];
        l1i_acc[key] = words[10];
        l1i_hits[key] = words[11];
        l1i_miss[key] = words[12];
        l1d_acc[key] = words[13];
        l1d_hits[key] = words[14];
        l1d_miss[key] = words[15];
        l2_acc[key] = words[16];
        l2_hits[key] = words[17];
        l2_miss[key] = words[18];

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
  ofile.open("jgr/numCycles.jgr");
  if (!ofile.is_open()) { printf("failed to open numCycles.jgr\n");  return 1; }

  // Calculate dynamic axis limits based on how many configurations were in the file
  int block_size = configs.size() + 1;
  double max_x = benchmarks.size() * block_size + 0.9;

  ofile << "newgraph\n\n" << "xaxis size 5\n" << "  min 0.1 max " << max_x << " mhash 0 shash 0\n  label : Benchmark\n\n";
  ofile << "  no_auto_hash_labels\n";

  // Place hash labels in the center of each benchmark cluster
  for (size_t i = 0; i < benchmarks.size(); i++) {
      double pos = (i * block_size) + (configs.size() / 2.0) + 1.0;
      ofile << "  hash_label at " << pos << " : " << benchmarks[i] << "\n";
  }

  ofile << "\n  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n";
  ofile << "yaxis min 0 max 1 size 5\n  label : numCycles (Normalized)\n  grid_lines grid_gray .7\n\n";
  ofile << "legend top\n\n";
  ofile << "newline pts .1 0 " << max_x << " 0\n\n";

  // Palette to cycle through matching the original format style
  string cfill_colors[] = {"0 1 0", "1 1 0", "1 0 0", "1 0 1", "0 1 1", "0 0 1"};

  count = 1;
  for (size_t b = 0; b < benchmarks.size(); b++) {
      for (size_t c = 0; c < configs.size(); c++) {
          ofile << "newcurve marktype xbar cfill " << cfill_colors[c % 6] << "\n\n  marksize .8 .025\n\n";

          if (b == 0) {
              ofile << "  label : " << configs[c] << "\n\n  pts\n  ";
          } else {
              ofile << "  pts \n  ";
          }

          string key = benchmarks[b] + "-" + configs[c];
          double val = 0;
          if (numCycles.find(key) != numCycles.end()) {
              val = strtod(numCycles[key].c_str(), NULL);
          }

          ofile << count << " " << val / Cycles << "\n\n";
          count++;
      }

      // Gap spacing between benchmark groups (matches original `count % 5 == 0` logic)
      ofile << "\nnewcurve marktype xbar cfill 0 1 1\n\n  marksize .8 .025\n\n  pts\n  ";
      ofile << count << " " << 0 << " \n\n";
      count++;
  }

  ofile << "\n";
  ofile.close();

  return 0;
}
