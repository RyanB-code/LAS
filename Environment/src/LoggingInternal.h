#include <LAS/Logging.h>

class LogToFile final : public LogOutput{
public:
    LogToFile();
    ~LogToFile();

    std::string getPath() const;
    bool setPath(std::string setPath);      // Sets the path to the file, will not create file

    bool log (const Log& log, const LogSettings& logSettings) const override;
private:
    std::string path;
};