// lexer.hpp
#ifndef LEXER
#define LEXER

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
using namespace std;

class Token
{
public:
    enum class TokenType
    {
        IDENT,
        KEY,
        CONST_NUM,
        CONST_STR,
        SYMBOL,
        CONST_FLOAT,
        CONST_CHAR,
        UNKNOWN,
        END
    };
    Token(TokenType type, std::string val, int id) : type(type), val(val), id(id) {}
    TokenType type;
    std::string val;
    int id;
};

class DFA
{
public:
    map<pair<int, char>, int> transition_map;
    int start_state;
    string symbol_array;
    set<int> final_state_set;
    DFA(string symbol_array = "", int start_state = 0) : symbol_array(symbol_array), start_state(start_state) {}
    void set_symbol_array(string symbol_array)
    {
        this->symbol_array = symbol_array;
    }
    void add_transition(int cur_state, string str, int to_state)
    {
        for (int i = 0; str[i]; ++i)
            transition_map[make_pair(cur_state, str[i])] = to_state;
    }
    void add_finalstate(int state)
    {
        final_state_set.insert(state);
    }
    bool run(int &final_state, int &final_cursor)
    {
        int cur_state = start_state;
        int cursor = 0;
        int len = symbol_array.size();
        int ok = 0;
        while (cursor < len)
        {
            // cout << "cursor " << cursor  << " curstate " << cur_state << endl;
            char cur_symbol = symbol_array[cursor++];
            pair<int, char> t = make_pair(cur_state, cur_symbol);
            if (transition_map.count(t) == 0)
                break;
            int next_state = transition_map[t];
            ok = 0;
            if (final_state_set.count(next_state) > 0)
            {
                final_state = next_state;
                final_cursor = cursor;
                ok = 1;
            }
            cur_state = next_state;
        }
        return ok;
    }
};
class NumDFA : public DFA
{
public:
    NumDFA()
    {
        add_transition(0, "+-", 1);
        add_transition(0, "123456789", 2);
        add_transition(0, "0", 8);
        add_transition(1, "123456789", 2);
        add_transition(2, "0123456789", 2);
        add_transition(2, ".", 3);
        add_transition(3, "0123456789", 4);
        add_transition(4, "0123456789", 4);
        add_transition(4, "e", 5);
        add_transition(5, "+-", 6);
        add_transition(6, "0123456789", 7);
        add_transition(7, "0123456789", 7);
        add_transition(2, "e", 5);
        add_transition(5, "0123456789", 7);
        add_transition(1, "0", 8);
        add_transition(8, "0123456789", 10);
        add_transition(8, ".", 3);
        add_transition(10, "e", 5);
        add_transition(10, ".", 3);
        add_transition(8, "x", 9);
        add_transition(9, "0123456789abcdefABCDEF", 11);
        add_transition(11, "0123456789abcdefABCDEF", 11);
        add_finalstate(8);
        add_finalstate(10);
        add_finalstate(11);
        add_finalstate(2);
        add_finalstate(4);
        add_finalstate(7);
    }
};

class SymbolTable
{
public:
    std::vector<std::string> IDENTSet;
    std::vector<std::string> KEYSet;
    std::vector<std::string> SYMBOLSet;
    std::vector<std::string> CONSTZSet;
    std::vector<std::string> CONSTRSet;
    std::vector<std::string> CONSTCSet;
    std::vector<std::string> CONSTSSet;

    // 获取符号表单例
    static SymbolTable *get()
    {
        if (instance == nullptr)
            instance = new SymbolTable;
        return instance;
    }

    // 将元素添加到指定表中
    int add2Table(std::vector<std::string> &which, std::string val)
    {
        for (int i = 0; i < which.size(); ++i)
        {
            if (which[i] == val)
                return i;
        }
        which.push_back(val);
        return which.size() - 1;
    }

    // 将元素添加到指定表中
    int findInTable(std::vector<std::string> &which, std::string val)
    {
        for (int i = 0; i < which.size(); ++i)
        {
            if (which[i] == val)
                return i;
        }
        return -1;
        // return 0;
    }

    // 判断是否为已存在关键字
    int hasKey(std::string val)
    {
        int index = findInTable(KEYSet, val);
        return index;
    }

    // 判断是否为已存在标识符
    bool hasIdent(std::string val)
    {
        int index = findInTable(IDENTSet, val);
        return index;
    }

    // 新增标识符
    int addIdent(std::string val)
    {
        int index = add2Table(IDENTSet, val);
        return index;
    }

    // 判断是否为已存在界符
    int hasSymbol(std::string val)
    {
        int index = findInTable(SYMBOLSet, val);
        return index;
    }

    // 构造函数
    SymbolTable()
    {
        // 初始化关键字表
        KEYSet = std::vector<std::string>{"int", "void", "break", "float", "while", "do", "struct", "const", "case", "for", "return", "if", "default", "else", "continue"};
        // 初始化界符表
        SYMBOLSet = std::vector<std::string>{"-", "/", "(", ")", "==", "<=", "<", "+", "*", ">", "=", ",", ";", "++", "{", "}", ">="};
    }

private:
    static SymbolTable *instance;
};
SymbolTable *SymbolTable::instance = nullptr;

// 词法分析器
class Lexer
{
public:
    SymbolTable *st;
    NumDFA numdfa;
    int line;
    // 构造函数
    Lexer() : cur(0), line(1)
    {
        st = SymbolTable::get();
    }
    int getLine(){
        return line;
    }
    int getCol(){
        int ans = 0;
        int ct = cur - 1;
        for (ct; ct >= 0; --ct) {
            if (code[ct] == '\n')break;
            ans++;
        }
        return ans;
    }
    // 加载源代码
    void loadCode(string code)
    {
        this->code = code;
        cur = 0;
    }
    // 获取指针位置
    int getCur()
    {
        return cur;
    }
    // 获取当前光标所在行代码
    string getLine(int &new_cur)
    {
        int l = cur, r = cur;
        if (cur > 0 && (code[cur] == '\n' || code[cur] == '\r')) {
            l = cur - 1;
            r = cur - 1;
        }
        
        while (l >= 0 && code[l] != '\n')
            l--;
        while (code[r] != 0 && code[r] != '\n')
            r++;
        l++;
        new_cur = cur - l;
        return code.substr(l, r - l);
    }
    // 获取下一Token
    Token *next()
    {
        // 当未到达文末
        while (cur < code.size())
        {
            // 当前字符
            char curChar = code[cur];
            int index = 0;
            // 跳过空格和换行符
            if (curChar == ' ' || curChar == '\n' || curChar == '\r')
            {
                cur++;
                if (curChar == '\n')line++;
            }
            else if (curChar == '/')
            {
                // 单行注释
                if (cur + 1 < code.size() && code[cur + 1] == '/')
                {
                    cur += 2;
                    while (cur < code.size() && code[cur] != '\n')
                    {
                        cur++;
                    }
                }
                // 多行注释
                else if (cur + 1 < code.size() && code[cur + 1] == '*')
                {
                    cur += 2;
                    while (cur < code.size() - 1 && !(code[cur] == '*' && code[cur + 1] == '/'))
                    {
                        if (code[cur] == '\n')line++;
                        cur++;
                    }
                    cur += 2;
                }
                // 单个除号
                else
                {
                    string val = "/";
                    cur++;
                    return new Token(Token::TokenType::SYMBOL, val, index);
                }
            }
            // 字母开头，可能为关键字或标识符
            else if (isalpha(curChar) || isZH(curChar))
            {
                string val = "";
                while (cur < code.size() && (isalpha(code[cur]) || isdigit(code[cur]) || isZH(code[cur])))
                    val += code[cur++];
                if ((index = st->hasKey(val)) != -1)
                {
                    return new Token(Token::TokenType::KEY, val, index);
                }
                else
                {
                    index = st->addIdent(val);
                    return new Token(Token::TokenType::IDENT, val, index);
                }
            }
            // 引号开头
            else if (curChar == '\"')
            {
                string val = "";
                cur++;

                while (cur < code.size() && code[cur] != '\"')
                {
                    if (cur < code.size() && code[cur] == '\\')
                    {
                        cur++;
                        if (cur < code.size() && code[cur] == 'n')
                        {
                            val += "\n";
                        }
                        else if (cur < code.size() && code[cur] == 't')
                        {
                            val += "\t";
                        }
                         else if (cur < code.size() && code[cur] == 'v')
                        {
                            val += "\t";
                        }
                        else if (cur < code.size() && code[cur] == 'r')
                        {
                            val += "\r";
                        }
                        else if (cur < code.size() && code[cur] == 'f')
                        {
                            val += "\f";
                        }
                        else if (cur < code.size() && code[cur] == 'b')
                        {
                            val += "\b";
                        }
                        else if (cur < code.size() && code[cur] == '\\')
                        {
                            val += "\\";
                        }
                        else if (cur < code.size() && code[cur] == '\'')
                        {
                            val += "\'";
                        }
                        else if (cur < code.size() && code[cur] == '\"')
                        {
                            val += "\"";
                        }
                        cur++;
                    }
                    if(cur < code.size() && code[cur] != '\"')
                     {val += code[cur++];}
                }
                // 判断是否为转义符

                cur++;
                index = st->add2Table(st->CONSTSSet, val);
                return new Token(Token::TokenType::CONST_STR, val, index);
            }
            // 单引号开头
            else if (curChar == '\'')
            {
                string val = "";
                cur++;
                  if (cur < code.size() && code[cur] == '\\')
                    {
                        cur++;
                        if (cur < code.size() && code[cur] == 'n')
                        {
                            val += "\n";
                        }
                        else if (cur < code.size() && code[cur] == 't')
                        {
                            val += "\t";
                        }
                        else if (cur < code.size() && code[cur] == 'r')
                        {
                            val += "\r";
                        }
                        else if (cur < code.size() && code[cur] == 'f')
                        {
                            val += "\f";
                        }
                        else if (cur < code.size() && code[cur] == 'v')
                        {
                            val += "\v";
                        }
                        else if (cur < code.size() && code[cur] == 'b')
                        {
                            val += "\b";
                        }
                        else if (cur < code.size() && code[cur] == '\\')
                        {
                            val += "\\";
                        }
                        else if (cur < code.size() && code[cur] == '\'')
                        {
                            val += "\'";
                        }
                        else if (cur < code.size() && code[cur] == '\"')
                        {
                            val += "\"";
                        }
                        cur++;
                    }
                else if (cur < code.size() && code[cur] != '\'')
                    val += code[cur++];
                if (cur < code.size() && code[cur] == '\'')
                {
                    cur++;
                    index = st->add2Table(st->CONSTCSet, val);
                    return new Token(Token::TokenType::CONST_CHAR, val, index);
                }
                return new Token(Token::TokenType::UNKNOWN, val, index);
            }
            // 数字开头
            else if (isdigit(curChar))
            {
                string temp = code.substr(cur);
                numdfa.set_symbol_array(temp);
                int final_state, final_cursor;
                if (!numdfa.run(final_state, final_cursor))
                {
                    cur++;
                    return new Token(Token::TokenType::UNKNOWN, "" + curChar, index);
                }
                string val = code.substr(cur, final_cursor);
                cur += final_cursor;
                if (final_state == 2 || final_state == 8 || final_state == 10)
                {
                    index = st->add2Table(st->CONSTZSet, val);
                    return new Token(Token::TokenType::CONST_NUM, val, index);
                }
                else if (final_state == 4 || final_state == 7)
                {
                    index = st->add2Table(st->CONSTRSet, val);
                    return new Token(Token::TokenType::CONST_FLOAT, val, index);
                }
                else if (final_state == 11)
                {
                    val = hex2dec(val);
                    index = st->add2Table(st->CONSTZSet, val);
                    return new Token(Token::TokenType::CONST_NUM, val, index);
                }

                return new Token(Token::TokenType::UNKNOWN, val, index);
            }
            else if (isSymbol(code[cur], code[cur + 1], index))
            {
                string val = "";
                int n = isSymbol(code[cur], code[cur + 1], index);
                for (int i = 0; i < n; ++i)
                    val += code[cur++];
                return new Token(Token::TokenType::SYMBOL, val, index);
            }
            // 未知符号
            else
            {
                string val = "";
                val += code[cur++];
                return new Token(Token::TokenType::UNKNOWN, val, index);
            }
        }
        return new Token(Token::TokenType::END, "#", 0);
    }

private:
    int isSymbol(char c1, char c2, int &index)
    {
        char temp[3] = {0};
        temp[0] = c1;
        temp[1] = c2;
        string ss(temp);
        if ((index = st->hasSymbol(ss)) != -1 && c2 != 0)
        {
            return 2;
        }
        temp[1] = 0;
        string s(temp);
        if ((index = st->hasSymbol(s)) != -1)
        {
            return 1;
        }
        return 0;
    }
    bool isZH(char c)
    {
        return c < 0;
    }
    string nextNum(bool canbeminus, bool canbefloat, bool canbescience, bool canbehex)
    {
        return "";
    }
    string hex2dec(string hex)
    {
        long long a = 0;
        for (int i = 0; hex[i]; ++i)
        {
            a *= 16;
            if (hex[i] >= '0' && hex[i] <= '9')
                a += hex[i] - '0';
            else if (hex[i] >= 'a' && hex[i] <= 'f')
                a += hex[i] - 'a' + 10;
            else if (hex[i] >= 'A' && hex[i] <= 'F')
                a += hex[i] - 'A' + 10;
        }
        string ans = "";
        do
        {
            int t = a % 10;
            ans += t + '0';
            a /= 10;
        } while (a);
        reverse(ans.begin(), ans.end());
        return ans;
    }
    string code; // 源代码
    int cur;     // 当前文本指针位置
};
#endif
