/*
 * Log.h
 *
 * Provides easy to use thread safe logging using operator<<
 *
 *  Created on: 25.11.2009
 *     Changed: 31.05.2012
 *      Author: david
 */

#ifndef Q0_COMMON_LOG_H_
#define Q0_COMMON_LOG_H_
//---------------------------------------------------------------------------
#include <boost/thread.hpp>
#include <iostream>
#include <string>
#include <ctime>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace LogLevels
{
	enum LogLevelType
	{
		Error=0,
		Warning=1,
		Info=2,
		Debug=3
	};
}
typedef LogLevels::LogLevelType LogLevel;

namespace detail
{

	struct Logger
	{
		Logger(std::ostream& stream)
		: level_(LogLevels::Info), stream_(stream)
		{}
		LogLevel level_;
		std::ostream& stream_;
		boost::mutex io_mutex_;
	};

	inline std::string LevelAsString(LogLevel level) {
		switch(level) {
		case LogLevels::Error:	return "ERROR  ";
		case LogLevels::Warning:return "Warning";
		case LogLevels::Info:	return "Info   ";
		case LogLevels::Debug:	return "Debug  ";
		default: 				return "";
		}
	}

	inline std::string CurrentTimeAsString() {
		time_t rawtime;
		time(&rawtime); // get time
		struct tm* timeinfo = localtime(&rawtime);
		char buffer[32];
		strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", timeinfo); // format takes 20 characters
		return std::string(buffer);
	}

	struct LogAccess
	{
		LogAccess(LogLevel level, Logger& logger, bool print_endl=true, bool print_head=true)
		: logger_(logger), print_endl_(print_endl), lock_(logger.io_mutex_) {
			if(print_head) {
				// print timestamp
				logger_.stream_ << "[ " << CurrentTimeAsString() << "  " << LevelAsString(level) << " ] ";
			}
		}

		~LogAccess() {
			if(print_endl_) {
				// print new line
				logger_.stream_ << std::endl;
			} else {
				// flush stream
				logger_.stream_ << std::flush;
			}
		}

		inline
		LogAccess& operator<<(std::ostream& (*__pf)(std::ostream&)) {
			__pf(logger_.stream_);
			return *this;
		}

		template<typename A>
		LogAccess& operator<<(const A& a) {
			logger_.stream_ << a;
			return *this;
		}

	private:
		LogAccess& operator=(const LogAccess&); // disabled

		LogAccess(const LogAccess&); // disabled

	private:
		Logger& logger_;
		bool print_endl_;
		boost::mutex::scoped_lock lock_;
	};

	inline Logger& LogSingleton() {
		static Logger logger(std::cout);
		return logger;
	}

	LogLevel& LogLevelSingleton() {
		static LogLevel level;
		return level;
	}

}

/** Gets the log level */
inline static LogLevel GetLogLevel() {
	return detail::LogLevelSingleton();
}

/** Sets the log level */
inline static void SetLogLevel(LogLevel log_level) {
	detail::LogLevelSingleton() = log_level;
}

/** Tests if the given log level is currently logged */
inline bool IsLogged(LogLevel level) {
	return level <= detail::LogLevelSingleton();
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

#define LOG_DEBUG if(Q0::IsLogged(Q0::LogLevel::Debug)) Q0::detail::LogAccess(Q0::LogLevel::Debug, Q0::detail::LogSingleton())
#define LOG_INFO if(Q0::IsLogged(Q0::LogLevel::Info)) Q0::detail::Private::LogAccess(Q0::LogLevel::Info, Q0::detail::LogSingleton())
#define LOG_WARNING if(Q0::IsLogged(Q0::LogLevel::Warning)) Q0::detail::Private::LogAccess(Q0::LogLevel::Warning, Q0::detail::LogSingleton()) << "[File=" << __FILE__ << ", Line=" << __LINE__ << ", Function=" << __FUNCTION__ << "] "
#define LOG_ERROR if(Q0::IsLogged(Q0::LogLevel::Error)) Q0::detail::Private::LogAccess(Q0::LogLevel::Error, Q0::detail::LogSingleton()) << "[File=" << __FILE__ << ", Line=" << __LINE__ << ", Function=" << __FUNCTION__ << "] "

//---------------------------------------------------------------------------
#endif
