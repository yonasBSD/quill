.. title:: Formatters

Formatters
==========

The :cpp:class:`PatternFormatter` specifies the layout of log records in the final output.

Each :cpp:class:`LoggerImpl` object has a ``PatternFormatter`` object.
This means that each Logger can be customised to output in a different format.

Customising the format output only be done during the creation of the logger via the :cpp:class:`PatternFormatterOptions`

If no custom format is set each newly created Sink uses the same formatting as the default logger.

The format output can be customised by providing a string of certain attributes.

+-------------------------+--------------------------+----------------------------------------+
| Name                    | Format                   | Description                            |
+=========================+==========================+========================================+
| time                    | %(time)                  | Human-readable timestamp when the log  |
|                         |                          | statement was issued                   |
+-------------------------+--------------------------+----------------------------------------+
| file_name               | %(file_name)             | Filename only, excluding the full path |
+-------------------------+--------------------------+----------------------------------------+
| full_path               | %(full_path)             | Full path of the source file where the |
|                         |                          | logging call was issued                |
+-------------------------+--------------------------+----------------------------------------+
| caller_function         | %(caller_function)       | Name of function containing the        |
|                         |                          | logging call                           |
+-------------------------+--------------------------+----------------------------------------+
| log_level               | %(log_level)             | Log level description                  |
|                         |                          | (‘TRACEL3’, ‘TRACEL2’, ‘TRACEL1’,      |
|                         |                          | ‘DEBUG’, ‘INFO’, ‘WARNING’, ‘ERROR’,   |
|                         |                          | ‘CRITICAL’, ‘BACKTRACE’)               |
+-------------------------+--------------------------+----------------------------------------+
| log_level_short_code    | %(log_level_short_code)  | Abbreviated log level (‘T3’, ‘T2’,     |
|                         |                          | ‘T1’, ‘D’, ‘I’, ‘W’, ‘E’, ‘C’, ‘BT’)   |
+-------------------------+--------------------------+----------------------------------------+
| line_number             | %(line_number)           | Source line number where the logging   |
|                         |                          | call was issued                        |
+-------------------------+--------------------------+----------------------------------------+
| logger                  | %(logger)                | Name of the logger                     |
+-------------------------+--------------------------+----------------------------------------+
| message                 | %(message)               | The log message                        |
+-------------------------+--------------------------+----------------------------------------+
| thread_id               | %(thread_id)             | Thread ID, if available                |
+-------------------------+--------------------------+----------------------------------------+
| thread_name             | %(thread_name)           | Thread name, if set. The name of the   |
|                         |                          | thread must be set prior to issuing    |
|                         |                          | any log statement on that thread       |
+-------------------------+--------------------------+----------------------------------------+
| process_id              | %(process_id)            | Process ID                             |
+-------------------------+--------------------------+----------------------------------------+
| source_location         | %(source_location)       | Source file path and line number       |
|                         |                          | as a single string. See                |
|                         |                          | `source_location_path_depth`           |
|                         |                          | `PatternFormatterOptions`              |
|                         |                          | to control directory depth             |
+-------------------------+--------------------------+----------------------------------------+
| short_source_location   | %(short_source_location) | Filename and line number as a single   |
|                         |                          | string formatted as filename:line,     |
|                         |                          | excluding the full file path           |
+-------------------------+--------------------------+----------------------------------------+
| tags                    | %(tags)                  | Additional custom tags appended to the |
|                         |                          | message when _TAGS macros are used     |
+-------------------------+--------------------------+----------------------------------------+
| named_args              | %(named_args)            | Key-value pairs appended to the        |
|                         |                          | message. Only applicable with          |
|                         |                          | for a named args format string;        |
|                         |                          | remains empty otherwise                |
+-------------------------+--------------------------+----------------------------------------+

Customising the timestamp
-------------------------

The timestamp is customisable by :

- Format. Same format specifiers as ``strftime(...)`` format without the additional ``.Qms`` ``.Qus`` ``.Qns`` arguments.
- Local timezone or GMT timezone. Local timezone is used by default.
- Fractional second precision. Using the additional fractional second specifiers in the timestamp format string.

========= ============
Specifier Description
========= ============
%Qms      Milliseconds
%Qus      Microseconds
%Qns      Nanoseconds
========= ============

By default ``"%H:%M:%S.%Qns"`` is used.

.. note:: MinGW does not support all ``strftime(...)`` format specifiers and you might get a ``bad alloc`` if the format specifier is not supported

Customizing Log Message Formats
-------------------------------

.. literalinclude:: examples/quill_docs_example_custom_format.cpp
   :language: cpp
   :linenos: