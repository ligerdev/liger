/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
**
****************************************************************************/

#include <iostream>
#include <tigon/Tigon.h>
#include <tigon/Log/LogManager.h>

#include <tigon/tigon_global.h>

#include <getopt.h>
#include <unistd.h>

using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;
using namespace Log;

void loadMatlabPlugin()
{
    // get the full path of the executable
    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    TString fullPathFile (result, (count > 0) ? count : 0);

    // remove the file from the path
    TString fullPath = fullPathFile.substr(0, fullPathFile.find_last_of("\\/"));

    // go up one directory
    TString buildPath = fullPath.substr(0, fullPath.find_last_of("\\/"));

    // move to the plugins folder
    TString pluginsPath = buildPath + TString("/lib/tigon/plugins");

    // Load the Matlab plugin
    PluginManager::instance()->definePath(pluginsPath);
    PluginManager::instance()->loadPlugins();
}

void usage(void) {
    printf("Usage: %s [OPTIONS]... [FILE]\n", program_invocation_short_name);
    printf("Command line interface for Liger.\n\n"
           "Loads and runs a Liger workflow.\n\n"
           "If a workflow runs an optimizer, the generated solution set is saved "
           "in a json file.\n\n");

    printf(	"Options:\n"
            " -h, --help            Show help options\n"
            " -v, --version         Show version information and exit\n"
            " -f, --file=<filename> Specify the Liger workflow file to be loaded and processed\n"
            " -o, --output=<path>   Override the default path where the solution set is stored\n\n");
    return;
}

void version(void) {

    printf("%s version 1.0\n\n", program_invocation_short_name);
    printf(	"Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk) \n\n"
            "This program is part of Liger.\n\n"
            "GNU Lesser General Public License Usage\n"
            "This file may be used under the terms of the GNU Lesser General\n"
            "Public License version 3 as published by the Free Software\n"
            "Foundation and appearing in the file LICENSE.LGPL included in the\n"
            "packaging of this file.  Please review the following information to\n"
            "ensure the GNU Lesser General Public License version 3 requirements\n"
            "will be met: https://www.gnu.org/licenses/lgpl-3.0.html.\n\n"
            "This is free software, and you are welcome to redistribute it under certain\n"
            "conditions. See the GNU Lesser General Public License for details. There is NO\n"
            "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
    return;
}


int main(int argc, char* argv[])
{
    static struct option long_options[] = {
                {"help",		no_argument,		0, 'h'},
                {"version",		no_argument,		0, 'V'},
                {"file",        required_argument,  0, 'f'},
                {"output",      required_argument,  0, 'o'},
                {0, 0, 0, 0} /* marks end of list */
    };

    int option_index = 0; /* getopt_long stores the option index here. */
    int opt;              /* it's actually going to hold a char */
    char* workflowFilePath = nullptr;
    char* popFilesPath = nullptr;

    while (0 < (opt = getopt_long(argc, argv, "f:ho:Vv",
                                  long_options, &option_index))) {

        switch(opt) {

        case 'f':
            workflowFilePath=(char*)malloc(PATH_MAX*sizeof(char));
            strcpy(workflowFilePath, optarg);
            break;

        case 'o':
            popFilesPath=(char*)malloc(PATH_MAX*sizeof(char));
            strcpy(popFilesPath, optarg);
            break;

        case 'V':
        case 'v':
            version();
            exit(EXIT_SUCCESS);

        case '?':
            /* getopt prints an error message right here */
            fprintf(stderr, "Try `%s --help' for more information.\n", program_invocation_short_name);
            exit(EXIT_FAILURE);

        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        default: /* should never happen */
            abort();
        }
    }

    if(workflowFilePath == nullptr) {
        fprintf(stderr, "Try `%s --help' for more information.\n", program_invocation_short_name);
        exit(EXIT_FAILURE);
    }

    // 1. Load the Matlab plugin if a compatible Matlab instalation is available
    #ifdef MATLAB_API
    loadMatlabPlugin();
    #endif


    // 2. Load the Liger workflow
    TString wFile(workflowFilePath);
    TigonEngine* engine = TigonEngine::instance();
    engine->clearFlow();
    //bool success = engine->loadFlowFromFile("/home/jduro/ogata.lgr");
    bool success = engine->loadFlowFromFile(wFile);
    if(!success) {
        fprintf(stderr, "File %s not found, or not a valid Liger workflow file\n", wFile.c_str());
        exit(EXIT_FAILURE);
    }


    // 3. Run the optimizer
    while(!engine->currentFlow()->isTerminate()) {
        engine->currentFlow()->evaluate();
        engine->currentFlow()->incrementIteration();
    }


    // 4. Get the population of solutions
    ISet* ndPop = engine->currentFlow()->finalNode()->set(0);


    // 5. Save the population to a json file
    LogManagerSPtr log = LogManagerSPtr(new LogManager());
    log->logPopulation(ndPop);
    JsonDocument jdoc(log->populationsLog());
    TString fileName;
    if(popFilesPath == nullptr) {
        fileName = TString("population.json");
    } else {
        TString sPopFilesPath(popFilesPath);
        if(sPopFilesPath.back()=='/') {
            fileName = sPopFilesPath + ("population.json");
        } else {
            fileName = sPopFilesPath + ("/population.json");
        }
    }
    Tigon::writeToFile(TString(jdoc.toJson().c_str()), fileName);

    exit(EXIT_SUCCESS);
}
