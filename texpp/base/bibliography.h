#ifndef BIBLIOGRAPHY_H
#define BIBLIOGRAPHY_H

#include <texpp/common.h>
#include <texpp/command.h>
#include <texpp/base/func.h>
#include <texpp/lexer.h>
#include <map>

namespace texpp {
namespace base {

class Bibliography : public Command
{
public:
    explicit Bibliography(const string& name): Command(name) {}
    bool invoke(Parser& parser, shared_ptr<Node> node);
};

class Cite : public Command
{
public:
    explicit Cite(const string& name): Command(name) {}
    bool invoke(Parser& parser, shared_ptr<Node> node);
};

} // namespace base
} // namespace texpp

#endif // BIBLIOGRAPHY_H
