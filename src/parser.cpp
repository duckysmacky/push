#include "parser.h"

Command Parser::next_command()
{
    Tokenizer tokenizer(m_input);

    Command cmd;
    while (auto token = tokenizer.next_token())
    {
        cmd.args.emplace_back(token->value);
    }

    return cmd;
}