#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <climits>
#include <unordered_map>

using namespace std;

struct Token
{
    string type;
    int line;
    int column;
    string value;
};

struct Comment
{
    string content;
    int line;
    int column;
};

const vector<pair<string, string>> TOKENS =
{
    {"RUSSIAN_SYMBOL", R"([^\s,.:;(){}\[\]\+\-\*/:=<>]*[а-€ј-я]+[^\s,.:;(){}\[\]\+\-\*/:=<>]*)"},
    {"BEGIN", R"(\b[Bb][Ee][Gg][Ii][Nn]\b)"},
    {"END", R"(\b[Ee][Nn][Dd]\b)"},
    {"PROGRAM", R"(\b[Pp][Rr][Oo][Gg][Rr][Aa][Mm]\b)"},
    {"VAR", R"(\b[Vv][Aa][Rr]\b)"},
    {"INTEGER", R"(\b[Ii][Nn][Tt][Ee][Gg][Ee][Rr]\b)"},
    {"FLOAT", R"(\b[Ff][Ll][Oo][Aa][Tt]\b)"},
    {"CHAR", R"(\b[Cc][Hh][Aa][Rr]\b)"},
    {"BOOLEAN", R"(\b[Bb][Oo][Oo][Ll][Ee][Aa][Nn]\b)"},
    {"IF", R"(\b[Ii][Ff]\b)"},
    {"ELSE", R"(\b[Ee][Ll][Ss][Ee]\b)"},
    {"ARRAY", R"(\b[Aa][Rr][Rr][Aa][Yy]\b)"},
    {"OF", R"(\b[Oo][Ff]\b)"},
    {"THEN", R"(\b[Tt][Hh][Ee][Nn]\b)"},
    {"COMMENT",  R"(//.*|\{[^}]*\}|\{[^}]*)"},
    {"PLUS", R"(\+)"},
    {"MINUS", R"(-)"},
    {"MULTIPLY", R"(\*)"},
    {"DIVIDE", R"(/)"},
    {"LESS_OR_EQUAL", R"(<=)"},
    {"GREATER_OR_EQUAL", R"(>=)"},
    {"ASSIGN", R"(:=)"},
    {"EQUAL", R"(=)"},
    {"NOT_EQUAL", R"(<>)"},
    {"LESS_THAN", R"(<)"},
    {"GREATER_THAN", R"(>)"},
    {"DELIMITER", R"([.]{2})"},
    {"E_FLOAT", R"((\.\d+)?[eE][+-]?\d+)"},
    {"FIXED_FLOAT", R"(\.\d+)"},
    {"COMMA", R"(,)"},
    {"DOT", R"(\.)"},
    {"COLON", R"(:)"},
    {"SEMICOLON", R"(;)"},
    {"LEFT_PARENTHESIS", R"(\()"},
    {"RIGHT_PARENTHESIS", R"(\))"},
    {"LEFT_BRACE", R"(\{)"},
    {"RIGHT_BRACE", R"(\})"},
    {"LEFT_BRACKET", R"(\[)"},
    {"RIGHT_BRACKET", R"(\])"},
    {"IDENTIFIER", R"([a-zA-Z_][a-zA-Z0-9_]*)"},
    {"E_FLOAT", R"(\d+(\.\d+)?[eE][+-]?\d+)"},
    {"FIXED_FLOAT", R"(\d+\.\d+)"},
    {"INTEGER_NUMBER", R"(-?\d+)"},
    {"STRING", R"('([^']*)'|'([^']*))"},
    {"WHITESPACE", R"(\s+)"},
    {"UNKNOWN", R"(.)"}
};

const string ERROR = "ERROR_";

enum ErrorCode {
    ERROR_UNKNOWN_TOKEN = 1,
    ERROR_UNCLOSED_STRING_LITERAL,
    ERROR_UNCLOSED_BLOCK_COMMENT,
    ERROR_INVALID_INTEGER,
    ERROR_INVALID_FLOAT,
    ERROR_RUSSIAN_SYMBOL,
    ERROR_IDENTIFIER_TOO_LONG,
    ERROR_EMPTY_FILE = 100
};

static void ReportError(int line, int column, const string& message, ErrorCode code) {
    cerr << "ќшибка (код " << code << "): " << message
        << " на строке " << line + 1
        << ", позици€ " << column + 1 << endl;
}

static bool IsValidInteger(const string& value)
{
    try
    {
        int num = stoll(value);
        return num >= -32'767 && num <= 32'767;
    }
    catch (...)
    {
        return false;
    }
}

static bool IsValidFloat(const string& value)
{
    size_t ePos = value.find_first_of("eE");
    string exponent = value.substr(ePos + 1);

    if (exponent.front() == '+' || exponent.front() == '-')
    {
        exponent = exponent.substr(1);
    }

    if (exponent.size() > 2)
    {
        return false;
    }

    return true;
}

static void ClearComment(Comment& comment)
{
    comment.content = "";
    comment.column = 1;
    comment.line = 1;
}

static vector<Token> Lexer(const string& code)
{
    vector<Token> tokens;
    vector<string> lines;
    size_t start = 0, end = 0;

    bool inBlockComment = false;
    bool inStringLiteral = false;

    Comment comment;

    if (code.empty())
    {
        tokens.push_back({ ERROR + to_string(ERROR_EMPTY_FILE), 1, 1, code });
        return tokens;
    }

    while ((end = code.find('\n', start)) != string::npos)
    {
        lines.push_back(code.substr(start, end - start));
        start = end + 1;
    }
    lines.push_back(code.substr(start));

    for (size_t lineNum = 0; lineNum < lines.size(); ++lineNum)
    {
        const string& line = lines[lineNum];
        size_t pos = 0;

        while (pos < line.size())
        {
            smatch match;
            bool matched = false;

            if (inBlockComment || inStringLiteral)
            {
                comment.content += line[pos];
                if (pos + 1 >= line.size() && (line.substr(pos, 2) != "}" && line.substr(pos, 2) != "'"))
                {
                    comment.content += '\n';
                }

                if (inBlockComment && line.substr(pos, 2) == "}")
                {
                    tokens.push_back({ "COMMENT", static_cast<int>(comment.line) + 1, static_cast<int>(comment.column) + 1, comment.content });
                    ClearComment(comment);
                    inBlockComment = false;
                }
                else if (inStringLiteral && line.substr(pos, 2) == "'")
                {
                    tokens.push_back({ "STRING", static_cast<int>(comment.line) + 1, static_cast<int>(comment.column) + 1, comment.content });
                    ClearComment(comment);
                    inStringLiteral = false;
                }

                matched = true;
                pos++;
            }

            if (!inBlockComment && !inStringLiteral)
            {
                for (const auto& [type, regexPattern] : TOKENS)
                {
                    smatch match;
                    if (regex_search(line.begin() + pos, line.end(), match, regex(regexPattern)) && match.position() == 0)
                    {
                        string lexeme = match.str();

                        if (type != "WHITESPACE")
                        {
                            if (type == "COMMENT" && lexeme.front() == '{' && lexeme.back() != '}')
                            {
                                comment.content += lexeme + '\n';
                                comment.column = pos;
                                comment.line = lineNum;
                                inBlockComment = true;
                            }
                            else if (type == "STRING" && lexeme.back() != '\'')
                            {
                                comment.content += lexeme + '\n';
                                comment.column = pos;
                                comment.line = lineNum;
                                inStringLiteral = true;
                            }
                            else if (type == "E_FLOAT" && !IsValidFloat(lexeme))
                            {
                                tokens.push_back({ ERROR + to_string(ERROR_INVALID_FLOAT), static_cast<int>(lineNum) + 1, static_cast<int>(pos) + 1, lexeme });
                            }
                            else if (type == "INTEGER_NUMBER" && !IsValidInteger(lexeme))
                            {
                                tokens.push_back({ ERROR + to_string(ERROR_INVALID_INTEGER), static_cast<int>(lineNum) + 1, static_cast<int>(pos) + 1, lexeme });
                            }
                            else if (type == "RUSSIAN_SYMBOL")
                            {
                                tokens.push_back({ ERROR + to_string(ERROR_RUSSIAN_SYMBOL), static_cast<int>(lineNum) + 1, static_cast<int>(pos) + 1, lexeme });
                            }
                            else if (type == "IDENTIFIER" && lexeme.length() > 256)
                            {
                                tokens.push_back({ ERROR + to_string(ERROR_IDENTIFIER_TOO_LONG), static_cast<int>(lineNum) + 1, static_cast<int>(pos) + 1, lexeme });
                            }
                            else
                            {
                                tokens.push_back({ type, static_cast<int>(lineNum) + 1, static_cast<int>(pos) + 1, lexeme });
                            }
                        }

                        pos += lexeme.length();
                        matched = true;
                        break;
                    }
                }
            }

            if (!matched)
            {
                ++pos;
            }
        }
    }

    if (inBlockComment)
    {
        tokens.push_back({ ERROR + to_string(ERROR_UNCLOSED_BLOCK_COMMENT), static_cast<int>(comment.line) + 1, static_cast<int>(comment.column) + 1, comment.content });
    }

    if (inStringLiteral)
    {
        tokens.push_back({ ERROR + to_string(ERROR_UNCLOSED_STRING_LITERAL), static_cast<int>(comment.line) + 1, static_cast<int>(comment.column) + 1, comment.content });
    }

    tokens.push_back({ "EOF", static_cast<int>(lines.size() + 1), 0, "" });

    return tokens;
}

static void PrintTokens(const vector<Token>& tokens, const string& filename)
{
    ofstream file(filename);

    for (const auto& token : tokens)
    {
        file << token.type << "(" << token.line << ", " << token.column << ")-\"" << token.value << "\"" << endl;
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");

    string inFile = "input.pas";
    string outFile = "output.txt";

    ifstream file(inFile);

    string code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    vector<Token> tokens = Lexer(code);
    PrintTokens(tokens, outFile);

    return 0;
}
