#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/JsonSink.h"

#include <utility>

/**
 * This example showcases the usage of the JsonFileSink to generate JSON-formatted logs.
 * Additionally, it demonstrates how to simultaneously log in both the standard logger output
 * format, e.g., to console and the corresponding JSON format to a JSON output sink.
 *
 * For successful JSON logging, it's essential to use named placeholders within the provided
 * format string, such as "{method}" and "{endpoint}".
 */

int main()
{
  // Start the backend thread
  quill::BackendOptions backend_options;
  quill::Backend::start(backend_options);

  // Frontend

  // Create a json file for output
  auto json_sink = quill::Frontend::create_or_get_sink<quill::JsonFileSink>(
    "example_json.log",
    []()
    {
      quill::FileSinkConfig cfg;
      cfg.set_open_mode('w');
      cfg.set_filename_append_option(quill::FilenameAppendOption::None);
      return cfg;
    }(),
    quill::FileEventNotifier{});

  // When using the JsonFileSink, it is ideal to set the logging pattern to empty to avoid unnecessary message formatting.
  quill::Logger* json_logger = quill::Frontend::create_or_get_logger(
    "json_logger", std::move(json_sink),
    quill::PatternFormatterOptions{"", "%H:%M:%S.%Qns", quill::Timezone::GmtTime});

  for (int i = 0; i < 2; ++i)
  {
    LOG_INFO(json_logger, "{method} to {endpoint} took {elapsed} ms", "POST", "http://", 10 * i);
  }

  // It is also possible to create a logger than logs to both the json file and stdout
  // with the appropriate format
  auto json_sink_2 = quill::Frontend::get_sink("example_json.log");
  auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("console_sink_id_1");

  // Define a custom format pattern for console logging, which includes named arguments in the output.
  // If you prefer to omit named arguments from the log messages, you can remove the "[%(named_args)]" part.
  quill::PatternFormatterOptions console_log_pattern = quill::PatternFormatterOptions{
    "%(time) [%(thread_id)] %(short_source_location:<28) LOG_%(log_level:<9) %(logger:<20) "
    "%(message) [%(named_args)]"};

  // Create a logger named "hybrid_logger" that writes to both a JSON sink and a console sink.
  // Note: The JSON sink uses its own internal format, so the custom format defined here
  // will only apply to the console output (via console_sink).
  quill::Logger* hybrid_logger = quill::Frontend::create_or_get_logger(
    "hybrid_logger", {std::move(json_sink_2), std::move(console_sink)}, console_log_pattern);

  for (int i = 2; i < 4; ++i)
  {
    LOG_INFO(hybrid_logger, "{method} to {endpoint} took {elapsed} ms", "POST", "http://", 10 * i);
  }

  LOG_INFO(hybrid_logger, "Operation {name} completed with code {code}", "Update", 123,
           "Data synced successfully");
}
