#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdlib>

using namespace std;

// Structure to hold all stats for a specific combination of Benchmark and Policy
struct Stats {
    double cycles, ipc, insts;
    double l1i_acc, l1i_hits, l1i_miss;
    double l1d_acc, l1d_hits, l1d_miss;
    double l2_acc, l2_hits, l2_miss;
    double req_handled, meta_acc, meta_hits, meta_miss;
    double mem_reads; // New field for Memory Reads

    Stats() : cycles(0), ipc(0), insts(0), l1i_acc(0), l1i_hits(0), l1i_miss(0),
              l1d_acc(0), l1d_hits(0), l1d_miss(0), l2_acc(0), l2_hits(0), l2_miss(0),
              req_handled(0), meta_acc(0), meta_hits(0), meta_miss(0), mem_reads(0) {}
};

vector<string> parse_line(string line) {
  istringstream ss(line);
  vector<string> words;
  string word;

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
  vector<string> words;

  // C++98 compliant vector initialization
  vector<string> order_benchmarks;
  order_benchmarks.push_back("Blackscholes");
  order_benchmarks.push_back("Fluidanimate");
  order_benchmarks.push_back("Bodytrack");
  order_benchmarks.push_back("Canneal");
  order_benchmarks.push_back("Dedup");
  order_benchmarks.push_back("Streamcluster");
  order_benchmarks.push_back("Microbenchmark");

  vector<string> order_policies;
  order_policies.push_back("No Security");
  order_policies.push_back("Hashing Only");
  order_policies.push_back("Encryption Only");
  order_policies.push_back("Integrity Tree");
  order_policies.push_back("Hashing + Encryption");
  order_policies.push_back("Hashing + Integrity");
  order_policies.push_back("Encryption + Integrity");
  order_policies.push_back("Full Security");

  // Map to hold Benchmark -> (Policy -> Stats)
  unordered_map<string, unordered_map<string, Stats> > data;

  int count;

  if (argc != 2) { fprintf(stderr, "USAGE:\n./processGraph /path/To/stats.csv\n"); exit(1); }

  fname = argv[1];
  file.open(fname);
  if (!file) { fprintf(stderr, "%s failed to open\n", fname.c_str()); exit(1); }

  printf("Processing %s\n\n", fname.c_str());

  // Process each line
  while (getline(file, line)) {
     // C++98 compliant way to strip the invisible carriage return
     if (!line.empty() && line[line.size() - 1] == '\r') {
         line.erase(line.size() - 1);
     }

     if (line.empty()) { continue; }

     words = parse_line(line);

     if (words.size() > 18 && words[0] != "Benchmark" && !words[0].empty()) {
        string bench = words[0];
        string policy = words[1];

        // Ensure it's one of the targeted benchmarks
        if (bench != "Blackscholes" && bench != "Canneal" && bench != "Bodytrack" && 
            bench != "Fluidanimate" && bench != "Dedup" && bench != "Streamcluster" && bench != "Microbenchmark") {
            continue;
        }

        Stats s;
        s.cycles = strtod(words[7].c_str(), NULL);
        s.ipc = strtod(words[8].c_str(), NULL);
        s.insts = strtod(words[9].c_str(), NULL);
        s.l1i_acc = strtod(words[10].c_str(), NULL);
        s.l1i_hits = strtod(words[11].c_str(), NULL);
        s.l1i_miss = strtod(words[12].c_str(), NULL);
        s.l1d_acc = strtod(words[13].c_str(), NULL);
        s.l1d_hits = strtod(words[14].c_str(), NULL);
        s.l1d_miss = strtod(words[15].c_str(), NULL);
        s.l2_acc = strtod(words[16].c_str(), NULL);
        s.l2_hits = strtod(words[17].c_str(), NULL);
        s.l2_miss = strtod(words[18].c_str(), NULL);

        s.req_handled = strtod(words[19].c_str(), NULL);
        s.meta_acc = strtod(words[20].c_str(), NULL);
        s.meta_hits = strtod(words[21].c_str(), NULL);
        s.meta_miss = strtod(words[22].c_str(), NULL);

        // Memory Reads Parsing
        s.mem_reads = strtod(words[23].c_str(), NULL);

        data[bench][policy] = s;
    }
  }

  // Helper macro to generate graphs (C++98 compliant)
  #define GENERATE_GRAPH(FILENAME, YLABEL, STAT_VAR, BASE_POLICY) \
  do { \
      /* First pass to find the maximum normalized value for y-axis scaling */ \
      double max_norm = 0; \
      for (size_t b_idx = 0; b_idx < order_benchmarks.size(); ++b_idx) { \
          string bench = order_benchmarks[b_idx]; \
          double base_val = data[bench][BASE_POLICY].STAT_VAR; \
          for (size_t p_idx = 0; p_idx < order_policies.size(); ++p_idx) { \
              string policy = order_policies[p_idx]; \
              double val = data[bench][policy].STAT_VAR; \
              double normalized_val = (base_val > 0) ? (val / base_val) : 0; \
              if (normalized_val > max_norm) max_norm = normalized_val; \
          } \
      } \
      if (max_norm == 0) max_norm = 1.0; \
      double y_max = max_norm * 1.1; /* Add 10% headroom */ \
      \
      /* Dynamically scale the x-axis configuration based on number of benchmarks */ \
      double x_max = order_benchmarks.size() * 9.0 + .9; \
      double x_size = order_benchmarks.size() * 1.1; \
      \
      ofile.open("jgr/" FILENAME); \
      if (!ofile.is_open()) { printf("failed to open %s\n", FILENAME); return 1; } \
      ofile << "newgraph\n\nxaxis size " << x_size << "\n  min 0.1 max " << x_max << " mhash 0 shash 0\n  label : Benchmark\n\n"; \
      ofile << "  no_auto_hash_labels\n"; \
      \
      /* Loop dynamically to output correct tick labels and locations */ \
      for (size_t b_idx = 0; b_idx < order_benchmarks.size(); ++b_idx) { \
          ofile << "  hash_label at " << (b_idx * 9.0 + 4) << " : " << order_benchmarks[b_idx] << "\n"; \
      } \
      ofile << "\n"; \
      ofile << "  hash_labels fontsize 12 font Times-Italic hjl vjc rotate -60\n\n"; \
      ofile << "yaxis min 0 max " << y_max << " size 5\n  label : " YLABEL "\n  grid_lines grid_gray .7\n  hash .1 mhash 0\n\n"; \
      ofile << "legend top\n\nnewline pts .1 0 " << x_max << " 0\n\n"; \
      \
      count = 1; \
      for (size_t b_idx = 0; b_idx < order_benchmarks.size(); ++b_idx) { \
          string bench = order_benchmarks[b_idx]; \
          double base_val = data[bench][BASE_POLICY].STAT_VAR; \
          for (size_t p_idx = 0; p_idx < order_policies.size(); ++p_idx) { \
              string policy = order_policies[p_idx]; \
              double val = data[bench][policy].STAT_VAR; \
              double normalized_val = (base_val > 0) ? (val / base_val) : 0; \
              \
              if (p_idx == 0) { \
                  ofile << "newcurve marktype xbar cfill 0 1 0\n  marksize .6 .025\n"; \
                  if (count <= 8) ofile << "  label : No Security\n"; \
              } else if (p_idx == 1) { \
                  ofile << "newcurve marktype xbar cfill 1 1 0\n  marksize .6 .025\n"; \
                  if (count <= 8) ofile << "  label : Hashing Only\n"; \
              } else if (p_idx == 2) { \
                  ofile << "newcurve marktype xbar cfill 1 0 0\n  marksize .6 .025\n"; \
                  if (count <= 8) ofile << "  label : Encryption Only\n"; \
              } else if (p_idx == 3) { \
                  ofile << "newcurve marktype xbar cfill 1 0 1\n  marksize .6 .025\n"; \
                  if (count <= 8) ofile << "  label : Integrity Tree\n"; \
              } else if (p_idx == 4) { \
                  ofile << "newcurve marktype xbar cfill 1 .5 .5\n  marksize .6 .025\n"; \
                  if (count <= 8) ofile << "  label : Hashing + Encryption\n"; \
              } else if (p_idx == 5) { \
                  ofile << "newcurve marktype xbar cfill .5 .1 .2\n  marksize .6 .025\n"; \
                  if (count <= 8) ofile << "  label : Hashing + Integrity\n"; \
              }  else if (p_idx == 6) { \
                  ofile << "newcurve marktype xbar cfill 0 .5 .5\n  marksize .6 .025\n"; \
                  if (count <= 8) ofile << "  label : Encryption + Integrity\n"; \
              }  else if (p_idx == 7) { \
                  ofile << "newcurve marktype xbar cfill 0 0 0\n  marksize .6 .025\n"; \
                  if (count <= 8) ofile << "  label : Full Security\n"; \
              } \
              ofile << "  pts\n  " << count << " " << normalized_val << "\n\n"; \
              count++; \
          } \
          /* Add the spacer blank bar */ \
          ofile << "newcurve marktype xbar cfill 0 1 1\n  marksize .8 .025\n  pts\n  " << count << " 0\n\n"; \
          count++; \
      } \
      ofile.close(); \
  } while (0)

  // ============= Generate All Graphs ============== //
  // Cycles, ipc & instructions
  GENERATE_GRAPH("numCycles.jgr", "numCycles (Normalized)", cycles, "No Security");
  GENERATE_GRAPH("ipc.jgr", "IPC (Normalized)", ipc, "No Security");
  GENERATE_GRAPH("insts.jgr", "Commit Instructions (Normalized)", insts, "No Security");

  // On-chip caches
  GENERATE_GRAPH("l1i-acc.jgr", "L1 I-Cache Accesses (Normalized)", l1i_acc, "No Security");
  GENERATE_GRAPH("l1i-hits.jgr", "L1 I-Cache Hits (Normalized)", l1i_hits, "No Security");
  GENERATE_GRAPH("l1i-miss.jgr", "L1 I-Cache Misses (Normalized)", l1i_miss, "No Security");
  GENERATE_GRAPH("l1d-acc.jgr", "L1 D-Cache Accesses (Normalized)", l1d_acc, "No Security");
  GENERATE_GRAPH("l1d-hits.jgr", "L1 D-Cache Hits (Normalized)", l1d_hits, "No Security");
  GENERATE_GRAPH("l1d-miss.jgr", "L1 D-Cache Misses (Normalized)", l1d_miss, "No Security");
  GENERATE_GRAPH("l2-acc.jgr", "L2 Cache Accesses (Normalized)", l2_acc, "No Security");
  GENERATE_GRAPH("l2-hits.jgr", "L2 Cache Hits (Normalized)", l2_hits, "No Security");
  GENERATE_GRAPH("l2-miss.jgr", "L2 Cache Misses (Normalized)", l2_miss, "No Security");

  // Metadata requests (Normalized to Integrity Tree)
  GENERATE_GRAPH("meta-acc.jgr", "Metadata Accesses (Normalized)", meta_acc, "Integrity Tree");
  GENERATE_GRAPH("meta-hits.jgr", "Metadata Hits (Normalized)", meta_hits, "Integrity Tree");
  GENERATE_GRAPH("meta-miss.jgr", "Metadata Misses (Normalized)", meta_miss, "Integrity Tree");

  // Memory Reads graph
  GENERATE_GRAPH("mem-reads.jgr", "Memory Reads (Normalized)", mem_reads, "No Security");

  // Output jgraph commands for easy copy
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
  printf("jgraph -P jgr/meta-acc.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/meta-acc.jpg\n");
  printf("jgraph -P jgr/meta-hits.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/meta-hits.jpg\n");
  printf("jgraph -P jgr/meta-miss.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/meta-miss.jpg\n");
  printf("jgraph -P jgr/mem-reads.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/mem-reads.jpg\n");

  return 0;
}
