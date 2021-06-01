#include <iostream>
#include <string>

using namespace std;

//创建抽象的日志记录器AbstractLogger类。
class AbstractLogger
{
public:
    AbstractLogger();
    virtual ~AbstractLogger();
    static int INFO;
    static int DEBUG;
    static int ERROR;

    void setNextLogger(AbstractLogger *nextLogger);
    void logMessage(int level, string messager);

protected:
    int level;
    AbstractLogger *nextLogger;

    virtual void write(string message) = 0;
};

int AbstractLogger::INFO = 1;
int AbstractLogger::DEBUG = 2;
int AbstractLogger::ERROR = 3;

AbstractLogger::AbstractLogger()
{
    level = INFO;
    nextLogger = NULL;
}
AbstractLogger::~AbstractLogger()
{
    delete nextLogger;
    nextLogger = NULL;
}

void AbstractLogger::setNextLogger(AbstractLogger *nextLogger)
{
    this->nextLogger = nextLogger;
}

void AbstractLogger::logMessage(int level, string messager)
{
    if (this->level == level)
    {
        write(messager);
    }
    if (nextLogger != NULL)
    {
        nextLogger->logMessage(level, messager);
    }
}

//创建扩展了AbstractLogger类的实体ConsoleLogger类。
class ConsoleLogger : public AbstractLogger
{
public:
    ConsoleLogger(int level);
    ~ConsoleLogger(){};

    void write(string message);
};

ConsoleLogger::ConsoleLogger(int level)
{
    this->level = level;
}

void ConsoleLogger::write(string message)
{
    cout << "ConsoleLogger: console is " << message << endl;
}

//创建扩展了AbstractLogger类的实体ErrorLogger类。
class ErrorLogger : public AbstractLogger
{
public:
    ErrorLogger(int level);
    ~ErrorLogger(){};

    void write(string message);
};

ErrorLogger::ErrorLogger(int level)
{
    this->level = level;
}

void ErrorLogger::write(string message)
{
    cout << "ErrorLogger: console is " << message << endl;
}

//创建扩展了AbstractLogger类的实体FileLogger类。
class FileLogger : public AbstractLogger
{
public:
    FileLogger(int level);
    ~FileLogger(){};

    void write(string message);
};

FileLogger::FileLogger(int level)
{
    this->level = level;
}

void FileLogger::write(string message)
{
    cout << "FileLogger: console is " << message << endl;
}

//创建不同类型的记录器。赋予它们不同的错误级别，并在每个记录器中设置下一个记录器。每个记录器中的下一个记录器代表的是链的一部分。
class chainPatternDemo
{
public:
    chainPatternDemo(){};
    ~chainPatternDemo(){};
    static AbstractLogger *getchainOfLoggers();
};

AbstractLogger *chainPatternDemo::getchainOfLoggers()
{
    AbstractLogger *terrorlogger = (AbstractLogger *)new ErrorLogger(AbstractLogger::ERROR);
    AbstractLogger *tconsolelogger = (AbstractLogger *)new ConsoleLogger(AbstractLogger::DEBUG);
    AbstractLogger *tfilelogger = (AbstractLogger *)new FileLogger(AbstractLogger::INFO);

    terrorlogger->setNextLogger(tfilelogger);
    tfilelogger->setNextLogger(tconsolelogger);
    return terrorlogger;
}

int main()
{
    AbstractLogger *loggerchain = chainPatternDemo::getchainOfLoggers();
    loggerchain->logMessage(AbstractLogger::INFO, "This is an info information");
    loggerchain->logMessage(AbstractLogger::DEBUG, "This is an debug information");
    loggerchain->logMessage(AbstractLogger::ERROR, "This is an error information");

    delete loggerchain;
    loggerchain = NULL;
    // system("pause");
    return 0;
}
