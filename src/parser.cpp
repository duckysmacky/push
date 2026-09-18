#include "parser.h"

// ls -la | wc -l > lines.txt && cat lines.txt

Redirection Parser::parse_redirection()
{
	std::optional<RedirectionType> redirection_type;

	while (auto token = m_tokenizer.peek_token())
	{
		switch (token->type)
		{
			// Empty
			case TokenType::Empty:
			{
				m_tokenizer.consume_token();
				continue;
			}
			// Out
			case TokenType::RightArrow:
			{
				m_tokenizer.consume_token();
				redirection_type = RedirectionType::Out;
				continue;
			}
			// Out append
			case TokenType::DoubleRightArrow:
			{
				m_tokenizer.consume_token();
				redirection_type = RedirectionType::OutAppend;
				continue;
			}
			// Target
			case TokenType::Word:
			{
				m_tokenizer.consume_token();

				if (!redirection_type.has_value())
				{
					throw ParserException("No redirection type for target");
				}

				return {
					.type = *redirection_type,
					.target = std::string{*token->value},
				};
			}
			default:
			{
				break;
			}
        }
	}

	if (!redirection_type.has_value())
	{
		throw ParserException("No redirection type specified");
	}
	else
	{
		throw ParserException("No redirection target specified");
	}
}

Command Parser::parse_command()
{
	Command command;

	std::vector<std::string> argv;
	std::vector<Redirection> redirections;

	while (auto token = m_tokenizer.peek_token())
	{
		switch (token->type)
		{
			// Empty
			case TokenType::Empty:
			{
				m_tokenizer.consume_token();
				continue;
			}
			// Command arg
			case TokenType::Word:
			{
				m_tokenizer.consume_token();
				// token of type word always has a value
				argv.emplace_back(*token->value);
				continue;
			}
			// Redirection
			case TokenType::RightArrow:
			case TokenType::DoubleRightArrow:
			{
				if (argv.empty())
				{
					throw ParserException("No command to redirect");
				}

				Redirection redirection = parse_redirection();
				redirections.push_back(std::move(redirection));
				continue;
			}
			default:
			{
				break;
			}
        }
	}

	if (argv.empty())
	{
		throw ParserException("Empty command arguments");
	}

	return {
		.argv = std::move(argv),
		.redirections = std::move(redirections),
	};
}

Pipeline Parser::parse_pipeline()
{
	Pipeline pipeline;

	std::optional<Command> command;

	while (auto token = m_tokenizer.peek_token())
	{
		switch (token->type)
		{
			// Empty
			case TokenType::Empty:
			{
				m_tokenizer.consume_token();
				continue;
			}
			// Background
			case TokenType::And:
			{
				m_tokenizer.consume_token();

				if (pipeline.commands.empty())
				{
					throw ParserException("Invalid background process specifier for empty pipeline");
				}
				else if (pipeline.background)
				{
					throw ParserException("Expected pipeline, got background process specifier");
				}

				pipeline.background = true;
				continue;
			}
			// Pipe
			case TokenType::Or:
			{
				m_tokenizer.consume_token();

				if (!command.has_value())
				{
					throw ParserException("No command to pipe");
				}

				pipeline.commands.push_back(std::move(*command));
				command.reset();
				continue;
			}
			// Command
			case TokenType::Word:
			case TokenType::LeftArrow:
			case TokenType::DoubleLeftArrow:
			case TokenType::RightArrow:
			case TokenType::DoubleRightArrow:
			case TokenType::DoubleOr:
			{
				if (command.has_value())
				{
					throw ParserException("Pipe operator missing to pipe the command");
				}

				command = parse_command();
				continue;
			}
			default:
			{
				break;
			}
        }
	}

	if (!command.has_value())
	{
		throw ParserException("No command specified");
	}

	pipeline.commands.push_back(std::move(*command));
	return pipeline;
}

CommandEntry Parser::parse_command_entry()
{
	std::optional<Pipeline> pipeline;

	while (auto token = m_tokenizer.peek_token())
	{
		switch (token->type)
		{
			// Empty
			case TokenType::Empty:
			{
				m_tokenizer.consume_token();
				continue;
			}
			// Separator
			case TokenType::DoubleAnd:
			case TokenType::Semicolon:
			{
				m_tokenizer.consume_token();

				if (!pipeline.has_value())
				{
					throw ParserException("Unexpected separator: no pipeline");
				}

				auto separator = (token->type == TokenType::DoubleAnd)
					? Separator::Sequence
					: Separator::End;

				// pipeline is guaranteed to exist at this point
				return {
					.pipeline = std::move(*pipeline),
					.separator = separator,
				};
			}
			// Pipeline
			case TokenType::Word:
			case TokenType::LeftArrow:
			case TokenType::DoubleLeftArrow:
			case TokenType::RightArrow:
			case TokenType::DoubleRightArrow:
			case TokenType::And:
			case TokenType::Or:
			case TokenType::DoubleOr:
			{
				pipeline = parse_pipeline();
				continue;
			}
			default:
			{
				break;
			}
        }
	}

	if (!pipeline.has_value())
	{
		throw ParserException("Empty pipeline");
	}

	return {
		.pipeline = std::move(*pipeline),
		.separator = std::nullopt
	};
}

std::vector<CommandEntry> Parser::parse()
{
	std::vector<CommandEntry> command_list;

	while (auto token = m_tokenizer.peek_token())
	{
		switch (token->type)
		{
			case TokenType::Empty:
			{
				m_tokenizer.consume_token();
				continue;
			}
			case TokenType::Unknown:
			{
				throw ParserException("Unknown token");
			}
			case TokenType::Invalid:
			{
				throw ParserException("Invalid token");
			}
			// TODO: add support
			case TokenType::LeftArrow:
			case TokenType::DoubleLeftArrow:
			case TokenType::DoubleOr:
			{
				throw ParserException("Unsupported token");
			}
			case TokenType::Word:
			case TokenType::RightArrow:
			case TokenType::DoubleRightArrow:
			case TokenType::And:
			case TokenType::DoubleAnd:
			case TokenType::Semicolon:
			case TokenType::Or:
			{
				CommandEntry entry = parse_command_entry();
				command_list.push_back(std::move(entry));
				continue;
			}
        }
	}

	return command_list;
}

