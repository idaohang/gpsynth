#ifndef GPLOGGER_H
#define GPLOGGER_H

#include <string>
#include <streambuf>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iosfwd>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include "../Synth/GPNetwork.h"

struct GPLogParams {
    bool to_file;
    bool to_cout;
    bool to_cerr;
};

// log stream
class GPLog : public std::streambuf {
    public:
        explicit GPLog(GPLogParams* params, std::string output_file_path, std::size_t buff_sz = 2048);
        ~GPLog();

        void set_forward(std::ostream* f);

    protected:
        bool forward_and_flush();

    private:
        GPLog::int_type overflow(int_type ch);
        int sync();

        // copy ctor and assignment not implemented;
        // copying not allowed
        GPLog(const GPLog &);
        GPLog &operator= (const GPLog &);

        GPLogParams* params;
        bool do_forward;
        std::ostream* forward;
        std::ofstream log_file_stream;
        std::stringstream sink_;
        std::vector<char> buffer_;
};

// params for log stream
struct GPLoggerParams {
    GPLogParams* log_params;
    GPLogParams* verbose_params;
    GPLogParams* debug_params;
    GPLogParams* error_params;
    unsigned log_precision;
    unsigned save_net_precision;
    unsigned print_net_precision;
    bool verbose_to_log;
};

class GPLogger {
    public:    
        GPLogger(GPLoggerParams* params, std::string seed_string, std::string output_dir_path);
        ~GPLogger();

        std::string get_seed_string();
        std::string get_system_info();

        std::string net_to_string_print(GPNetwork* net);
        std::string net_to_string_save(GPNetwork* net);
        std::string param_to_string_print(GPMutatableParam* param);

        std::ostream log;
        std::ostream verbose;
        std::ostream debug;
        std::ostream error;

    private:
        GPLoggerParams* params;
        std::string seed_string;

        GPLog log_buff;
        GPLog verbose_buff;
        GPLog debug_buff;
        GPLog error_buff;
};

#endif
