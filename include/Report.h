#ifndef REPORT_H
#define REPORT_H

#include <string>

// Abstract report interface — concrete reports use polymorphic generate().
class Report {public:
    virtual ~Report() = default;
    virtual std::string generate() const = 0;
    virtual std::string getTitle() const = 0;
};

#endif
