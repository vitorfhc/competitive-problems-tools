#ifndef CP_TOOLS_POLYGON_H
#define CP_TOOLS_POLYGON_H

#include <iostream>

using std::ostream;
using std::string;

namespace cptools::commands::polygon {

    int run(int argc, char * const argv[], ostream& out, ostream& err);

    string help();
    string usage();

}

#endif
