.. title:: Cheat Sheet

Cheat Sheet
===========

Introduction
------------

The logging library provides macros to efficiently log various types of data. The ``LOG_`` macro copies each argument passed into contiguous pre-allocated memory. On the invoking thread, only a binary copy of the arguments is taken. Formatting, string conversions, and IO are handled by the backend logging thread.

For instance, when logging a ``std::vector``, no copy constructor is invoked. Instead, the vector elements are serialized into pre-allocated memory. To serialize complex types, the library needs explicit template specializations within the ``quill`` namespace, and formatter specializations required by ``libfmt`` within the ``fmtquill`` namespace.

In addition to the ``LOG_`` macros, ``LOGV_`` macros are available and convenient.

Quill uses ``libfmt`` for formatting, supporting the ``{}`` placeholders syntax (https://fmt.dev/latest/syntax). However, ``LOGV_`` macros do not support this syntax and are hardcoded, accepting up to 20 arguments.

The library minimizes unnecessary header inclusions; therefore, to log STL types, you must explicitly include headers from the ``quill/std/`` folder.

Built-in Types and Strings
--------------------------

Logging ``arithmetic types``, ``strings``, ``string_view``, ``C strings``, ``C char arrays``, or ``void const*`` is supported out of the box.

.. code:: cpp

    double a = 123.4567;
    LOG_INFO(logger, "number {:.2f}", a);
    LOGV_INFO(logger, "number", a);

Outputs:

    number 123.46

    number [a: 123.4567]

Logging Arithmetic C-style Arrays
---------------------------------

This functionality is supported by including ``quill/std/Array.h``

.. code:: cpp

    #include "quill/std/Array.h"

    int a[3] = {1, 2, 3};
    LOG_INFO(logger, "array {}", a);
    LOGV_INFO(logger, "array", a);

Outputs:

    array [1, 2, 3]

    array [a: [1, 2, 3]]

Logging Enums
-------------
For enums, you can either cast them to their underlying type or provide an ``operator<<`` or an ``fmtquill::formatter``

.. code:: cpp

    #include "quill/bundled/fmt/format.h"
    #include "quill/bundled/fmt/ostream.h"

    enum class Side
    {
      BUY,
      SELL
    };

    std::ostream& operator<<(std::ostream& os, Side s)
    {
      if (s == Side::BUY)
      {
        os << "BUY";
      }
      else
      {
        os << "SELL";
      }
      return os;
    }

    Side s {Side::SELL};
    LOG_INFO(logger, "Side {}", s);
    LOGV_INFO(logger, "Side", s);

Outputs:

    Side SELL

    Side [s: SELL]

Logging Strings Without Additional Copy
---------------------------------------
By default, the logger takes a deep copy of any string. To log an immutable string with a valid lifetime without copying, use ``quill::utility::StringRef``.

.. code:: cpp

    #include "quill/StringRef.h"

    static constexpr std::string_view s {"Test String"};
    LOG_INFO(logger, "The answer is {}", quill::utility::StringRef {s});

    auto sref = quill::utility::StringRef {s};
    LOGV_INFO(logger, "The answer is", sref);

Outputs:

    The answer is Test String

    The answer is [sref: Test String]

Logging STL Library Types
-------------------------
To log STL types, include the relevant header from ``quill/std/`` and pass the type to the logger. There is support for most ``STL`` types.

.. code:: cpp

    #include "quill/std/Vector.h"

    std::vector<std::string> v1 {"One", "Two", "Three"};
    std::vector<std::string> v2 {"Four", "Five", "Six"};
    std::vector<std::vector<std::string>> vv {v1, v2};

    LOG_INFO(logger, "Two vectors {} {} and a vector of vectors {}", v1, v2, vv);
    LOGV_INFO(logger, "Two vectors and a vector of vectors", v1, v2, vv);

Outputs:

    Two vectors ["One", "Two", "Three"] ["Four", "Five", "Six"] and a vector of vectors [["One", "Two", "Three"], ["Four", "Five", "Six"]]

    Two vectors and a vector of vectors [v1: ["One", "Two", "Three"], v2: ["Four", "Five", "Six"], vv: [["One", "Two", "Three"], ["Four", "Five", "Six"]]]

Logging Nested STL Library Types
--------------------------------
Logging nested STL types is supported. Include all relevant files from ``quill/std/``.

For example, to log a ``std::vector`` of ``std::pair``, include both ``quill/std/Vector.h`` and ``quill/std/Pair.h``.

.. code:: cpp

    #include "quill/std/Vector.h"
    #include "quill/std/Pair.h"

    std::vector<std::pair<int, std::string>> v1 {{1, "One"}, {2, "Two"}, {3, "Three"}};
    LOG_INFO(logger, "Vector {}", v1);
    LOGV_INFO(logger, "Vector", v1);

Outputs:

    Vector [(1, "One"), (2, "Two"), (3, "Three")]

    Vector [v1: [(1, "One"), (2, "Two"), (3, "Three")]]

Logging Wide Strings
--------------------
On Windows, wide strings are supported by including ``quill/std/WideString.h``.

.. code:: cpp
    #include "quill/std/WideString.h"
    #include "quill/std/Vector.h"

    std::wstring w {L"wide"};
    std::vector<std::wstring> wv {L"wide", L"string"};
    LOG_INFO(logger, "string {} and vector {}", w, wv);
    LOGV_INFO(logger, "string and vector", w, wv);

Outputs:

    string wide and vector ["wide", "string"]

    string and vector [w: wide, wv: ["wide", "string"]]

Logging User Defined Types
--------------------------

To log user-defined types, you must tell the library how to serialize them or convert them to a string and pass the string to the logger.

For log statements during program initialization, or for debug log statements not on the critical path, it is recommended to convert user-defined types to strings and pass a string to the ``LOG_`` function. This approach requires less work and reduces template instantiations. For example

.. code:: cpp
    class Config
    {
      public:
      std::string param_1;
      std::string param_2;

      friend std::ostream& operator<<(std::ostream& os, Config config)
      {
        os << "param_1: " << config.param_1 << " param_2 " << config.param_2;
        return os;
      }
    };

    template <>
    struct fmtquill::formatter<Config> : fmtquill::ostream_formatter
    {
    };

    Config cfg {"123", "456"};

    LOG_INFO(logger, "Starting with config {}", fmtquill::format("{}", cfg));

    std::string const cfg_str = fmtquill::format("{}", cfg);
    LOGV_INFO(logger, "Starting", cfg_str);

Outputs:

    Starting with config param_1: 123 param_2 456

    Starting [cfg_str: param_1: 123 param_2 456]

For log statements on the critical path, it is advised to provide serialization methods so that only a binary copy is taken on the critical path. You need to provide the required ``formatter`` specializations for ``libfmt`` to format the types and the ``Codec`` for the quill library. The type will be encoded on the critical path, then decoded and recreated on the backend thread, and finally passed to ``libfmt`` for formatting.

Serializing Trivially Copyable Types With Public Members
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Use ``quill::TriviallyCopyableTypeCodec`` helper. Note that a default constructor is required.

.. code:: cpp

    #include "quill/TriviallyCopyableCodec.h"

    struct Order
    {
      char symbol[32];
      double price;
      int quantity;

      friend std::ostream& operator<<(std::ostream& os, Order const& order)
      {
        os << "symbol=" << order.symbol << " price=" << order.price << " quantity=" << order.quantity;
        return os;
      }
    };

    template <>
    struct fmtquill::formatter<Order> : fmtquill::ostream_formatter
    {

    };

    template <>
    struct quill::Codec<Order> : quill::TriviallyCopyableTypeCodec<Order>
    {

    };

    Order order;
    strcpy(order.symbol, "AAPL");
    order.quantity = 100;
    order.price = 220.10;

    LOG_INFO(logger, "Order is {}", order);
    LOGV_INFO(logger, "Order", order);

Outputs:

    Order is symbol=AAPL price=220.1 quantity=100

    Order [order: symbol=AAPL price=220.1 quantity=100]

Serializing Trivially Copyable Types With Private Members
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For types with private members, make ``quill::TriviallyCopyableTypeCodec<T>`` a friend and add a private default constructor if a public one does not exist.

.. code:: cpp

    #include "quill/TriviallyCopyableCodec.h"

    class Order
    {
    public:
      Order(double price, int quantity)
        : timestamp(std::chrono::system_clock::now().time_since_epoch().count()), price(price), quantity(quantity)
      {
      }

    private:
      uint64_t timestamp;
      double price;
      int quantity;

      template <typename T>
      friend struct quill::TriviallyCopyableTypeCodec;

      Order() = default;

      friend std::ostream& operator<<(std::ostream& os, Order const& order)
      {
        os << "timestamp=" << order.timestamp << " price=" << order.price << " quantity=" << order.quantity;
        return os;
      }
    };

    template <>
    struct fmtquill::formatter<Order> : fmtquill::ostream_formatter
    {
    };

    template <>
    struct quill::Codec<Order> : quill::TriviallyCopyableTypeCodec<Order>
    {
    };

    int main()
    {
      quill::BackendOptions backend_options;
      quill::Backend::start(backend_options);

      // Frontend
      auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1");
      quill::Logger* logger = quill::Frontend::create_or_get_logger("root", std::move(console_sink));

      // Change the LogLevel to print everything
      logger->set_log_level(quill::LogLevel::TraceL3);

      Order order {220.10, 100};

      LOG_INFO(logger, "Order is {}", order);
      LOGV_INFO(logger, "Order", order);
    }

Outputs:

    Order is timestamp=17220422717461192 price=220.1 quantity=100

    Order [order: timestamp=17220422717461192 price=220.1 quantity=100]

Logging Complex User Defined Types With Public Members
------------------------------------------------------
For user defined types with complex types as members we need to define ``quill::Codec<T>``. Note that it is possible to pass STL types to ``compute_total_encoded_size``, ``encode_members``, ``decode_members`` as long as you have included the relevant header file from ``quill/std/`` for that type.

.. code:: cpp

    #include "quill/core/Codec.h"
    #include "quill/core/DynamicFormatArgStore.h"

    struct Order
    {
      std::string symbol;
      double price;
      int quantity;

      friend std::ostream& operator<<(std::ostream& os, Order const& order)
      {
        os << "symbol=" << order.symbol << " price=" << order.price << " quantity=" << order.quantity;
        return os;
      }
    };

    template <>
    struct fmtquill::formatter<Order> : fmtquill::ostream_formatter
    {
    };

    template <>
    struct quill::Codec<Order>
    {
      static size_t compute_encoded_size(std::vector<size_t>& conditional_arg_size_cache, ::Order const& order) noexcept
      {
        // pass as arguments the class members you want to serialize
        return compute_total_encoded_size(conditional_arg_size_cache, order.symbol, order.price, order.quantity);
      }

      static void encode(std::byte*& buffer, std::vector<size_t> const& conditional_arg_size_cache,
                         uint32_t& conditional_arg_size_cache_index, ::Order const& order) noexcept
      {
        encode_members(buffer, conditional_arg_size_cache, conditional_arg_size_cache_index, order.symbol,
                       order.price, order.quantity);
      }

      static ::Order decode_arg(std::byte*& buffer)
      {
        ::Order order;
        decode_members(buffer, order, order.symbol, order.price, order.quantity);
        return order;
      }

      static void decode_and_store_arg(std::byte*& buffer, DynamicFormatArgStore* args_store)
      {
        args_store->push_back(decode_arg(buffer));
      }
    };

    Order order {"AAPL", 220.10, 100};

    LOG_INFO(logger, "Order is {}", order);
    LOGV_INFO(logger, "Order", order);

Outputs:

    Order is symbol=AAPL price=220.1 quantity=100

    Order [order: symbol=AAPL price=220.1 quantity=100]

Logging Complex User Defined Types With Private Members
-------------------------------------------------------
For a user define type with private members the easier workaround is same as in the trivially copyable case, to make friend ``quill::Codec<T>`` and also have a private default constructor

.. code:: cpp

    #include "quill/core/Codec.h"
    #include "quill/core/DynamicFormatArgStore.h"

    class Order
    {
    public:
      Order(std::string symbol, double price, int quantity)
        : timestamp(std::chrono::system_clock::now().time_since_epoch().count()), symbol(std::move(symbol)), price(price), quantity(quantity)
      {
      }

    private:
      uint64_t timestamp;
      std::string symbol;
      double price;
      int quantity;

      template <typename T, typename U>
      friend struct quill::Codec;

      Order() = default;

      friend std::ostream& operator<<(std::ostream& os, Order const& order)
      {
        os << "timestamp=" << order.timestamp << " symbol=" << order.symbol << " price=" << order.price << " quantity=" << order.quantity;
        return os;
      }
    };

    template <>
    struct fmtquill::formatter<Order> : fmtquill::ostream_formatter
    {
    };

    template <>
    struct quill::Codec<Order>
    {
      static size_t compute_encoded_size(std::vector<size_t>& conditional_arg_size_cache, ::Order const& order) noexcept
      {
        // pass as arguments the class members you want to serialize
        return compute_total_encoded_size(conditional_arg_size_cache, order.timestamp, order.symbol, order.price, order.quantity);
      }

      static void encode(std::byte*& buffer, std::vector<size_t> const& conditional_arg_size_cache,
                         uint32_t& conditional_arg_size_cache_index, ::Order const& order) noexcept
      {
        encode_members(buffer, conditional_arg_size_cache, conditional_arg_size_cache_index, order.timestamp, order.symbol,
                       order.price, order.quantity);
      }

      static ::Order decode_arg(std::byte*& buffer)
      {
        ::Order order;
        decode_members(buffer, order, order.timestamp, order.symbol, order.price, order.quantity);
        return order;
      }

      static void decode_and_store_arg(std::byte*& buffer, DynamicFormatArgStore* args_store)
      {
        args_store->push_back(decode_arg(buffer));
      }
    };

    Order order {"AAPL", 220.10, 100};
    LOG_INFO(logger, "Order is {}", order);
    LOGV_INFO(logger, "Order", order);

Outputs:

    Order is timestamp=17220432928367021 symbol=AAPL price=220.1 quantity=100

    Order [order: timestamp=17220432928367021 symbol=AAPL price=220.1 quantity=100]