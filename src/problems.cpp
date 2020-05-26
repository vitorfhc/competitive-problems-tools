#include <map>

#include <unistd.h>
#include <getopt.h>

#include "defs.h"
#include "error.h"

#include "init.h"
#include "check.h"
#include "clean.h"
#include "gentex.h"
#include "genpdf.h"
#include "cptools.h"


// Raw strings
static const std::string help_message {
R"message(
Format, test and pack competitive programming problems.

    Action              Description

    init                Generates template files on current directory.
    check               Verifies problem files and tools.
    clean               Removes autogenerated files.
    genpdf              Generates a PDF file from the problem description. 
    gentex              Generates a LaTeX file from the problem description. 
)message"
};


namespace cptools::problem {

    // Global variables
    std::map<std::string, int (*)(int, char *const [], std::ostream&, std::ostream&)> commands {
        { "init", init::run },
        { "check", check::run },
        { "clean", clean::run },
        { "gentex", gentex::run },
        { "genpdf", genpdf::run },
    };

    static struct option longopts[] = {
       { "help", no_argument, NULL, 'h' },
       { 0, 0, 0, 0 }
    };

    // Auxiliary routines
    std::string usage()
    {
        return "Usage: " NAME " problem [-h] action";
    }

    std::string help()
    {
        return usage() + help_message;
    }

    // API functions
    int run(int argc, char* const argv[], std::ostream& out, std::ostream& err)
    {
        if (argc >= 3)
        {
            std::string command { argv[2] };

            for (const auto& [cmd, exec] : commands)
                if (cmd == command)
                    return exec(argc, argv, std::cout, std::cerr);

            if (command.front() != '-')
            {
                err << NAME << " problem: invalid action '" << command << "'\n";
                return CP_TOOLS_ERROR_INVALID_COMMAND;
            }
        }

        int option = -1;

        while ((option = getopt_long(argc, argv, "h", longopts, NULL)) != -1)
        {
            switch (option) {
            case 'h':
                out << help() << '\n';
                return CP_TOOLS_OK;

            default:
                err << help() << '\n';
                return CP_TOOLS_ERROR_INVALID_PARAMETER;
            }
        }

        out << usage() << '\n';
        return CP_TOOLS_OK;
    }
}
