#pragma once

//--------------------------------------------------------------------------------

#include <format>
#include <iostream>
#include <fstream>
#include <chrono>
#include <source_location>

//--------------------------------------------------------------------------------

namespace ak_logger
{

//--------------------------------------------------------------------------------

enum class LogMode
{
    kDEBUG,
    kINFO,
    kWARNING,
    kERROR,
    kTRACE
};

//--------------------------------------------------------------------------------

const std::string kLogFileName = ".log";

//--------------------------------------------------------------------------------

class Logger
{
private:
    std::ofstream log_file_;

    //==================================================
    
    Logger (const std::string& file_name) : log_file_(file_name, std::ios::app)
    {
        if (!log_file_.is_open ()) {
            std::cerr << "Failed to open a log_file " << file_name << std::endl;
        }
    }

    //==================================================

    ~Logger ()
    {
        if (log_file_.is_open ()) {
            log_file_.close ();
        }
    }

    //==================================================

    std::string GetLogModeString (LogMode mode)
    {
        switch (mode)
        {
        case LogMode::kDEBUG:   return "DEBUG";
        case LogMode::kINFO:    return "INFO";
        case LogMode::kWARNING: return "WARNING";
        case LogMode::kERROR:   return "ERROR";
        case LogMode::kTRACE:   return "TRACE";
        default:                return "UNKNOWN";
        }
        
        return "UNKNOWN";
    }

    //==================================================

    std::string GetCurrentTime ()
    {
        auto time = std::chrono::system_clock::now ();

        auto time_in_seconds = std::chrono::floor<std::chrono::seconds>(time);

        return std::format ("{:%T}", time_in_seconds);
    }

//————————————————————————————————————————————————————————————————————————————————

public:
    // Make a singleton class
    // delete copy and assign operator
    Logger (const Logger&) = delete;
    Logger& operator= (const Logger&) = delete;

    //==================================================

    static Logger& GetInstance ()
    {
        static Logger instance (kLogFileName);
        return instance;
    }

    //==================================================

    template <typename... Args>
    void Log (std::source_location location,
              const char*          location_file_name, 
              LogMode              mode,
              std::string_view     format, 
              Args&&...            args)
    {
        std::string location_line = std::format (
            "{}:{}:{} | {}",
            location_file_name,
            location.line (),
            location.column (),
            location.function_name ()
        );

        std::string message = std::vformat (format, std::make_format_args (args...));

        std::string log_line = std::format (
            "[{}] [{}] [{:<7}] [{}]\n",
            location_line,
            GetCurrentTime (),
            GetLogModeString (mode),
            message
        );

        if (!log_file_.is_open ()) {
            return;
        }

        log_file_ << log_line;
        log_file_.flush ();
    }

    //==================================================

#if (!defined(NDEBUG) && defined(LOGGING))
    #define LOG_(log_mode, fmt, ...) ak_logger::Logger::GetInstance().Log(std::source_location::current(), __FILE__, (log_mode), (fmt), ##__VA_ARGS__);
    #define LOG_DEBUG_(fmt, ...) LOG_(ak_logger::LogMode::kDEBUG, (fmt), ##__VA_ARGS__);
    #define LOG_INFO_(fmt, ...) LOG_(ak_logger::LogMode::kINFO, (fmt), ##__VA_ARGS__);
    #define LOG_WARNING_(fmt, ...) LOG_(ak_logger::LogMode::kWARNING, (fmt), ##__VA_ARGS__);
    #define LOG_ERROR_(fmt, ...) LOG_(ak_logger::LogMode::kERROR, (fmt), ##__VA_ARGS__);
    #define LOG_TRACE_(fmt, ...) LOG_(ak_logger::LogMode::kTRACE, (fmt), ##__VA_ARGS__);
#else
    #define LOG_(log_mode, fmt, ...) ((void)0)
    #define LOG_DEBUG_(fmt, ...) ((void)0)
    #define LOG_INFO_(fmt, ...) ((void)0)
    #define LOG_WARNING_(fmt, ...) ((void)0)
    #define LOG_ERROR_(fmt, ...) ((void)0)
    #define LOG_TRACE_(fmt, ...) ((void)0)
#endif // NDEBUG

};  // class Logger

//--------------------------------------------------------------------------------

}; // namespace ak_logger

//--------------------------------------------------------------------------------