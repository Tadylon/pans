#include "logger/formatter.h"
#include <unordered_map>
#include <charconv>

namespace pans::detail {

constexpr std::string_view DEFAULT_DATE_FORMAT = "%Y-%m-%d %H:%M:%S";

Formatter::Formatter(std::string_view pattern)
    : m_pattern(pattern)
{
    if(parse())
    {
        throw std::invalid_argument("invalid log format. parse failed.");
    }
}

void Formatter::format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept
{
    for(const auto& item : m_items)
    {
        item->format(record, output);
    }
}

template <typename T>
void AppendInteger(FormattedRecordBuffer& output, T value) noexcept
{
    std::array<char, 24> buffer{};
    const auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value); // 基本上不会失败，转失败了也没关系
    ASSERT_RETNONE2(result.ec == std::errc(), "trans " << value << " to chars failed.");
    output.append(buffer.data(), static_cast<std::size_t>(result.ptr - buffer.data()));
}

class LiteralFormatItem final : public Formatter::FormatItem
{
public:
    explicit LiteralFormatItem(std::string value)
        : m_value(std::move(value))
    {}
    void format(const LogRecordView&, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(m_value);
    }

private:
    std::string m_value;
};

class MessageFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(record.m_message);
    }
};

class LevelFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(LogLevel::ToString(record.m_level));
    }
};

class ElapsedFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        AppendInteger(output, std::chrono::duration_cast<std::chrono::milliseconds>(record.m_elapsed).count());
    }
};

class LoggerNameFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(record.m_loggerName);
    }
};

class ThreadIdFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        AppendInteger(output, record.m_threadId);
    }
};

class NewLineFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView&, FormattedRecordBuffer& output) const noexcept override
    {
        output.append('\n');
    }
};

class DateTimeFormatItem final : public Formatter::FormatItem
{
public:
    explicit DateTimeFormatItem(std::string_view format)
        : m_format(format.empty() ? DEFAULT_DATE_FORMAT : format)
    {}
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        static thread_local time_t last_second = 0;
        static thread_local char cached_date_time[20] = {'\0'};
        const auto duration = record.m_timestamp.time_since_epoch();
        const time_t current_second = static_cast<time_t>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
        if(current_second != last_second)
        {
            std::tm buffer{};
#if defined(_WIN32)
            const errno_t result = localtime_s(&buffer, &current_second);
            ASSERT_RETNONE2(result == 0, "failed to convert log time to local time");
#else
            const std::tm* result = localtime_r(&current_second, &buffer);
            ASSERT_RETNONE2(result != nullptr, "failed to convert log time to local time");
#endif
            const std::size_t size = std::strftime(cached_date_time, sizeof(cached_date_time), m_format.c_str(), &buffer);
            ASSERT_RETNONE2(size != 0, "failed to format log time");
            last_second = current_second;
        }

        output.append(cached_date_time);
    }

private:
    std::string m_format;
};

class MicrosecondsFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        const auto total_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(record.m_timestamp.time_since_epoch()).count();
        std::int64_t microseconds = total_microseconds % 1'000'000;
        if(microseconds < 0)
        {
            microseconds += 1'000'000;
        }
        AppendInteger(output, microseconds);
    }
};

class FileNameFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(record.m_fileName);
    }
};

class LineFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        AppendInteger(output, record.m_location.line());
    }
};

class FunctionNameFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(record.m_functionName);
    }
};

class TabFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView&, FormattedRecordBuffer& output) const noexcept override
    {
        output.append('\t');
    }
};

class FiberIdFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        AppendInteger(output, record.m_fiberId);
    }
};

class ThreadNameFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(record.m_threadName);
    }
};

using FormatItemFactory = std::unique_ptr<Formatter::FormatItem> (*)(std::string_view);

template <typename Item>
[[nodiscard]] std::unique_ptr<Formatter::FormatItem> CreateSimpleFormatItem(std::string_view)
{
    return std::make_unique<Item>();
}

template <typename Item>
[[nodiscard]] std::unique_ptr<Formatter::FormatItem> CreateConfiguredFormatItem(std::string_view format)
{
    return std::make_unique<Item>(format);
}

[[nodiscard]] const std::unordered_map<char, FormatItemFactory>& GetFormatItemFactories()
{
    static const std::unordered_map<char, FormatItemFactory> FORMAT_ITEM_FACTORIES{
        {'m', &CreateSimpleFormatItem<MessageFormatItem>}, // m代表MessageFormatItem, 它是程序员写日志时插入的message
        {'p', &CreateSimpleFormatItem<LevelFormatItem>}, // p代表LevelFormatItem
        {'r', &CreateSimpleFormatItem<ElapsedFormatItem>}, // r代表ElapsedFormatItem
        {'c', &CreateSimpleFormatItem<LoggerNameFormatItem>}, //c日志器名称
        {'t', &CreateSimpleFormatItem<ThreadIdFormatItem>},//t 线程id
        {'n', &CreateSimpleFormatItem<NewLineFormatItem>},// n换行
        {'d', &CreateConfiguredFormatItem<DateTimeFormatItem>},//d代表datetime
        {'u', &CreateSimpleFormatItem<MicrosecondsFormatItem>},//u毫秒数
        {'f', &CreateSimpleFormatItem<FileNameFormatItem>},//小f文件名
        {'l', &CreateSimpleFormatItem<LineFormatItem>},//小l行号
        {'M', &CreateSimpleFormatItem<FunctionNameFormatItem>},//大M函数名
        {'T', &CreateSimpleFormatItem<TabFormatItem>},//大T制表符
        {'F', &CreateSimpleFormatItem<FiberIdFormatItem>},//大F协程ID
        {'N', &CreateSimpleFormatItem<ThreadNameFormatItem>},//大N线程名
    };
    return FORMAT_ITEM_FACTORIES;
}

[[nodiscard]] std::unique_ptr<Formatter::FormatItem> CreateFormatItem(char directive, std::string_view format)
{
    const auto& factories = GetFormatItemFactories();
    const auto it = factories.find(directive);
    ASSERT_RETVAL2(it != factories.end(), nullptr, "unknown logger format directive: " << directive);
    return it->second(format);
}

void Formatter::addLiteral(std::string& literal) noexcept
{
    if(literal.empty())
    {
        return;
    }
    m_items.push_back(std::make_unique<LiteralFormatItem>(std::move(literal)));
    literal.clear();
}

// %d{%Y-%m-%d %H:%M:%S}.%u%Tthread=%t%Tfiber=%F%T[%p]%T%f:%l%T%M%T%m%n
int Formatter::parse() noexcept
{
    std::string literal;
    for(std::size_t index = 0; index < m_pattern.size(); ++index)
    {
        if(m_pattern[index] != '%')
        {
            literal.push_back(m_pattern[index]);
            continue;
        }
        ASSERT_RETVAL2(index + 1 < m_pattern.size(), -1, "logger format pattern ends with an incomplete directive");
        const char directive = m_pattern[++index];
        if(directive == '%')
        {
            literal.push_back('%');
            continue;
        }
        addLiteral(literal);
        std::string_view item_format;
        if(index + 1 < m_pattern.size() && m_pattern[index + 1] == '{')
        {
            const std::size_t closing_brace = m_pattern.find('}', index + 2);
            ASSERT_RETVAL2(closing_brace != std::string::npos && closing_brace != (index + 2), -2, "missing a closing brace or empty");
            item_format = std::string_view(m_pattern).substr(index + 2, closing_brace - index - 2);
            index = closing_brace;
        }
        m_items.push_back(CreateFormatItem(directive, item_format));
    }
    addLiteral(literal);
    for(const auto& item : m_items)
    {
        ASSERT_RETVAL2(item != nullptr, -3, "logger formatter contains a null format item");
    }

    return 0;
}

} // namespace pans::detail
