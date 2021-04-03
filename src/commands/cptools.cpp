#include <unordered_map>

#include <getopt.h>
#include <unistd.h>

#include "defs.h"
#include "error.h"

#include "commands/check.h"
#include "commands/clean.h"
#include "commands/cptools.h"
#include "commands/genpdf.h"
#include "commands/gentex.h"
#include "commands/init.h"
#include "commands/judge.h"
#include "commands/polygon/polygon.h"

// Raw strings
static const std::string help_message{
    R"message(
Format, test and pack competitive programming problems.

    Action              Description

    init                Generates template files on current directory.
    check               Verifies problem files and tools.
    clean               Removes autogenerated files.
    genpdf              Generates a PDF file from the problem description. 
    gentex              Generates a LaTeX file from the problem description. 
    judge               Runs a solution against all tests sets.
    polygon             Connects and synchronize with a Polygon account.
)message"};

static const std::string version_header{NAME " " VERSION "\n"};

static const std::string version_body{
    R"body(License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Written by Edson Alves.)body"};

namespace cptools::commands {

// Global variables
std::unordered_map<std::string, int (*)(int, char *const[], std::ostream &, std::ostream &)>
    commands{
        {"init", init::run},       {"check", check::run},   {"clean", clean::run},
        {"gentex", gentex::run},   {"genpdf", genpdf::run}, {"judge", judge::run},
        {"polygon", polygon::run},
    };

static struct option longopts[] = {
    {"help", no_argument, NULL, 'h'}, {"version", no_argument, NULL, 'v'}, {0, 0, 0, 0}};

// Auxiliary routines
std::string usage() { return "Usage: " NAME " [-h] [-v] action"; }

std::string help() { return usage() + help_message; }

std::string version() { return version_header + version_body; }

// API functions
int run(int argc, char *const argv[], std::ostream &out, std::ostream &err) {
    if (argc >= 2) {
        std::string command{argv[1]};
        auto it = commands.find(command);

        if (it != commands.end()) {
            return commands[command](argc, argv, out, err);
        }

        if (command.front() != '-') {
            err << NAME << ": invalid action '" << command << "'\n";
            return CP_TOOLS_ERROR_INVALID_COMMAND;
        }
    }

    int option = -1;

    while ((option = getopt_long(argc, argv, "hv", longopts, NULL)) != -1) {
        switch (option) {
        case 'h':
            out << help() << '\n';
            return CP_TOOLS_OK;

        case 'v':
            out << version() << '\n';
            return CP_TOOLS_OK;

        default:
            err << help() << '\n';
            return CP_TOOLS_ERROR_INVALID_OPTION;
        }
    }

    out << usage() << '\n';
    return CP_TOOLS_OK;
}
} // namespace cptools::commands
