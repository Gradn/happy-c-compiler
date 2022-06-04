#include <iostream>
#include "Lexer.hpp"
#include <fstream>
#include "Fuhaobiao.hpp"
#include <stack>
#include "Siyuanshi.hpp"
#include "Global.hpp"
#include "Actions.hpp"
#include "Youhua.hpp"
// 递归下降子程序分析法
// #define RELEASE
// #define OUTDETAIL
#define FILE_DIR "./test_code/"
#define FILE_NAME "test4.hcc"
#define OUT_DIR "./output/"

#define DECRIPTION "-----------------------------------\n        Happy C Compiler 1.0        \n-----------------------------------\n   HCC 全体开发人员欢迎您的使用  \n-----------------------------------"
Lexer lexer;
Token* nowToken;
using namespace std;
void error(string tip) {
    throw string("语法分析-" + tip);
}

int cur;
void next_w() {
    nowToken = lexer.next();
#ifdef OUTDETAIL
    cout << "下一token " << nowToken->val << " " << int(nowToken->type) << endl;
#endif
    cur = lexer.getCur() - 1;
}

void Biaodashi();
void Yuju();
void Bianliangshengming();
void OG_analyse();

// 变量赋值
void Bianliangfuzhi() {
    if (nowToken->type != Token::TokenType::IDENT)
        error("贝贝，变量赋值要写标识符哦");
    action1(nowToken);
    next_w();
    if (nowToken->val != "=")
        error("贝贝，变量赋值要写等于号哦");
    next_w();
    Biaodashi();
    action5(nowToken);
}

// for循环语句
void Forxunhuanyuju() {
    action16(nowToken);
    if (nowToken->val != "(")
        error("贝贝，for循环语句要写左括号哦");
    next_w();
    Bianliangfuzhi();
    if (nowToken->val != ";")
        error("贝贝，for循环语句要写分号哦");
    action17(nowToken);
    next_w();
    Biaodashi();
    action18(nowToken);
    if (nowToken->val != ";")
        error("贝贝，for循环语句要写分号哦");
    next_w();
    Bianliangfuzhi();
    if (nowToken->val != ")")
        error("贝贝，for循环语句要写右括号哦");
    next_w();
    action19(nowToken);
    Yuju();
    action20(nowToken);
}

// 循环语句
void Xunhuanyuju() {
    action10(nowToken);
    if (nowToken->val != "(")
        error("贝贝，循环语句，要写左括号哦");
    next_w();
    Biaodashi();
    if (nowToken->val != ")")
        error("贝贝，循环语句，要写右括号哦");
    action11(nowToken);
    next_w();
    Yuju();
    action12(nowToken);
}

// 返回语句
void Fanhuiyuju() {
    Biaodashi();
    if (nowToken->val != ";")
        error("贝贝，返回语句，要写分号哦");
    next_w();
    action13(nowToken);
}

// 条件else语句
void Tiaojianelseyuju() {
    if (nowToken->val == "else") {
        action15(nowToken);
        next_w();
        Yuju();
    }
}

// 条件语句
void Tiaojianyuju() {
    if (nowToken->val != "(")
        error("贝贝，if后面要写左括号哦");
    next_w();
    Biaodashi();
    if (nowToken->val != ")")
        error("贝贝，别忘记写右括号哦");
    action6(nowToken);
    next_w();
    Yuju();
    Tiaojianelseyuju();
    action7(nowToken);
}

// 赋值语句
void Fuzhiyuju() {
    Biaodashi();
    action5(nowToken);
    if (nowToken->val != ";")
        error("贝贝，你忘记写分号了哦");
    next_w();
}

// 调用参数
void Diaoyongcanshu() {
    if (nowToken->type == Token::TokenType::CONST_STR) {
        action14(nowToken);
        next_w();
    }
    else {
        OG_analyse();
        action9(nowToken);
    }
}

// 函数调用参数组
void Hanshudiaoyongcanshuzu() {
    if (nowToken->val == ")")
        return;
    Diaoyongcanshu();
    while (nowToken->val == ",") {
        next_w();
        Diaoyongcanshu();
    }
}

// 函数调用语句
void Hanshudiaoyongyuju() {
    Hanshudiaoyongcanshuzu();
    action8(nowToken);
    if (nowToken->val != ")")
        error("贝贝，函数调用要写右括号哦");
    next_w();
    if (nowToken->val != ";")
        error("贝贝，还要记得写分号哦");
    next_w();
}

// 函数调用
void Hanshudiaoyong() {
    Hanshudiaoyongcanshuzu();
    action8(nowToken);
    if (nowToken->val != ")")
        error("贝贝，函数调用要写右括号哦");
    next_w();
}

void Yujukuai() {
    action26(nowToken);
    // 当遇到 } 说明是语句块的结尾
    while (nowToken->val != "}")
    {
        Yuju();
    }
    action27(nowToken);
}

// 数组变量声明语句
void Shuzubianliangfuzhiyuju() {
    if (nowToken->val != ";")
        error("贝贝，数组变量声明语句需要以分号结尾");
    action24(nowToken);
    next_w();
}

// 普通变量声明语句
void Putongbianliangfuzhiyuju() {
    if (nowToken->val == "=") {
        next_w();
        Biaodashi();
        action23(nowToken);
        if (nowToken->val != ";")
            error("贝贝，不要忘记写分号哦");
        next_w();
    }
    else if (nowToken->val == ";") {
        action4(nowToken);
        next_w();
    }
    else {
        error("贝贝，为什么会有一个 " + nowToken->val);
    }
}

// 变量定义语句2
void Bianliangdiyiyujuer() {
    // 数组型变量
    if (nowToken->val == "[") {
        next_w();
        if (nowToken->type != Token::TokenType::CONST_NUM)
            error("贝贝，这里我想要一个常数");
        action1(nowToken);
        next_w();
        if (nowToken->val != "]")
            error("贝贝，这里我想要一个右方括号");
        next_w();
        Shuzubianliangfuzhiyuju();
    }
    else {
        Putongbianliangfuzhiyuju();
    }
}

// 变量定义语句
void Bianliangdingyiyuju() {
    if (nowToken->type != Token::TokenType::IDENT)
        error("贝贝，变量定义语句不可以不写标识符哦");
    action1(nowToken);
    next_w();
    Bianliangdiyiyujuer();
}

// 数组项赋值语句
void Shuzuxiangfuzhiyuju() {
    // if (nowToken->type != Token::TokenType::CONST_NUM)
    //     error("贝贝，数组项赋值，要输入整数哦");
    // action1(nowToken);
    Biaodashi();
    action30(nowToken);
    if (nowToken->val != "]")
        error("贝贝，数组项赋值，要输入右方括号哦");
    next_w();
    if (nowToken->val != "=")
        error("贝贝，数组项赋值，要输入等于号哦");
    next_w();
    Biaodashi();
    action25(nowToken);
    if (nowToken->val != ";")
        error("贝贝，数组项赋值语句末尾，要输入分号哦");
    next_w();
}

// 语句
void Yuju() {
    // 变量定义语句
    if (nowToken->val == "int") {
#ifdef OUTDETAIL
        cout << "变量定义语句" << endl;
#endif
        action1(nowToken);
        next_w();
        Bianliangdingyiyuju();
    }
    else if (nowToken->val == "float") {
#ifdef OUTDETAIL
        cout << "变量定义语句" << endl;
#endif
        action1(nowToken);
        next_w();
        Bianliangdingyiyuju();
    }
    // 赋值语句或函数调用语句
    else if (nowToken->type == Token::TokenType::IDENT) {
        tokens.push(*nowToken);
        next_w();
        // 函数调用语句
        if (nowToken->val == "(") {
#ifdef OUTDETAIL
            cout << "函数调用语句" << endl;
#endif
            next_w();
            Hanshudiaoyongyuju();
        }
        // 赋值语句
        else if (nowToken->val == "=") {
#ifdef OUTDETAIL
            cout << "赋值语句" << endl;
#endif
            next_w();
            Fuzhiyuju();
        }
        // 数组项赋值语句
        else if (nowToken->val == "[") {
#ifdef OUTDETAIL
            cout << "数组项赋值语句" << endl;
#endif
            next_w();
            Shuzuxiangfuzhiyuju();
        }
    }
    // 条件语句
    else if (nowToken->val == "if") {
#ifdef OUTDETAIL
        cout << "条件语句" << endl;
#endif
        next_w();
        Tiaojianyuju();
    }
    // 循环语句
    else if (nowToken->val == "while") {
#ifdef OUTDETAIL
        cout << "循环语句" << endl;
#endif
        next_w();
        Xunhuanyuju();
    }
    // for循环语句
    else if (nowToken->val == "for") {
#ifdef OUTDETAIL
        cout << "for循环语句" << endl;
#endif
        next_w();
        Forxunhuanyuju();
    }
    // 返回语句
    else if (nowToken->val == "return") {
#ifdef OUTDETAIL
        cout << "返回语句" << endl;
#endif
        next_w();
        Fanhuiyuju();
    }
    // 跳出语句
    else if (nowToken->val == "break") {
#ifdef OUTDETAIL
        cout << "跳出语句" << endl;
#endif
        next_w();
        action21(nowToken);
        if (nowToken->val != ";")
            error("贝贝，break语句后面要写分号哦");
        next_w();
    }
    // 跳过语句
    else if (nowToken->val == "continue") {
#ifdef OUTDETAIL
        cout << "跳过语句" << endl;
#endif
        next_w();
        action22(nowToken);
        if (nowToken->val != ";")
            error("贝贝，continue语句后面要写分号哦");
        next_w();
    }
    // 语句块
    else if (nowToken->val == "{") {
        next_w();
        Yujukuai();
        if (nowToken->val != "}")
            error("贝贝，块结束要加上对应的} 哦");
        next_w();
    }
    else {
        error("贝贝，没有以" + nowToken->val + "开头的语句哦");
    }
}


// 类型
void Leixing() {
    action1(nowToken);
    if (nowToken->val == "int") {
        next_w();
    }
    else if (nowToken->val == "float") {
        next_w();
    }
    else if (nowToken->val == "char") {
        next_w();
    }
    else
        error("贝贝，" + nowToken->val + "是什么意思嘛~");
}

// 变量声明（仅声明，不含初始化）
void Bianliangshengming() {
    Leixing();
    if (nowToken->type != Token::TokenType::IDENT)
        error("贝贝，变量声明不可以不写标识符哦");
    action1(nowToken);
    next_w();
}

// 函数定义参数组
void Hanshudingyicanshuzu() {
    if (nowToken->val == ")")
        return;
    Bianliangshengming();
    action3(nowToken);
    while (nowToken->val == ",") {
        next_w();
        Bianliangshengming();
        action3(nowToken);
    }
}

// 函数定义或声明
void Hanshudingyihuoshengming() {
    // cout << "hs" << endl;
    action2(nowToken);
    // cout << "1" << endl;
    Hanshudingyicanshuzu();
    // cout << "2" << endl;
    if (nowToken->val != ")")
        error("贝贝，你忘写) 了");
    next_w();
    // cout << "3" << endl;
    if (nowToken->val == ";") {
        action29(nowToken);
        next_w();
        return;
    }
    action28(nowToken);
    if (nowToken->val != "{")
        error("我想要一个{ 球球了");
    next_w();
    Yujukuai();
    if (nowToken->val != "}")
        error("贝贝，你忘写} 了");
    next_w();
    qualters.add("endfunc");
}

// 外部内容
void Waibuneirong() {
    Leixing();
    if (nowToken->type != Token::TokenType::IDENT)
        error("贝贝，记得要写标识符哦");
    action1(nowToken);
    next_w();
    if (nowToken->val == "(") {
        next_w();
        Hanshudingyihuoshengming();
    }
    else {
        Bianliangdiyiyujuer();
    }
}

// 程序
void Chengxu() {
    while (nowToken->type != Token::TokenType::END)
        Waibuneirong();
}

// 表达式
void Biaodashi() {
    OG_analyse();
}

// 算符优先关系表
char orderBetween(string o1s, string o2s) {
    char o1 = o1s[0];
    char o2 = o2s[0];
    //   ... ia o1 ib o2 ...
    if (o1 == '#' && o2 == '#')return '=';
    if (o1 == '#' && o2 == ')')return 'e';
    else if (o1 == '#')return '<';
    else if (o2 == '#')return '>';

    if (o1 == '(' && o2 == ')')return '=';
    else if (o2 == '(')return '<';
    else if (o2 == ')')return '>';
    else if (o1 == '(')return '<';

    if (o1 == '*' || o1 == '/')return '>';
    else if (o2 == '*' || o2 == '/')return '<';
    else return '>';
    // cout << "比较" << o1s << " " << o2s << endl;
    return 'e';
}

int tid = 0;
stack<string> optr; //运算符栈
set<string> VT = { "+", "-", "*", "/", "(", ")","=","==","!=",">","<",">=","<=",  "#" }; // 终极符表
int tempid = 0;
// 算符优先分析
void OG_analyse() {
#ifdef OUTDETAIL
    cout << "进入算符优先分析" << endl;
#endif
    optr.push("#");
    int endFlag = 0;
    // next_w();
    while (!optr.empty()) {
        string cur_opt = nowToken->val;
        if (endFlag) cur_opt = "#";
        // 为终极符，比较算符优先级
        if (VT.count(cur_opt) == 1) {
            // cout << "terminal " << now_w << endl;
            // pair<string, string> key = make_pair(optr.top(), get_token(now_w));
            // if (OGMAP.count(key) == 1) {
            char order;
            if ((order = orderBetween(optr.top(), cur_opt)) != 'e') {
                // 栈内优先级高，规约
                if (order == '>') {
#ifdef OUTDETAIL
                    cout << "规约" << endl;
#endif
                    if (opnd.size() < 2) error("运算数栈 < 2， 天，怎么会遇到这个： " + nowToken->val);
                    Token val2 = opnd.top(); opnd.pop();
                    Token val1 = opnd.top(); opnd.pop();

                    SymbLPointer sid1 = symbL.findToken(&val1, layer);
                    SymbLPointer sid2 = symbL.findToken(&val2, layer);
                    SymbLPointer sid3 = symbL.addVariable("t" + to_string(tid++), ValType::INT, layer);

                    Token result(Token::TokenType::IDENT, symbL.get(sid3).ident, 0);
                    opnd.push(result);

                    qualters.add(optr.top(), sid1, sid2, sid3);

                    if (optr.size() < 1) error("运算符栈 < 1， 天，怎么会遇到这个： " + nowToken->val);
                    optr.pop();
                }
                else if (order == '=') {
                    // cout << "弹出" << endl;
                    if (optr.size() < 1) error("天，怎么会遇到这个： " + nowToken->val);
                    optr.pop();
                    if (endFlag == 0)next_w();
                }
                // 栈内优先级低，移进
                else if (order == '<') {
#ifdef OUTDETAIL
                    cout << "移进算符" << nowToken->val << endl;
#endif
                    optr.push(nowToken->val);
                    next_w();
                }
            }
            else if (endFlag == 0) {
                // 遇到非表达式内容，完成表达式的计算并退出算符优先分析
                endFlag = 1;
            }
            else {
                error("天，怎么会遇到这个： " + string(string("") + optr.top()) + " : " + nowToken->val);
            }
        }
        else if (nowToken->type == Token::TokenType::IDENT || nowToken->type == Token::TokenType::CONST_NUM || nowToken->type == Token::TokenType::CONST_FLOAT) {
#ifdef OUTDETAIL
            cout << "token type " << int(nowToken->type) << endl;
#endif

            // 是函数标识符
            if (nowToken->type == Token::TokenType::IDENT && symbL.findFunc(nowToken->val) != -1) {
                action1(nowToken);
                next_w();
                if (nowToken->val != "(")
                    error("贝贝，函数标识符后面要写左括号哦");
                next_w();
                Hanshudiaoyong();
                opnd.push(Token(Token::TokenType::IDENT, string("gtret") + to_string(tempid), 0));
                int sid = symbL.addVariable(string("gtret") + to_string(tempid), ValType::INT, layer);
                qualters.add("gtret", sid);
                tempid++;
            }
            // 是数组标识符
            else if (nowToken->type == Token::TokenType::IDENT && symbL.findArray(nowToken->val) != -1) {
                action1(nowToken);
                int sid = symbL.findArray(nowToken->val);
                next_w();
                if (nowToken->val != "[")
                    error("贝贝，数组标识符后面要写 \"[\" 哦");
                next_w();
                Biaodashi();

                if (nowToken->val != "]")
                    error("贝贝，记得写 \"]\" 哦");
                next_w();
                string ident = tokens.top().val; tokens.pop();
                SymbLPointer posid = symbL.findToken(&(opnd.top()), layer); opnd.pop();
                opnd.push(Token(Token::TokenType::IDENT, symbL.get(sid).ident, posid));
            }
            // 是其他标识符
            else if (nowToken->type == Token::TokenType::IDENT) {
                opnd.push(*nowToken);
#ifdef OUTDETAIL
                cout << "运算数入栈：" << nowToken->val << endl;
#endif
                next_w();
            }
            // 是常量
            else {
                SymbLPointer sid3 = symbL.addConst("t" + to_string(tid++), ValType::INT, nowToken->val, layer);
                Token result(Token::TokenType::IDENT, symbL.get(sid3).ident, 0);
                opnd.push(result);
#ifdef OUTDETAIL
                cout << "运算数入栈：" << nowToken->val << endl;
#endif
                next_w();
            }
        }
        else {
            endFlag = 1;
        }
    }
#ifdef OUTDETAIL
    cout << "退出算符优先分析" << endl;
#endif
}





// 主控程序入口
void LL1_recursive() {
    next_w();
    Chengxu();
    if (nowToken->type == Token::TokenType::END) {}
    else
        error("已退出，多余的符号： " + nowToken->val);
}


#include <chrono>   
#include "Macroproce.hpp"
using namespace std;
using namespace chrono;
char temp[1000000];
int main() {
    cout << DECRIPTION << endl;
    string filename = FILE_NAME;
    int auto_nasm = 1;
    int auto_link = 1;
    int auto_run = 1;
    ifstream is(string(FILE_DIR) + FILE_NAME);
#ifdef RELEASE
    cout << "请输入hcc文件名：";
    cin >> filename;
    is.open(filename.c_str());
#endif
    string code = "";
    // string temp;
    while (is.getline(temp, 1000000)) {
        code += temp;
        code += "\n";
    }

    code = macro(code); // 进行宏处理
    
    // cout << code << endl;
    cur = -1;
    lexer.loadCode(code);
    try {
        LL1_recursive();

        // 更新符号表活动记录指针
        for (int i = 0; i < symbL.size(); ++i) {
            if (symbL.get(i).identType == IdentType::FUNCTION) {
                symbL.getDisplay(symbL.get(i).ident);
            }
        }

        ofstream os("orgin.fq", ios::binary);
        qualters.display(os);
        os.close();

        qualters = youhua(qualters);

        os.open("greater.fq", ios::binary);
        qualters.display(os);
        os.close();

        os.open("symbl.table", ios::binary);
        symbL.display(os);
        os.close();

        os.open("constl.table", ios::binary);
        constL.display(os);
        os.close();

        os.open("arrayl.table", ios::binary);
        arrayL.display(os);
        os.close();

        os.open("funcl.table", ios::binary);
        funcL.display(os);
        os.close();

        qualters.makeASM(OUT_DIR + filename + ".asm");

        cout << "---asm生成完毕" << endl;
        if (auto_nasm) {
            cout << "---编译asm" << endl;
            system(((const char*)(string("nasm.exe -f win64 "  + string(OUT_DIR) + filename + ".asm").c_str())));
            cout << "---asm编译完毕" << endl;
            if (auto_link) {
                cout << "---生成可执行文件" << endl;
                system(string("gcc " + string(OUT_DIR) + filename + ".obj -o " + string(OUT_DIR) + filename + ".exe").c_str());
                cout << "---可执行文件已生成" << endl;
                if (auto_run) {
                    cout << "---开始运行" << endl;
                    auto start = system_clock::now();
                    system(string(string("cd ") + OUT_DIR + "&&\"" + filename + ".exe" + "\"").c_str());
                    cout << "---结束运行 ";
                    auto end = system_clock::now();
                    auto duration = duration_cast<microseconds>(end - start);
                    cout << "用时"
                        << double(duration.count()) * microseconds::period::num / microseconds::period::den
                        << "秒" << endl;
                }
            }
        }
    }
    catch (string e) {
        cout << "error: " << e << endl;
        cout << "行：" << lexer.getLine() << " 列：" << lexer.getCol() << endl;
        int new_cur;
        string line = lexer.getLine(new_cur);
        cout << line << endl;
        for (int i = 0; i < line.size() + 1; ++i) {
            if (i >= new_cur - nowToken->val.size() && i < new_cur) cout << "~";
            else cout << " ";
        }
        cout << endl;
    }
    return 0;
}