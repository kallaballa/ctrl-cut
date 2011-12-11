/*
 * Cli.cpp
 *
 *  Created on: Dec 6, 2011
 *      Author: elchaschab
 */
#include <boost/program_options.hpp>
#include <iostream>
#include <iterator>

#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include "Cli.h"

namespace po = boost::program_options;
using namespace std;

Cli::Cli() {
  // TODO Auto-generated constructor stub

}

Cli::~Cli() {
  // TODO Auto-generated destructor stub
}


/* Read a string, and return a pointer to it.
   Returns NULL on EOF. */
//const bool Cli::get_readline(string& line) {
  /* Get a line from the user. */
/*  char * line_read = readline ("");
  line = line_read;*/

  /* If the line has any text in it,
     save it on the history. */
  /*if (line_read && *line_read)
    add_history (line_read);

  if(line_read == NULL) {
    return true;
  } else {
    free line_read;
    return false;
  }
}*/


int main(int ac, char* av[]) {
  try {
      po::options_description desc("Allowed options");
      desc.add_options()
          ("help", "produce help message")
          ("compression", po::value<int>(), "set compression level")
      ;

      po::variables_map vm;
      po::store(po::parse_command_line(ac, av, desc), vm);
      po::notify(vm);

      if (vm.count("help")) {
          cout << desc << "\n";
          return 1;
      }

      if (vm.count("compression")) {
          cout << "Compression level was set to "
               << vm["compression"].as<int>() << ".\n";
      } else {
          cout << "Compression level was not set.\n";
      }
  }
  catch(exception& e) {
      cerr << "error: " << e.what() << "\n";
      return 1;
  }
  catch(...) {
      cerr << "Exception of unknown type!\n";
  }

  return 0;
}
