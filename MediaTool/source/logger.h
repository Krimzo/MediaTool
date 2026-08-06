#pragma once

#include "general.h"


namespace mt
{
struct LogInfo
{
    ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    std::string message;
    std::string date = kl::format( kl::Date() );

    LogInfo()
    {}

    LogInfo( std::string message )
        : message( std::move( message ) )
    {}

    template<typename... Args>
    LogInfo( kl::Float4 const& color, Args&&... objects )
        : color( reinterpret_cast<ImVec4 const&>( color ) )
        , message( kl::format( objects... ) )
    {}
};

struct Logger
{
    static constexpr int MAX_SIZE = 250;

    template<typename... Args>
    static void log( kl::Float4 const& color, Args&&... objects )
    {
        std::lock_guard logs_lock{ C_LOGS_MUTEX };
        C_LOGS.emplace_back( color, objects... );
        ++C_LAST_LOG_INDEX;
        while ( C_LOGS.size() > MAX_SIZE )
            C_LOGS.pop_front();
    }

    static int last_log_index()
    {
        return C_LAST_LOG_INDEX;
    }

    static std::list<LogInfo> const& logs()
    {
        return C_LOGS;
    }

    static void clear()
    {
        C_LOGS.clear();
    }

private:
    static inline int C_LAST_LOG_INDEX = 0;
    static inline std::list<LogInfo> C_LOGS = {};
    static inline std::mutex C_LOGS_MUTEX = {};
};
}
