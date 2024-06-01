/* Copyright 2024 Alexander Neundorf <neundorf@kde.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <stdio.h>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <spdlog/fmt/bundled/format.h>
#include <spdlog/fmt/bundled/color.h>
#include <spdlog/fmt/bundled/printf.h>
#include <spdlog/fmt/bundled/ranges.h>

#include <unistd.h>

#include <chrono>
#include <iostream>

namespace MyLogExt
{

class MethodLog
{
  public:
    MethodLog(const char* file, int line, const char* funcName):_funcName(funcName), _fileName(file), _line(line) {

      spdlog::default_logger_raw()->log(spdlog::source_loc{_fileName, _line, _funcName}, spdlog::level::trace, "ENTERING {}...", _funcName);
    }
    ~MethodLog() {
      spdlog::default_logger_raw()->log(spdlog::source_loc{_fileName, _line, _funcName}, spdlog::level::trace, "LEAVING {}", _funcName);
      // spdlog::trace("LEAVING: {}", _funcName);
    }
  private:
    const char * const _funcName = nullptr;
    const char * const _fileName = nullptr;
    const int _line = 0;
};
}

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
#define MYTRACE_METHOD() \
MyLogExt::MethodLog myMethodLogger(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#else
 #define MYTRACE_METHOD() (void)0
#endif

#define MYTRACE_VAR1(var1) \
SPDLOG_TRACE(#var1 "={}", var1)

#define MYTRACE_VAR2(var1, var2) \
SPDLOG_TRACE(#var1 "={} " #var2 "={}", var1, var2)

#define MYTRACE_VAR3(var1, var2, var3) \
SPDLOG_TRACE(#var1 "={} " #var2 "={} " #var3 "={}", var1, var2, var3)

#define MYTRACE_LOGGER_VAR1(logger, var1) \
SPDLOG_LOGGER_TRACE(logger, #var1 "={}", var1)

#define MYTRACE_LOGGER_VAR2(logger, var1, var2) \
SPDLOG_LOGGER_TRACE(logger, #var1 "={} " #var2 "={}", var1, var2)

#define MYTRACE_LOGGER_VAR3(logger, var1, var2, var3) \
SPDLOG_LOGGER_TRACE(logger, #var1 "={} " #var2 "={} " #var3 "={}", var1, var2, var3)

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
#  define MYTRACE_PRINTF(fmtStr, ...) \
{ \
  std::string msgTmpInternal = fmt::sprintf(fmtStr, __VA_ARGS__); \
  SPDLOG_TRACE("{}", msgTmpInternal); \
}
#else
#  define MYTRACE_PRINTF(fmtStr, ...) (void)0
#endif

int foo(int f)
{
  MYTRACE_METHOD();
  SPDLOG_TRACE("");
  // spdlog::dump_backtrace();
  return 13*f;
}


std::shared_ptr<spdlog::sinks::stderr_color_sink_st> getStderrSink()
{
  static auto my_stderr_sink = std::make_shared<spdlog::sinks::stderr_color_sink_st>();
  static bool firstTime = true;
  if (firstTime)
  {
    firstTime = false;
    my_stderr_sink->set_pattern("[%H:%M:%S.%e <%^%l%$> '%n' %s:%#] %v");
  }
  return my_stderr_sink;

}

std::shared_ptr<spdlog::sinks::basic_file_sink_st> getFileSink()
{
  static auto my_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>("logs/basic-log.txt", true);
  static bool firstTime = true;
  if (firstTime)
  {
    firstTime = false;
    my_file_sink->set_pattern("[%H:%M:%S.%e <%l> '%n'] %v");
  }
  return my_file_sink;

}


void addSinks(spdlog::logger* logger)
{
  // logger->sinks().push_back(spdlog::default_logger_raw()->sinks()[0]);
  logger->sinks().push_back(getStderrSink());
  logger->sinks().push_back(getFileSink());
}


std::shared_ptr<spdlog::logger> createLogger(const char *name)
{
  std::shared_ptr<spdlog::logger> logger = spdlog::get(name);
  if (logger)
  {
    return logger;
  }
  logger = std::make_shared<spdlog::logger>(name);
  logger->set_level(spdlog::level::trace);
  logger->flush_on(spdlog::level::trace);
  addSinks(logger.get());
  spdlog::register_logger(logger);
  return logger;
}


#define DECLARE_LOGGER(loggerName) \
namespace \
{ \
  std::shared_ptr<spdlog::logger> loggerName = createLogger( #loggerName ); \
}

DECLARE_LOGGER(g_BaxLog);
DECLARE_LOGGER(g_AnotherLog);


int main()
{
  spdlog::set_level(spdlog::level::trace);

  // spdlog::enable_backtrace(100);

  spdlog::default_logger_raw()->sinks().clear();
  spdlog::default_logger_raw()->sinks().push_back(getStderrSink());
  spdlog::default_logger_raw()->sinks().push_back(getFileSink());
  spdlog::default_logger_raw()->flush_on(spdlog::level::trace);


  spdlog::logger foo_logger("foo");
  foo_logger.set_level(spdlog::level::trace);
  foo_logger.sinks().push_back( spdlog::default_logger_raw()->sinks()[0]);
  foo_logger.sinks().push_back(getFileSink());

  foo_logger.trace("I AM FOO !");

  SPDLOG_LOGGER_TRACE(g_BaxLog, "------- BAX BAX BAX");
  SPDLOG_LOGGER_TRACE(g_AnotherLog, "qwertzuio");

  spdlog::info("everything normal !!");
  spdlog::warn("Easy padding in numbers like {:08d}", 12);
  SPDLOG_TRACE("tt");
  SPDLOG_DEBUG("aa");
  SPDLOG_INFO("info !");
  SPDLOG_WARN("warning !");
  SPDLOG_ERROR("Something went wrong {} !", "XXX");
  SPDLOG_CRITICAL("Critical error !!!");
  foo(2);

  // spdlog::apply_all([](std::shared_ptr<spdlog::logger> logger) { fmt::print(fmt::color::blue, "logger: {} {}\n",
  spdlog::apply_all([](std::shared_ptr<spdlog::logger> logger) {
        fmt::print(fmt::emphasis::bold |
                   fmt::emphasis::italic |
                   fmt::emphasis::strikethrough |
                   fmt::fg(fmt::color::sky_blue),
                   "logger: {} {}\n",
                   logger->name(), (int)logger->level()); });

  spdlog::get("g_AnotherLog")->set_level(spdlog::level::debug);
  SPDLOG_LOGGER_TRACE(g_AnotherLog, "tracing is now disabled");
  SPDLOG_LOGGER_INFO(g_AnotherLog, "info still works");

#ifdef DONT
  FILE* f = fopen("test.txt", "w+");
  setlinebuf(f);
  // setvbuf(f, NULL, _IONBF, 0);
  char buf[64*1024];
  std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();
  for(int i=0; i<1000000; i++)
  {
    int lineLength = 128 + rand() % 2048;
    memset(buf, 'a', lineLength);
    buf[lineLength] = '\n';
    fwrite(buf, 1, lineLength+1, f);
    // fflush(f);
  }
  // buffered: 2.5 s
  // linebuf: 3.1 s
  // unbuf: 2.9, 3.0, ... 5.1 s
  // buffered + flush:
  fclose(f);
  std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();

  std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count() << "[ms]" << std::endl;
#endif

  std::string s2 = fmt::sprintf("hello world %d", 123);
  float f = 1.234;
  bool b = true;
  std::vector<int> vi = {6, 4, 2, 0};
  std::string s3 = fmt::format("hello vector {}", vi);

  MYTRACE_VAR1(s2);
  MYTRACE_VAR2(b, f);
  MYTRACE_LOGGER_VAR3(g_BaxLog, s2, f,vi);

  MYTRACE_PRINTF("print a number %d !", 6789);

  SPDLOG_TRACE("");

  _exit(0); // does everything end up in the log files ?

  return 0;


}
