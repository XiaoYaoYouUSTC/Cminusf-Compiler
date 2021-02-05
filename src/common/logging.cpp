#include "logging.hpp"

void LogWriter::operator<(const LogStream &stream) {
    std::ostringstream msg;
    msg << stream.sstream_->rdbuf();
    output_log(msg);
}

void LogWriter::output_log(const std::ostringstream &msg) {
    if (log_level_ >= env_log_level)
        std::cout << "[" << level2string(log_level_) << "] " 
                    << "(" <<  location_.file_ 
                    << ":" << location_.line_ 
                    << "L  "<< location_.func_<<")"
                    << msg.str() << std::endl;

}
std::string level2string(LogLevel level) {
    switch (level)
    {
        case DEBUG:
            return "DEBUG";
            
        case INFO:
            return "INFO";

        case WARNING:
            return "WARNING";

        case ERROR:
            return "ERROR";

        default:
            return "";
    }
}
std::string get_short_name(const char * file_path) {
    std::string short_file_path = file_path;
    int index = short_file_path.find_last_of('/');

    return short_file_path.substr(index+1);
}
