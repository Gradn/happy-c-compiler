#ifndef _SYYUANSHI
#define _SYYUANSHI
#include <iostream>
#include "Lexer.hpp"
#include <vector>
#include <fstream>
#include <stack>
#include "Fuhaobiao.hpp"
using namespace std;
#include <sstream>
class ASM {
public:
    struct ASMItem
    {
        string commond; // 汇编指令
        string opnd1; // 操作数1
        string opnd2; // 操作数2
    };
    vector<ASMItem*> asms;
    ASMItem* add(string commond, string opnd1 = "", string opnd2 = "") {
        ASMItem* newItem = new ASMItem{ commond, opnd1, opnd2 };
        asms.push_back(newItem);
        return newItem;
    }
    void write(string filename) {
        ofstream os(filename, ios::binary);
        for (vector<ASMItem*>::iterator it = asms.begin(); it != asms.end(); ++it) {
            os << (*it)->commond << " " << (*it)->opnd1;
            if ((*it)->opnd2 != "")
                os << ", " << (*it)->opnd2;
            os << endl;
        }
    }
};

class Siyuanshi {
public:
    struct STRUCT {
        string opt;
        SymbLPointer val1;
        SymbLPointer val2;
        SymbLPointer result;
    };

    Siyuanshi() {
        this->interInsert = false;
    }
    
    
    vector<STRUCT> qualters;

    vector<STRUCT>::iterator interPos;
    stack<string> whilestart;

    stack<int> curinwhile; // 当前循环是for循环还是while循环

    bool interInsert;
    vector<STRUCT>::iterator getIterator() {
        return qualters.end();
    }
    
    // 在当前位置设置断点，之后的add均在此处插入
    void setInterupt(vector<STRUCT>::iterator pos) {
        this->interPos = pos;
        this->interInsert = true;
    }

    // 取消设置断点
    void cancelInterupt() {
        this->interInsert = false;
    }

    void add(string opt, SymbLPointer val1, SymbLPointer val2, SymbLPointer result) {
        if (interInsert) 
            qualters.insert(interPos, STRUCT{ opt, val1, val2, result });
        else
            qualters.push_back(STRUCT{ opt, val1, val2, result });
    }
    void add(string opt, SymbLPointer val1, SymbLPointer result) {
        if (interInsert) 
            qualters.insert(interPos, STRUCT{ opt, val1, -1, result });
        else
            qualters.push_back(STRUCT{ opt, val1, -1, result });
    }
    void add(string opt, SymbLPointer val1) {
        if (interInsert) 
            qualters.insert(interPos, STRUCT{ opt, val1, -1, -1 });
        else
            qualters.push_back(STRUCT{ opt, val1, -1, -1 });
    }
    void add(string opt) {
        if (interInsert) 
            qualters.insert(interPos, STRUCT{ opt, -1, -1, -1 });
        else
            qualters.push_back(STRUCT{ opt, -1, -1, -1 });
    }

    void display(ofstream& os) {
        os << "四元式序列为：" << endl;
        for (int i = 0; i < qualters.size(); ++i) {
            os << qualters[i].opt << "\t" << qualters[i].val1 << "\t" << qualters[i].val2 << "\t" << qualters[i].result << endl;
        }
    }

    STRUCT get(int i){
        return qualters[i];
    }

    int size(){
        return qualters.size();
    }
    
    int paraNum = 0;
    string getParaRegister() {
        string strs[] = { "rcx", "rdx", "r8", "r9", "r10", "r11" };
        return strs[paraNum++] ;
    }
    stack<vector<STRUCT>::iterator> iters;
    string toAsmVal(SymbL::Item sitem) {
        if (sitem.identType == IdentType::CONST && sitem.valType == ValType::INT)
            return "[const" + to_string(sitem.pointer) + "]";
        else if (sitem.identType == IdentType::CONST && sitem.valType == ValType::STRING)
            return "const" + to_string(sitem.pointer);
        return "[rsp+" + to_string(sitem.pointer) + "]";
    }

    string arr2AsmVal(SymbLPointer sid, int arrayPos) {
        return "[rsp+" + to_string(symbL.getOffset(sid, arrayPos)) + "]";
    }

    void makeASM(string filename) {
        stack<ASM::ASMItem*> asmstack; // 需要反填跳转位置的asm语句
        stack<ASM::ASMItem*> fantianbreak; // 需要反填跳转位置的break语句
        stack<ASM::ASMItem*> fantiancontinue; // 需要反填跳转位置的continue语句
        stack<string> flagstack; // 需要临时保存的跳转到语句
        ASM asms;
        asms.add(";本文件由HCC编译器生成");
        asms.add("        global main");
        asms.add("        extern printf");
        asms.add("section .data");
        asms.add("format db \"%d \", 0");
        // 初始化常量
        for (int i = 0; i < constL.size(); ++i) {
            if (constL.get(i).type == CONST_STR) {
                string ans = "";
                for (char c : constL.get(i).val) {
                    ans += to_string(int(c)) + ", ";
                }
                ans += "0";
                asms.add("const" + to_string(i) + " db " + ans);
                // asms.add("const" + to_string(i) + " db " + "\"" + constL.get(i).val + "\"");
            }
            else
                asms.add("const" + to_string(i) + " dq " + constL.get(i).val);
        }
        // 初始化全局变量
        for (int i = 0; i < symbL.size(); ++i) {
            if (symbL.get(i).layer == 0 && symbL.get(i).identType == IdentType::VARIABLE) {
                asms.add("globl" + symbL.get(i).ident + " dq 0");
            }
        }
        asms.add("section .text");

        int flag_ = 0;
        int flagID = 0;
        int stacksize;
        for (vector<STRUCT>::iterator it = qualters.begin(); it != qualters.end(); ++it) {
            SymbL::Item val1 = symbL.get(it->val1);
            SymbL::Item val2 = symbL.get(it->val2);
            SymbL::Item result = symbL.get(it->result);
            asms.add(";四元式：\t" + it->opt + "\t" + val1.ident + "\t" + val2.ident + "\t" + result.ident);
            if (it->opt == "+") {
                asms.add("mov", "rax", toAsmVal(val1));
                asms.add("add", "rax", toAsmVal(val2));
                asms.add("mov", toAsmVal(result), "rax");
            }
            else if (it->opt == "-") {
                asms.add("mov", "rax", toAsmVal(val1));
                asms.add("sub", "rax", toAsmVal(val2));
                asms.add("mov", toAsmVal(result), "rax");
            }
            else if (it->opt == "*") {
                asms.add("mov", "eax", toAsmVal(val1));
                asms.add("mov","ebx",toAsmVal(val2));
                asms.add("mul", "ebx");
                asms.add("mov", toAsmVal(result), "eax");
            }
            else if (it->opt == "=") {
               // asms.add("xor", "eax", "eax");
                asms.add("mov", "rax", toAsmVal(val1));
                asms.add("mov", toAsmVal(result), "rax");
            }
            else if(it->opt=="=="){
                asms.add("mov","rax",toAsmVal(val1));
                asms.add("mov","rbx",toAsmVal(val2));
                asms.add("cmp","rax","rbx");
                string fff="flag_dayu"+to_string(flag_++);
                asms.add("jz",fff);
                asms.add("mov","rax","0");
                asms.add("mov",toAsmVal(result),"rax");
                string fff1="flag_dayu"+to_string(flag_++);
                asms.add("jmp",fff1);
                asms.add(fff,":");
                asms.add("mov","rax","1");
                asms.add("mov",toAsmVal(result),"rax");
                asms.add(fff1,":");
            }
            else if(it->opt=="!="){
                asms.add("mov","rax",toAsmVal(val1));
                asms.add("mov","rbx",toAsmVal(val2));
                asms.add("cmp","rax","rbx");
                string fff="flag_dayu"+to_string(flag_++);
                asms.add("jz",fff);
                asms.add("mov","rax","1");
                asms.add("mov",toAsmVal(result),"rax");
                string fff1="flag_dayu"+to_string(flag_++);
                asms.add("jmp",fff1);
                asms.add(fff,":");
                asms.add("mov","rax","0");
                asms.add("mov",toAsmVal(result),"rax");
                asms.add(fff1,":");
            }
            else if(it->opt==">"){
                asms.add("mov","rax",toAsmVal(val1));
                asms.add("mov","rbx",toAsmVal(val2));
                asms.add("cmp","rax","rbx");
                string fff="flag_dayu"+to_string(flag_++);
                asms.add("jg",fff);
                asms.add("mov","rax","0");
                asms.add("mov",toAsmVal(result),"rax");
                string fff1="flag_dayu"+to_string(flag_++);
                asms.add("jmp",fff1);
                asms.add(fff,":");
                asms.add("mov","rax","1");
                asms.add("mov",toAsmVal(result),"rax");
                asms.add(fff1,":");
            }
            else if (it->opt == "<") {
                asms.add("xor", "rax", "rax");
                asms.add("xor", "rbx", "rbx");
                asms.add("mov", "rax", toAsmVal(val1));
                asms.add("mov","rbx",toAsmVal(val2));
                asms.add("cmp","rax","rbx");
                string fff="flag_dayu"+to_string(flag_++);
                asms.add("jl",fff);
                asms.add("mov","rax","0");
                asms.add("mov",toAsmVal(result),"rax");
                string fff1="flag_dayu"+to_string(flag_++);
                asms.add("jmp",fff1);
                asms.add(fff,":");
                asms.add("mov","rax","1");
                asms.add("mov",toAsmVal(result),"rax");
                asms.add(fff1,":");
            }
              
                else if(it->opt=="<="){
                asms.add("mov","rax",toAsmVal(val1));
                asms.add("mov","rbx",toAsmVal(val2));
                asms.add("cmp","rax","rbx");
                string fff="flag_dayu"+to_string(flag_++);
                asms.add("jle",fff);
                asms.add("mov","rax","0");
                asms.add("mov",toAsmVal(result),"rax");
                string fff1="flag_dayu"+to_string(flag_++);
                asms.add("jmp",fff1);
                asms.add(fff,":");
                asms.add("mov","rax","1");
                asms.add("mov",toAsmVal(result),"rax");
                asms.add(fff1,":");
            }
                else if(it->opt==">="){
                asms.add("mov","rax",toAsmVal(val1));
                asms.add("mov","rbx",toAsmVal(val2));
                asms.add("cmp","rax","rbx");
                string fff="flag_dayu"+to_string(flag_++);
                asms.add("jge",fff);
                asms.add("mov","rax","0");
                asms.add("mov",toAsmVal(result),"rax");
                string fff1="flag_dayu"+to_string(flag_++);
                asms.add("jmp",fff1);
                asms.add(fff,":");
                asms.add("mov","rax","1");
                asms.add("mov",toAsmVal(result),"rax");
                asms.add(fff1,":");
            }
            // result[val2] = val1
            else if (it->opt == "[]=") {
                asms.add("mov", "rax", toAsmVal(val2));
                asms.add("mov", "ebx", to_string(arrayL.getOneSize(symbL.getPointer(it->result))));
                asms.add("mul", "ebx");
                asms.add("add", "rax", to_string(symbL.getOffset(it->result)));
                asms.add("add", "rax", "rsp");
                asms.add("mov", "rbx", toAsmVal(val1));
                asms.add("mov", "[rax]", "rbx");
                // // result[val2] = val1
                // asms.add("mov", "rbx", toAsmVal(val1));
                // asms.add("mov", toAsmVal(result), "rbx");
                // asms.add("mov", arr2AsmVal(it->result, int(it->val2)), "rax");
            }
            else if (it->opt == "=[]") {
                asms.add("mov", "rax", toAsmVal(val2));
                asms.add("mov","rbx", to_string(arrayL.getOneSize(symbL.getPointer(it->val1))));
                asms.add("mul", "rbx");
                asms.add("add", "rax",to_string(symbL.getOffset(it->val1)));
                asms.add("add", "rax", "rsp");
                asms.add("mov", "rbx", "[rax]");
                asms.add("mov", toAsmVal(result), "rbx");
            }
            
            // else if(it->opt=="/"){
            //      // r1 % r2 = r1 - (r1 / r2) * r2
            //     asms.add("mov", "edx", "0");
            //     asms.add("mov", "edx", toAsmVal(val1));
            //     asms.add("mov", "rbx", toAsmVal(val2));
            //     asms.add("div", "rax", toAsmVal(val2));
            //     asms.add("mov","rbx",toAsmVal(val2));
            //     asms.add("mul", "rbx");
            //     asms.add("mov", toAsmVal(val2), "rax");
            //     asms.add("mov", "rax", toAsmVal(val1));
            //     asms.add("sub", "rax", toAsmVal(val2));
            //     asms.add("mov", toAsmVal(result), "rax");
            // }
       
            else if (it->opt == "if") {
                asms.add("mov", "rax", toAsmVal(val1));
                asms.add("cmp", "rax", "0");
                asmstack.push(asms.add("je", "else_pos"));
            }
            else if (it->opt == "else") {
                ASM::ASMItem* ifitem = asmstack.top(); asmstack.pop();
                asmstack.push(asms.add("jmp", "edif_pos"));
                string flag = "FLAG" + (to_string(flagID++));
                asms.add(flag + ":");
                ifitem->opnd1 = flag;
            }
            else if (it->opt == "edif") {
                string flag = "FLAG" + (to_string(flagID++));
                asms.add(flag + ":");
                asmstack.top()->opnd1 = flag;
                asmstack.pop();
            }
            else if (it->opt == "wl") {
                fantianbreak.push(0);
                fantiancontinue.push(0);
                curinwhile.push(1);
                string flag = "FLAG" + (to_string(flagID++));
                flagstack.push(flag);
                asms.add(flag + ":");
                whilestart.push(flag);
            }
            else if (it->opt == "do") {
                asms.add("mov", "rax", toAsmVal(val1));
                asms.add("cmp", "rax", "0");
                asmstack.push(asms.add("je", "edwl"));
            }
            else if (it->opt == "edwl") {
                string flag = "FLAG" + (to_string(flagID++));
                asms.add("jmp", flagstack.top()); flagstack.pop();
                asms.add(flag + ":");
                asmstack.top()->opnd1 = flag;
                asmstack.pop();
                // xunhuanstart.pop();
                while ((!fantianbreak.empty())) {
                    if (fantianbreak.top() == nullptr) {
                        fantianbreak.pop();
                        break;
                    }
                    fantianbreak.top()->opnd1 = flag;
                    fantianbreak.pop();
                }
                curinwhile.pop();
                whilestart.pop();
            }
            else if (it->opt == "forinit") {
                asms.add(";", "不需要做任何操作");
            }
            else if (it->opt == "for") {
                fantianbreak.push(0);
                fantiancontinue.push(0);
                curinwhile.push(0);
                string flag = "FLAG" + (to_string(flagID++));
                flagstack.push(flag);
                asms.add(flag + ":");
            }
            else if (it->opt == "fordo") {
                asms.add("mov", "rax", toAsmVal(val1));
                asms.add("cmp", "rax", "0");
                asmstack.push(asms.add("je", "edfor"));
                // 保存当前语句位置
                iters.push(it);
                // 向下扫描，暂时跳过foriter前语句
                for (it; it != qualters.end(); ++it) {
                    if (it->opt == "foriter") break;
                }
            }
            else if (it->opt == "foriter") {
                // 向下扫描，直到edfor语句
                for (it; it != qualters.end(); ++it) {
                    if (it->opt == "edfor") break;
                }
                string flag = "FLAG" + (to_string(flagID++));
                asms.add("jmp", flagstack.top()); flagstack.pop();
                asms.add(flag + ":");
                asmstack.top()->opnd1 = flag;
                asmstack.pop();
                // xunhuanstart.pop();
                while ((!fantianbreak.empty())) {
                    if (fantianbreak.top() == nullptr) {
                        fantianbreak.pop();
                        break;
                    }
                    fantianbreak.top()->opnd1 = flag;
                    fantianbreak.pop();
                }
            }
            else if (it->opt == "edfor") {
                // 打上foriter标记
                asms.add(";", "foriter起始");
                string flag = "FLAG" + (to_string(flagID++));
                asms.add(flag, ":");
                // 在此输出foriter前语句
                it = iters.top(); iters.pop();
                curinwhile.pop();
                while ((!fantiancontinue.empty())) {
                    if (fantiancontinue.top() == nullptr) {
                        fantiancontinue.pop();
                        break;
                    }
                    fantiancontinue.top()->opnd1 = flag;
                    fantiancontinue.pop();
                }
            }
            else if (it->opt == "func") {
                asms.add(val1.ident + ":");
                // 分配栈帧
                stacksize = symbL.getDisplay(val1.ident);
                // 分配栈内存
                asms.add(";", "保存栈指针");
                // asms.add("mov", "rbx", "rsp");
                asms.add(";", "分配栈帧");
                asms.add("sub", "rsp", to_string(stacksize));

                // 初始化栈帧
                asms.add(";", "初始化栈帧");
                // 将所有常量符号值保存到栈帧中
                SymbLPointer sid = it->val1;
                for (int i = sid + 1; i < symbL.size(); ++i) {
                    SymbL::Item item = symbL.get(i);
                    if (item.identType == IdentType::CONST) {
                        int constLID = item.pointer;
                    }
                }
                // 将所有参数保存到栈帧中
                int funcID = symbL.getPointer(sid); // 函数表指针
                vector<int> pointers = funcL.get(funcID).xingcan;
                for (int i = 0; i < pointers.size(); ++i) {
                    SymbL::Item sitem = symbL.get(pointers[i]);
                    asms.add("mov", toAsmVal(sitem), "rcx");
                }
            }
            else if (it->opt == "endfunc") {
                asms.add(";", "恢复栈指针");
                asms.add("add", "rsp", to_string(stacksize));
                asms.add("ret");
            }
            else if (it->opt == "call") {
                paraNum = 0;
                // 参数传递
                // asms.add("mov", "[n]", "r10");
                if (val1.ident == "hccout") {
                    asms.add("lea rcx, [format]");
                    // asms.add("mov edx, " + toAsmVal(val1));
                    asms.add("sub rsp, 64");
                    asms.add("call printf");
                    asms.add("add rsp, 64");
                }
                else {
                    asms.add("sub rsp, 64");
                    asms.add("call", val1.ident);
                    asms.add("add rsp, 64");
                }
            }
            else if (it->opt == "para") {
                // asms.add("push rip");
                asms.add("mov", getParaRegister(), toAsmVal(val1));
            }
            else if (it->opt == "ret") {
                asms.add(";", "恢复栈指针");
                asms.add("mov", "rax", toAsmVal(val1));
                asms.add("add", "rsp", to_string(stacksize));
                // asms.add("push rip");
                asms.add("ret");
            }
            else if (it->opt == "gtret") {
                // asms.add("push rip");
                asms.add("mov", toAsmVal(val1), "rax");
            }
            else if (it->opt == "continue") {
                asms.add(";", "跳过当前轮");
                // 当前在while体内，可直接获取start位置
                if (curinwhile.top() == 1) {
                    asms.add("jmp", whilestart.top());
                }
                // 当前在for体内，需要反填位置
                else {
                    fantiancontinue.push(asms.add("jmp", "foriter pos"));
                }
            }
            else if (it->opt == "break") {
                asms.add(";", "跳出循环");
                fantianbreak.push(asms.add("jmp", "edfor or edwl pos"));
            }
        }
        asms.add("runok:");
        asms.add("        db \"运行结束\", 0");
        asms.write(filename);
    }
};


Siyuanshi qualters;
#endif