#ifndef _FUHAOBIAO
#define _FUHAOBIAO
#include <vector>
#include <iostream>
#include "Global.hpp"
using namespace std;

string identtype[] = { "函数", "常量", "变量", "类型", "形参" };
string blocktype[] = { "参数", "内部", "临时", "全局" };
string valtype[] = { "整型", "浮点型", "字符型", "数组型", "结构型", "无返回值", "字符串" };
enum class IdentType { FUNCTION, CONST, VARIABLE, CLASS, FUNCPARA };
enum class BlockType { FUNCPARA, FUNCIN, TEMP, GLOBAL };
enum class ValType { INT, FLOAT, CHAR, ARR, STRUCT, VOID, STRING };

// 活动记录
class Display {
public:
    struct Item {
        int oldSP;
        int paraNum;
        int xingcan; // 形参存储区长度
        int jubu; // 局部变量存储区长度
        int temp; // 临时单元存储区
    };
    // 一个函数的活动记录
    vector<Item> _display;
    // 返回一个新的空活动记录的指针
    int newDisplay() {
        _display.push_back(Item());
        return _display.size() - 1;
    }
    // 设置主调过程的活动记录首地址
    void setOldSP(int displayID, int oldsp) {
        _display[displayID].oldSP = oldsp;
    }
    // 返回地址不需记录，直接使用ret即可
    // // 设置参数个数
    // void setParaNum(int displayID, int paranum) {
    //     _display[displayID].paraNum = paranum;
    // }
    // 形式单元大小（形参存储区）
    void addPara(int displayID, int type) {
        _display[displayID].paraNum++;
        _display[displayID].xingcan += 4;
    }
    // 局部变量存储区
    void addJubu(int displayID, int type) {
        _display[displayID].jubu += 4;
    }
    // 内情向量存储区
    // 临时单元存储区
    void addTemp(int displayID, int type) {
        _display[displayID].temp += 4;
    }

};

// 活动记录
Display displayL;

// 函数表
class FuncL {
public:
    struct FuncItem {
        FuncItem():defined(0) {}
        // 若为函数，则有以下函数表项
        vector<SymbLPointer> xingcan; // 形参
        int pos; // 指向活动记录中存储位置
        int defined; // 是否已经定义，有可能只有声明
    };
    vector<FuncItem> _FuncL; // 函数表
    // 添加一个空的新项
    int newItem(int displayid) {
        FuncItem f;
        f.pos = displayid;
        _FuncL.push_back(f);
        return _FuncL.size() - 1;
    }
    void add(int id, SymbLPointer sid) {
        _FuncL[id].xingcan.push_back(sid);
    }
    void setDefined(int id) {
        _FuncL[id].defined = 1;
    }
    int getPointer(int id) {
        return _FuncL[id].pos;
    }
    FuncItem get(int id) {
        return _FuncL[id];
    }
    void display(ostream& os) {
        os << "函数表如下：" << endl;
        for (int i = 0; i < _FuncL.size(); ++i) {
            os << i << "\t";
            for (auto xc : _FuncL[i].xingcan) {
                os << xc << ", ";
            }
            os << endl;
        }
    }
};

FuncL funcL; // 函数表




// 数组表
class ArrayL {
public:
    // 数组表项
    struct Item {
        int num; // 数组中元素个数
        ValType valType; // 数组中元素值类型
        int pointer; // 第一个表项在活动记录中位置
    };
    vector<Item> _ArrayL; // 数组表
    int add(int num, ValType valType) {
        _ArrayL.push_back(Item{ num, valType });
        return _ArrayL.size() - 1;
    }
    int size() {
        return _ArrayL.size();
    }

    // 获取数组中元素存储需要空间
    int getOneSize(int id) {
        // 默认都是整型
        int oneSize = 64;
        return oneSize;
    }

    // 获取栈帧大小
    int getDisplay(int id) {
        int oneSize = getOneSize(id);
        return _ArrayL[id].num * oneSize;
    }

    Item& get(int i) {
        return _ArrayL[i];
    }
    void display(ostream& os) {
        os << "数组表如下：" << endl;
        for (int i = 0; i < _ArrayL.size(); ++i) {
            os << i << "\t" << _ArrayL[i].num << "\t" << int(_ArrayL[i].valType) << endl;
        }
    }
};

ArrayL arrayL; // 数组表


// 常量值表（非符号表）
class ConstL {
public:
    // 常量表项
    struct Item {
        int type; // 常量类型
        string val; // 常量值
        // 在活动记录中的指针
    };
    vector<Item> _ConstL; // 常量表
    int add(int type, string val) {
        _ConstL.push_back(Item{ type, val });
        return _ConstL.size() - 1;
    }
    int size() {
        return _ConstL.size();
    }
    Item get(int i) {
        return _ConstL[i];
    }
    void display(ostream& os) {
        os << "常量表如下：" << endl;
        for (int i = 0; i < _ConstL.size(); ++i) {
            os << i << "\t" << _ConstL[i].val << "\t" << _ConstL[i].type << endl;
        }
    }
};

ConstL constL;


// 符号表总表
class SymbL {
public:
    // 符号表表项
    struct Item {
        Item(string ident, IdentType identType, BlockType blockType, ValType valType, int layer, int pointer = 0) :ident(ident), identType(identType), blockType(blockType), valType(valType), pointer(pointer), layer(layer){}
        string ident; // 符号名字
        IdentType identType; // 标识符种类
        BlockType blockType; // 块类型
        ValType valType; // 返回值类型
        int layer; // 符号所处层级
        int pointer; // 指针
    };
    int curDisplay; // 当前活动记录（初始时为全局活动记录）
    SymbL() {
        curDisplay = displayL.newDisplay();
        init();
    }
    vector<Item> _SymbL; // 符号表总表


    // 初始化符号表，添加常用函数，仅临时用于调试，不应包含在最终版中
    void init() {
        addFunction("printf", ValType::INT, 0);
    }

    // 向上寻找
    int findSymb(string ident) {
        // 在当前函数中寻找符号
        for (int i = _SymbL.size() - 1; i >= 0; --i) {
            if (_SymbL[i].ident == ident) return i;
        }
        return -1;
    }

    // 向上寻找，若为常量，则加入常量表
    int findToken(Token* token, int layer) {
        // 若是一个标识符
        if (token->type == Token::TokenType::IDENT) {
            // 在当前函数中寻找符号
            for (int i = _SymbL.size() - 1; i >= 0; --i) {
                if (_SymbL[i].ident == token->val) return i;
            }
            // 不存在则加入
            return addVariable(token->val, ValType::INT, layer);
        }
        else {
            return addTempConst(ValType::INT, token->val, layer);
        }
        return -1;
    }

    // 检查重定义
    int redefine(string ident, int strict = 0) {
        // 在当前函数中寻找是否重定义
        for (int i = _SymbL.size() - 1; i >= 0; --i) {
            if (_SymbL[i].ident == ident) return 1;
            if (_SymbL[i].identType == IdentType::FUNCTION)break;
        }
        return 0;
    }

    // 添加变量符号，用于用户定义变量
    int addVariable(string ident, ValType valType, int layer) {
        _SymbL.push_back(Item(ident, IdentType::VARIABLE, BlockType::FUNCIN, valType, layer));
        return _SymbL.size() - 1;
    }

    // 添加常量符号，用于用户定义常量
    int addConst(string ident, ValType valType, string val, int layer) {
        int ctype;
        if (valType == ValType::STRING) ctype = CONST_STR;
        else ctype = CONST_INT;
        int pointer = constL.add(ctype, val);
        _SymbL.push_back(Item(ident, IdentType::CONST, BlockType::FUNCIN, valType, layer, pointer));
        return _SymbL.size() - 1;
    }

    // 添加临时常量符号
    int addTempConst(ValType valType, string val, int layer) {
        int pointer = constL.add(CONST_INT, val);
        _SymbL.push_back(Item(string("contmp") + to_string(pointer), IdentType::CONST, BlockType::TEMP, valType, layer, pointer));
        return _SymbL.size() - 1;
    }


    // 添加函数符号，用于用户定义函数
    int addFunction(string ident, ValType valType, int layer) {
        curDisplay = displayL.newDisplay();
        int funcp = funcL.newItem(curDisplay);
        _SymbL.push_back(Item(ident, IdentType::FUNCTION, BlockType::GLOBAL, valType, layer, funcp));
        return _SymbL.size() - 1;
    }

    // 添加结构符号，用于结构体
    int addStruct(string ident, ValType valType, int layer) {
        // ...
        return _SymbL.size() - 1;
    }

    // 添加数组符号，用于用户数组
    int addArray(string ident, ValType valType, int num, int layer) {
        int arrayp = arrayL.add(num, valType);
        _SymbL.push_back(Item(ident, IdentType::VARIABLE, BlockType::FUNCIN, ValType::ARR, layer, arrayp));
        return _SymbL.size() - 1;
    }

    // 设置符号指针
    void setPointer(int id, int pointer) {
        _SymbL[id].pointer = pointer;
    }

    // 获取符号指针
    int getPointer(int id) {
        return _SymbL[id].pointer;
    }

    // 显示符号表
    void display(ostream& os) {
        os << "符号表如下：" << endl;
        for (int i = 0; i < _SymbL.size(); ++i) {
            os << i << "\t";
            os << _SymbL[i].ident << "\t";
            os << (identtype[int(_SymbL[i].identType)]) << "\t";
            os << (blocktype[int(_SymbL[i].blockType)]) << "\t";
            os << (valtype[int(_SymbL[i].valType)]) << "\t";
            os << _SymbL[i].layer << "\t";
            os << _SymbL[i].pointer << endl;
        }
    }

    // 查找函数标识符
    int findFunc(string ident) {
        for (int i = 0; i < _SymbL.size(); ++i) {
            if (_SymbL[i].ident == ident && _SymbL[i].identType == IdentType::FUNCTION) {
                return i;
            }
        }
        return -1;
    }

    // 查找数组标识符
    int findArray(string ident) {
        for (int i = 0; i < _SymbL.size(); ++i) {
            if (_SymbL[i].ident == ident && _SymbL[i].valType == ValType::ARR) {
                return i;
            }
        }
        return -1;
    }

    // 添加函数参数
    void addFuncPara(SymbLPointer fid, SymbLPointer vid) {
        funcL.add(_SymbL[fid].pointer, vid);
    }

    // 获取符号表项
    Item get(SymbLPointer id) {
        if (id < 0)
            return Item("none", IdentType::CONST, BlockType::TEMP, ValType::VOID, 0);
        return _SymbL[id];
    }

    // 获取符号表大小
    int size() {
        return _SymbL.size();
    }

    // 获取符号在活动记录偏移量，若为数组，还要传入数组第几个
    int getOffset(SymbLPointer id, int addr = 0) {
        if (_SymbL[id].valType == ValType::ARR) {
            return arrayL.getOneSize(getPointer(id)) * addr + arrayL.get(getPointer(id)).pointer;
        }
        return getPointer(id);
    }


    // 获取指定函数栈帧，同时设置符号表中偏移量的值
    int getDisplay(string funcName) {
        SymbLPointer fid = findFunc(funcName);
        int displayID = funcL.getPointer(getPointer(fid));
        // 遍历符号表，到下一个全局符号为止
        int pos = 0;
        for (int i = fid + 1; i < _SymbL.size(); ++i) {
            if (_SymbL[i].blockType == BlockType::GLOBAL) break;
            if (_SymbL[i].identType == IdentType::CONST) continue;// 常量不存在活动记录中
            // 根据不同类型分配空间，现默认都分配4字节
            int size = 128;
            if (_SymbL[i].valType == ValType::ARR) {
                size = arrayL.getDisplay(_SymbL[i].pointer);
                arrayL.get(_SymbL[i].pointer).pointer = pos;
            }
            else {
                _SymbL[i].pointer = pos;
            }
            pos += size;
        }
        // cout << funcName << " 的活动记录大小：" << pos << endl;
        return pos;
    }
};

SymbL symbL;

#endif