#include "parser.h"

Command Parser::next_command()
{
    Command cmd;

    while (auto token = m_tokenizer.peek_token())
    {
		if (token->type == TokenType::Word)
		{
			cmd.args.emplace_back(*(token->value));
		}
		else
		{
			break;
		}
    }

    return cmd;
}
