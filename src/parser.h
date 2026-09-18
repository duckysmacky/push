#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <stdexcept>

#include "tokenizer.h"

// TODO: add friend << overloads for std::ostream

class ParserException : public std::runtime_error
{
public:
    explicit ParserException(const std::string& msg) 
        : std::runtime_error(msg) {}
};

enum class RedirectionType
{
	In, InMultiline,
	Out, OutAppend
};

struct Redirection
{
	RedirectionType type;
	std::string target;
};

struct Command
{
	std::vector<std::string> argv;
	std::vector<Redirection> redirections;
};

struct Pipeline
{
	std::vector<Command> commands;
	bool background = false;
};

enum class Separator
{
	Sequence, End
};

struct CommandEntry
{
	Pipeline pipeline;
	std::optional<Separator> separator;
};

class Parser
{
private:
    Tokenizer m_tokenizer;

	void validate_token(const Token& token) const;

	Pipeline parse_pipeline();
	Command parse_command();
	Redirection parse_redirection();
	Separator parse_separator();
	CommandEntry parse_command_entry();

public:
    Parser(std::string_view input)
        : m_tokenizer(input)
    {}

	std::vector<CommandEntry> parse();
};
