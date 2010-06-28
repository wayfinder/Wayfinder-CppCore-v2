/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <stdlib.h>
#include <iostream>
#include <argp.h>
#include <cstring>
#include "nav2util.h"
#include <signal.h>
#include "GuiProt/GuiProtEnums.h"
#include "Nav2.h"
#include "NavServerComEnums.h"
#include "ErrorAndSyntax.h"

//#define LOCK_3RDPARTY

using namespace isab;
using namespace GuiProtEnums;
using namespace std;
namespace {
   /** Program version string. */
   /** Program bug report address. */
   const char* argp_program_bug_address = "$Author: daniel $";
   /** Program self-description string. */
   char doc[] = "The linux wayfinder app. No GUI yet...";
   //static char args_doc[] = "";
   /** Program argument list. */
   struct argp_option options[] = {
      {0,0,0,0, "Server options."},
#ifndef LOCK_3RDPARTY
      {"navigatorserver", '@', "host:port", 0, 
       "The NavigatorServer to connect to."},
      {"fake", 'k', 0, 0, 
       "Use FakeNavServerCom. All Server requests will return fix results."},
#endif
      {"http", 'z', "useragent", OPTION_ARG_OPTIONAL, 
       "Use HTTP protocol. Optionally specify USER-AGENT string."},
      {0,0,0,0, "Sets the connections of interface modules."},
      {"gui", 'g' , "[host:]port", 0, 
       "Open the GUI connection at the specified port. "
       "Default argument is 11112."},
      {"simulator", 's', "[host:]port    ", 0, 
       "Open a simulator connection at the specified port. "
       "Default argument is '4449'."},
#ifndef LOCK_3RDPARTY
      {"simtype", 'S', "n[mea] | t[rimble]", 0, 
       "Sets the simulation source type. Default is nmea."},
      {"shell",     'l', "[host:]port", 0,
       "Open a TestShell connection at the specified port. Default 1234."}, 
      {"regression-test", 'q', 0, 0, 
       "Tells TestShell to do regression test output."},
#endif
      {0,0,0,0, "Set application information to send to the server."},
      {"hardware", 'H', "hardware-id-string", 0, 
       "Hardware serial number. Mobile phones use IMEI."},
      {"language", 'L', "language-code   ", 0, 
       "Language used in client-server communication. Valid codes are: "
       "EN, SV, DE, DA, FI, NO, IT, NL, ES, FR, "/*"CY"*/ "PT, "
       "CS, US, HU, EL, PL, SL" },
      {"wayfindertype", 'w', "t[rial] | s[ilver] | g[old] | i[ron]", 0, 
       "Sets the Wayfinder Application access level. "
       "Default picks the value from the parameter file."},
#ifndef LOCK_3RDPARTY
      {"applicationversion", 'a', "major.minor.build", 0, 
       "Sets the application version information."},
      {"resourcesversion", 'v', "major.minor.build", 0, 
       "Sets the resources version information."},
      {"builder",   'u', "username",0,
       "The user that built this application."},
      {"buildhost", 'h', "hostname",0,
       "The host this application was built on."},
      {"buildtime", 'b', "string",0,
       "Valid date string that specifies the time this binary was built."},
      {"clienttype", 'c', "string", 0, "A client type string."},
      {"clientoptions", 'o', "string", 0, "A client type options string."},
#endif
      {0,0,0,0, "Use at most one of the log options to control where the "
                "log output should go."},
      {"logfile",   'f', "file", 0, "Log output to the specified file"},
      {"logtcp",    't', "[host:]port", 0, "Log output to a tcp connection"},
      {"lognull",   'n', 0, 0, "No log output"},
      {"logstream", 'r', "cerr|cout|clog ", 0, 
       "Log output to the specified stream."},
#ifndef LOCK_3RDPARTY
      {0,0,0,0, "Options for the argument parsing system."},
      {"test", '#', 0,0, "Parse arguments, print the results and then exit."},
#endif
      {0}
   };
   /** This struct stores the parsed argument data for later.*/
   struct arguments{
      /** The host for simulated data.*/
      char* simHost;
      /** The port for simulated data.*/
      uint16 simPort;
      /** The TestShell host.*/
      char* shellHost;
      /** The TestShell port. */
      uint16 shellPort;
      /** The target string to set as default log output.*/
      char* logString;
      char* server;
      uint16 serverPort;
      char* guiHost;
      uint16 guiPort;
     
      uint32 appVer[3];
      uint32 resVer[3];
      char* builder;
      char* buildHost;
      char* buildTime;
      char* clientType;
      char* clientTypeOptions;

      bool argTest;
      bool regressionTest;
      bool fakeNavServerCom;
      bool http;
      char* userAgent;
      char* simfile;

      enum WayfinderType wfType;
      enum Nav2StartupData::GpsProtocol gpsProto;

      char* hardwareid;

      int language;
      /** Constructor. Sets all fields to 0 or NULL.*/
      arguments() : 
         simHost(NULL), simPort(4449), 
         shellHost(NULL), shellPort(1234),
         logString(NULL), server(NULL), serverPort(0), 
         guiHost(NULL), guiPort(11112), 
         builder(NULL), buildHost(NULL), buildTime(NULL), 
         clientType(NULL), clientTypeOptions(NULL), 
         argTest(false),
         regressionTest(false), fakeNavServerCom(false), 
         http(false), userAgent(NULL),
         simfile(NULL), wfType(InvalidWayfinderType), 
         gpsProto(Nav2StartupData::NMEA),
         hardwareid(NULL),
         language(isab::NavServerComEnums::ENGLISH)
      {
         appVer[0] = 5;
         appVer[1] = 1;
         appVer[2] = 0;
         resVer[0] = 4;
         resVer[1] = 10;
         resVer[2] = 0;
#ifndef LOCK_3RDPARTY
         clientType = strdup_new("wf-linux-demo");
#else
         clientType = strdup_new("wf-linux-demo");
         clientTypeOptions = strdup_new("3rdparty");
#endif
      }

      /** Destructor. Deletes any string at logString.*/
      ~arguments() 
      {
         delete[] logString;
         delete[] clientTypeOptions;
         delete[] clientType;
         delete[] userAgent;
      }

      operator Nav2StartupData() const
      {
         Nav2StartupData ret(wfType, "./", "./", "./", NULL, NULL);
         ret.setSimulation(simHost, simPort);
         ret.setSimulateFromFile(simfile);
         ret.setTestShell(shellHost, shellPort);
         ret.setGui(guiHost, guiPort);
         ret.setExtraServer(server, serverPort);
         ret.setProgramVersion(appVer[0], appVer[1], appVer[2]);
         ret.setResourceVersion(resVer[0], resVer[1], resVer[2]);
         ret.debugOutput          = logString;
         ret.fakeNavServerCom     = fakeNavServerCom;
         ret.testshellRegTest     = regressionTest;
         ret.gpsProtocol          = gpsProto;
         ret.language             = language;
         if(clientType){
            ret.clientType        = clientType;
         }
         if(clientTypeOptions){
            ret.clientTypeOptions = clientTypeOptions;
         }
         ret.serialNumber         = hardwareid;
#ifndef LOCK_3RDPARTY
#else
         ret.superLogLevels       = 0x06; //info, error, warning
#endif
         ret.httpProto            = http;
         ret.httpUserAgent        = userAgent;
         ret.useTracking          = true;

         return ret;
      }
   };

#define NONULL(arg) (arg ? (const char*)arg : (const char*)"(null)")

   std::ostream& operator<<(std::ostream& out, arguments& data){
      using namespace std;
      out << "SimulatorData:    " << NONULL(data.simHost) << " : " 
          << data.simPort << endl
          << "ShellConnection:  " << NONULL(data.shellHost) << " : " 
          << data.shellPort << endl 
          << "Server:           " << NONULL(data.server) << " : "
          << data.serverPort << endl 
          << "GuiConnection:    " << NONULL(data.guiHost) << " : " 
          << data.guiPort << endl
          << "LogOutput:        " << NONULL(data.logString) << endl 
          << "AppVer:           " << data.appVer[0] << "." << data.appVer[1] 
          << "." << data.appVer[2] << endl 
          << "ResVer:           " << data.resVer[0] << "." << data.resVer[1] 
          << "." << data.resVer[2] 
          << endl 
          << "Builder:          " << NONULL(data.builder) << endl 
          << "BuildHost:        " << NONULL(data.buildHost) << endl 
          << "BuildTime:        " << NONULL(data.buildTime) << endl
          << "ClientType:       " << NONULL(data.clientType) << endl
          << "ClientTypeOptions:" << NONULL(data.clientTypeOptions) << endl
          << "Argument Test:    " << data.argTest << endl
          << "Regression Test:  " << data.regressionTest << endl
          << "FakeNavServerCom: " << data.fakeNavServerCom << endl
          << "HTTP:             " << NONULL(data.http) << endl
          << "USER-AGENT:       " << NONULL(data.userAgent) << endl
          << "Simdata File:     " << NONULL(data.simfile) << endl
          << "SimProto:         " << ((data.gpsProto == Nav2StartupData::Trimble) ? "Trimble" :
                                       ((data.gpsProto == Nav2StartupData::NMEA) ? "NMEA" :
                                         "Invalid selection!!!")) << endl
          << "WayfinderType:    " << ((data.wfType == Trial) ? "Trial" :
                                      ((data.wfType == Silver) ? "Silver" : 
                                       ((data.wfType == Gold) ? "Gold" :
                                        ((data.wfType == Iron) ? "Iron" : 
                                         "Invalid")))) << endl
          << "Language:         " << data.language << endl
          << "Hardware ID:      " << NONULL(data.hardwareid) << endl;
      return out;
   }


   /** The argp compatible parser. This function may not always return.
    * @param key   the short option letter.
    * @param arg   the argument string.
    * @param state the argp state.
    * @return 0 if the option was handled ARGP_ERR_UNKOWN if it wasn't.*/
   error_t parse_opt(int key, char* arg, struct argp_state* state)
   {
      struct arguments *args = static_cast<arguments*>(state->input);
      char* tmp;
      if(strchr("tfnr", key) != NULL && args->logString){
         argp_error(state, "Specify at most one of the options 'logfile',"
                    " 'logtcp', 'lognull', and 'logstream'.");
      }
      switch(key){
      case 'g':
         if(!parseHost(arg, &(args->guiHost), &(args->guiPort))){
            argp_error(state, "Argument '%s' doesn't parse.", arg);
         }
         break;
      case 's':
         if(! parseHost(arg, &(args->simHost), &(args->simPort))){
            argp_error(state, "Argument '%s' doesn't parse.", arg);
         }
         break;
      case 'z':
         args->http = true;
         if(arg){//optional use-agent string specified
            args->userAgent = strdup_new(arg);
         }
         break;
#ifndef LOCK_3RDPARTY
      case '@':
         if(!parseHost(arg, &(args->server), &(args->serverPort))){
            argp_error(state, "Argument '%s' doesn't parse.", arg);
         } else if(args->server == NULL){
            argp_error(state, "Option 'navigatorserver' requires a hostname.");
         }
         break;
      case 'k':
         args->fakeNavServerCom = true;
         break;
      case 'l':
         if(! parseHost(arg, &(args->shellHost), &(args->shellPort))){
            argp_error(state, "Argument '%s' doesn't parse.", arg);
         }
         break;
#endif
      case 't':
         {
            char* logHost = NULL;
            uint16 logPort = 0;
            if(!parseHost(arg, &logHost, &logPort)){
               argp_error(state, "Argument '%s' doesn't parse.", arg); 
            }
            tmp = new char[256];
            snprintf(tmp, 256, "tcp:%s%s%u", (logHost ? logHost : ""),
                     (logHost ? ":" : ""), logPort);
            args->logString = tmp;
         }
         break;
      case 'f':
         tmp = new char[strlen(arg) + 10];
         sprintf(tmp, "file:%s", arg);
         args->logString = tmp;
         break;
      case 'n':
         tmp = new char[8];
         sprintf(tmp, "null");
         args->logString = tmp;
         break;
      case 'r':
         if(strequ("cerr", arg) || strequ("cout", arg) || strequ("clog", arg)){
            args->logString = strdup_new(arg);
         } else {
            argp_error(state, "%s doesn't match 'cerr', 'cout', or 'clog'.", 
                       arg);
         }
         break;
#ifndef LOCK_3RDPARTY
      case 'a':
         if(!parseVersion(args->appVer, arg)){
            argp_error(state, "Argument %s doesn't parse.", arg);
         }
         break;
      case 'v':
         if(!parseVersion(args->resVer, arg)){
            argp_error(state, "Argument %s doesn't parse.", arg);
         }
         break;
#endif
      case 'w':
         {
            char* tmp = strdup_new(arg);
            size_t len = strlen(tmp);
            for(unsigned i = 0; i < len; ++i){
               tmp[i] = tolower(tmp[i]);
            }
            args->wfType = InvalidWayfinderType;
            switch(tmp[0]){
            case 'a': //just for the hell of it....
               if(strequ("au", tmp)) args->wfType = Gold;
               if(strequ("ag", tmp)) args->wfType = Silver;
               break;
            case 'f':
               if(strequ("fe", tmp)) args->wfType = Iron;
               break;
            case 'g': 
               if((len == 1) || strequ("gold", tmp))   args->wfType = Gold;
               break;
            case 's': 
               if((len == 1) || strequ("silver", tmp)) args->wfType = Silver;
               break;
            case 't':
            case 'l':
               if((len == 1) || strequ("trial", tmp) || 
                                strequ("light", tmp))  args->wfType = Trial;
               break;
            case 'i':
            case 'e':
               if((len == 1) || strequ("iron", tmp) || 
                                strequ("earth", tmp))  args->wfType = Iron;
               break;
            default:
               break;
            }
            if(args->wfType == InvalidWayfinderType){
               argp_error(state,
                          "'%s' is not a valid value for 'wayfindertype'.",
                          arg);
               args->wfType = Gold;
            }
            delete[] tmp ;
         }
         break;
#ifndef LOCK_3RDPARTY
      case 'S':
         {
            char* tmp = strdup_new(arg);
            size_t len = strlen(tmp);
            for(unsigned i = 0; i < len; ++i){
               tmp[i] = tolower(tmp[i]);
            }
            switch(tmp[0]){
            case 't': 
               if((len == 1) || strequ("trimble", tmp))   args->gpsProto = Nav2StartupData::Trimble;
               break;
            case 'n': 
               if((len == 1) || strequ("nmea", tmp)) args->gpsProto = Nav2StartupData::NMEA;
               break;
            default:
               argp_error(state,
                          "'%s' is not a valid value for 'gps protocol'.",
                          arg);
               break;
            }
            delete[] tmp ;
         }
         break;
#endif
      case 'L':
         {
            const char* codes[] = {"EN", "SV", "DE", "DA", "FI", "NO", "IT", 
                                   "NL", "ES", "FR", "CY", "PT", "CS", "US",
                                   "HU", "EL", "PL", "SL" /* RUSSIAN */};
            int i = 0;
            int end = sizeof(codes)/sizeof(*codes);
            while(i < end && (!arg || !codes[i] || !strequ(codes[i], arg))){
               ++i;
            }
            if(i < end){
               args->language = i;
            };
         }
         break;
      case 'H':     args->hardwareid = arg;                    break;
#ifndef LOCK_3RDPARTY
      case 'b':     args->buildTime  = arg;                    break;
      case 'u':     args->builder    = arg;                    break;
      case 'h':     args->buildHost  = arg;                    break;
      case 'c':     args->clientType        = strdup_new(arg); break;
      case 'o':     args->clientTypeOptions = strdup_new(arg); break;
      case '#':     args->argTest        = true;               break;
      case 'q':     args->regressionTest = true;               break;
#endif
      case ARGP_KEY_ARG: argp_usage(state);             break;
      case ARGP_KEY_END:                                break;
      default:           return ARGP_ERR_UNKNOWN;
      }
      return 0;
   }
   
   /** The argp data struct.*/
   struct argp my_argp ={ options, parse_opt, "", doc};
}

void signalHandler( int iMsg )
{
   //   printf("Wayfinder will now exit because of message %u\n", iMsg);
   if (iMsg == 2) {
      exit(0);
   }
}


int main(int argc, char* argv[]){
   signal(SIGINT, signalHandler);

   struct arguments args;
   argp_parse(&my_argp, argc, argv, 0, 0, &args);
#ifndef LOCK_3RDPARTY
   std::cerr << args;

   if(args.argTest){
      std::cerr << "Argument testing complete ... Exiting." << std::endl;
      exit(EXIT_FAILURE);
   }
#endif

   class isab::Nav2StartupData data = args;

   if(!ErrorAndSyntax(NavServerComEnums::languageCode(args.language), 
                      data.errorTable, data.audioSyntax)){    
      std::cerr << "Language " << args.language 
                << " is not supported yet!" << std::endl;
      exit(EXIT_FAILURE);
   }

#ifndef LOCK_3RDPARTY
   class isab::Nav2 * nav2 = new isab::Nav2Develop(data);
#else
   class isab::Nav2 * nav2 = new isab::Nav2Release(data);
#endif
   nav2->WaitNavigate2();
   delete nav2;

   exit(EXIT_SUCCESS);
}
