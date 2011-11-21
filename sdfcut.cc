#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

const unsigned int PROGRESS_GRANULARITY = 50;

inline long max(long x, long y) {
  return x > y ? x: y;
}

void usage() {
  std::cerr << "Usage: sdfcut file.sdf NUM_MOLS_PER_FILE" << std::endl;
}

std::string next_output_file(std::fstream& ofs, const char* prefix)
{
  static unsigned int i = 0;
  std::stringstream ss(std::stringstream::out);
  ss << prefix << "." << i ++;
  std::string ofp = ss.str();
  ofs.open(ofp.c_str(), std::fstream::out);
  if (! ofs.good()) {
    return std::string("");
  }
  std::cerr << ofp << " >> ";
  return ofp;
}

int main(int argc, char* argv[])
{
  if (argc != 3) {
    usage();
    return 1;
  }

  // parse and prepare input file
  std::ifstream _ifs;
  bool use_stdin = strcmp(argv[1], "-") == 0;
  const char* ifp = argv[1];
  std::istream& ifs = use_stdin ? std::cin : _ifs;
  if (! use_stdin) {
    _ifs.open(ifp, std::ios::in);
    if (! ifs.good()) {
      std::cerr << "Fail to open input file: " << ifp << std::endl;
      return 1;
    }
  } else {
    std::cerr << "Read from stdin..." << std::endl;
    ifp = "sdfcut-out.sdf";
  }

  // parse per-file limit
  long per_file = 0;
  per_file = strtol(argv[2], NULL, 10);
  if (per_file == 0) {
    std::cerr << "Number of mols for per must be a positive integer." 
      << std::endl
      << "You specified '" << argv[2] << "'" << std::endl;
    return 1;
  }

  // output
  std::string ofp;
  std::fstream ofs;

  std::string buffer, buf;
  long counter = 0, c = 0;
  while (ifs.good()) {
    // open output if necessary
    if (counter == 0) {
        if ((ofp = next_output_file(ofs, ifp)) == "") {
          std::cerr << "Fail to open output file. Exit now. " << std::endl;
          return 1;
        }
    }

    // read input
    buffer.clear();
    buf.clear();
    std::getline(ifs, buffer, '$'); /* read till we see a dollar sign */
    if (buffer.length() == 0 && ifs.eof())
      break;
    std::getline(ifs, buf);     /* read till we see end of this line or EOF */
    // write what we've read
    ofs << buffer;
    if (buf.length())
      ofs << "$" << buf;
    ofs << "\n";                    /* this compensates the LF we consumed or
                                       will append an LF if the input file does
                                       not end with one */
    // did we just finish one mol?
    if (buf == "$$$" || ifs.eof()) {
      if (counter % PROGRESS_GRANULARITY == 0) 
        std::cerr << ".";
      ++ counter, ++ c;
      // did we reach the limit of the number of per-file molecules?
      if (counter == per_file) {
        std::cerr << std::endl;
        counter = 0;
        ofs.close();
      }
    }
  }
  
  if (ofs.good())
    ofs.close();

  // done
  std::cerr << std::endl << "Finsh processing " << c << " molecules" <<
    std::endl;

  // check why we exit
  if (not ifs.eof()) {
    std::cerr << "We did not end with a clean state, possibly due to IO error"
      << std::endl;
  }
  return 0;
}
