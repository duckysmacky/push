#include "parser.h"

#include <iostream>

Command Parser::next_command()
{
    Command cmd;

    while (auto token = m_tokenizer.next_token())
    {
		std::cout << *token << std::endl;

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
