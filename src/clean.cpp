#include <unistd.h>
#include <getopt.h>

#include "sh.h"
#include "defs.h"
#include "dirs.h"
#include "clean.h"
#include "error.h"
#include "message.h"


// Raw strings
static const string help_message {
R"message(
Remove autogenerated files. These files are located in '.cp-build' and '.cp-tmp' directories 
of the working folder.

    Option          Description

    -h              Generates this help message.
    --help

    -w              Directory to be cleaned.
    --working-dir

)message"
};

namespace cptools::clean {

    // Global variables
    static struct option longopts[] = {
       { "help", no_argument, NULL, 'h' },
       { "working-dir", required_argument, NULL, 'w' },
       { 0, 0, 0, 0 }
    };

    // Auxiliary routines
    string usage()
    {
        return "Usage: " NAME " clean [-h] [-w working-dir]";
    }

    string help()
    {
        return usage() + help_message;
    }

    int remove_autogenerated_files(const string& target, ostream& out, ostream& err)
    {
        out << message::info("Cleaning autogenerated files...") << "\n";

        // Exclui arquivos intermediários
        vector<string> exts { "class" };

        for (auto ext : exts)
        {
            string command { "find . -name '*." + ext + "' -exec rm {} \\;" };
            auto res = sh::execute(command, "");

            if (res.rc != CP_TOOLS_OK)
            {
                err << message::failure("Error removing '*." + ext + "' files") << "\n";
                err << message::trace(res.output) << '\n';
                return CP_TOOLS_ERROR_CLEAN_DELETE_FILES;
            }
        }

        // Localiza os diretórios que armazenam os arquivos autogerados
        string build_dir { target + "/" + CP_TOOLS_BUILD_DIR };
        string temp_dir { target + "/" + CP_TOOLS_TEMP_DIR };

        auto res1 = sh::is_dir(build_dir);
        auto res2 = sh::is_dir(temp_dir);

        if (res1.rc == CP_TOOLS_FALSE and res2.rc == CP_TOOLS_FALSE)
        {
            out << message::warning("No autogenerated files found!") << "\n";
            return CP_TOOLS_OK;
        }

        // Exclui os referidos diretórios
        for (auto dir : { build_dir, temp_dir })
        {
            auto res = sh::remove_dir(dir);

            if (res.rc != CP_TOOLS_OK)
            {
                err << message::failure("Error removing '" + dir + "'") << "\n";
                err << message::trace(res.output) << '\n';
                return CP_TOOLS_ERROR_CLEAN_DELETE_FILES;
            }
        }

        out << message::success() << '\n';
        return CP_TOOLS_OK;
    }

    // API functions
    int run(int argc, char* const argv[], ostream& out, ostream& err)
    {
        int option = -1;
        string target { "." };

        while ((option = getopt_long(argc, argv, "hw:", longopts, NULL)) != -1)
        {
            switch (option) {
            case 'h':
                out << help() << '\n';
                return 0;

            case 'w':
                target = string(optarg);
                break;

            default:
                err << help() << '\n';
                return CP_TOOLS_ERROR_CLEAN_INVALID_OPTION;
            }
        }

        return remove_autogenerated_files(target, out, err);
    }
}
