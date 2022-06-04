#include "Fuhaobiao.hpp"
#include "Siyuanshi.hpp"


ValType getValType(string val) {
    if (val == "int") return ValType::INT;
    else if (val == "char") return ValType::CHAR;
    else if (val == "float") return ValType::FLOAT;
    return ValType::VOID;
}

stack<Token> tokens;
stack<SymbLPointer> sids;
stack<Token> opnd;
stack<int> xunhuanLayer; // 循环体层次，0位for，1位while
int layer = 0; // 记录层级
void error_analyse(string tip) {
    throw string("语义分析-" + tip);
}

void action1(Token* nowToken) {
    tokens.push(*nowToken); // 参数入栈
}

void action2(Token* nowToken) {
    // cout << 111 << endl;
    Token ident = tokens.top(); tokens.pop();
    // cout << "val " << ident.val << endl;
    SymbLPointer fid = symbL.findFunc(ident.val);
    // cout << "fid = " << fid << endl;
    // cout << "ppp =  " << symbL.getPointer(fid) << endl;
    if (fid != -1 && funcL.get(symbL.getPointer(fid)).defined == 1) {
        // cout << "000" << endl;
        error_analyse("函数重定义：" + ident.val);
    }
    // cout << 222 << endl;
    Token t = tokens.top(); tokens.pop();
    string leixing = t.val;
    string biaozhifu = ident.val;
    // cout << 333 << endl;
    // 添加函数符号到符号表
    SymbLPointer sid = symbL.addFunction(biaozhifu, getValType(leixing), layer);
    sids.push(sid);
    // cout << "push sidd = " << sid << endl;
}

void action3(Token* nowToken) {
    string ident = tokens.top().val; tokens.pop();
    string type = tokens.top().val; tokens.pop();
    SymbLPointer fsid = sids.top(); //sids.pop();
    // 检查符号是否重定义
    if (symbL.redefine(ident))
        error_analyse("符号重定义：" + ident);
    // 将变量符号加入符号表
    SymbLPointer vsid = symbL.addVariable(ident, getValType(type), layer);
    // 将变量符号加入函数参数
    symbL.addFuncPara(fsid, vsid);
}

void action4(Token* nowToken) {
    string ident = tokens.top().val; tokens.pop();
    string type = tokens.top().val; tokens.pop();
    // 检查符号是否重定义
    if (symbL.redefine(ident))
        error_analyse("符号重定义：" + ident);
    // 将变量符号加入符号表
    SymbLPointer vsid = symbL.addVariable(ident, getValType(type), layer);
}

void action5(Token* nowToken) {
    // 检查标识符未定义问题
    Token ident = tokens.top(); tokens.pop();
    SymbLPointer biaozhifu = symbL.findSymb(ident.val);
    if (biaozhifu == -1)
        error_analyse("未定义标识符：" + ident.val);
    Token result = opnd.top(); opnd.pop();
    SymbLPointer jieguo = symbL.findSymb(result.val);
    if (symbL.get(jieguo).valType == ValType::ARR) {
        qualters.add("=[]", jieguo, result.id, biaozhifu);
    }
    else {
        qualters.add("=", jieguo, biaozhifu);
    }
}

void action6(Token* nowToken) {
    Token result = opnd.top(); opnd.pop();
    SymbLPointer jieguo = symbL.findSymb(result.val);
    qualters.add("if", jieguo);
}

void action7(Token* nowToken) {
    qualters.add("edif");
}

void action8(Token* nowToken) {
    Token tk = tokens.top(); tokens.pop();
    SymbLPointer sid = symbL.findFunc(tk.val);
    if (sid == -1)
        error_analyse("函数未定义：" + tk.val);
    qualters.add("call", sid);
}

void action9(Token* nowToken) {
    SymbLPointer sid = symbL.findToken(&opnd.top(), layer); opnd.pop();
    qualters.add("para", sid);
}

void action10(Token* nowToken) {
    qualters.add("wl");
    xunhuanLayer.push(1);
}

void action11(Token* nowToken) {
    Token result = opnd.top(); opnd.pop();
    SymbLPointer jieguo = symbL.findSymb(result.val);
    qualters.add("do", jieguo);
}

void action12(Token* nowToken) {
    qualters.add("edwl");
    xunhuanLayer.pop();
}

void action13(Token* nowToken) {
    Token result = opnd.top(); opnd.pop();
    SymbLPointer jieguo = symbL.findSymb(result.val);
    // 生成返回四元式
    qualters.add("ret", jieguo);
}

int tempidstr = 0;
void action14(Token* nowToken) {
    SymbLPointer sid = symbL.addConst(string("constrtmp") + to_string(tempidstr++), ValType::STRING, nowToken->val, layer);
    qualters.add("para", sid);
}

void action15(Token* nowToken) {
    qualters.add("else");
}

void action16(Token* nowToken) {
    qualters.add("forinit");
}

void action17(Token* nowToken) {
    qualters.add("for");
    xunhuanLayer.push(0);
}


stack<vector<Siyuanshi::STRUCT>::iterator> iters;
void action18(Token* nowToken) {
    Token result = opnd.top(); opnd.pop();
    SymbLPointer jieguo = symbL.findSymb(result.val);
    qualters.add("fordo", jieguo);
    iters.push(qualters.getIterator());
}

void action19(Token* nowToken) {
    qualters.add("foriter");
}

void action20(Token* nowToken) {
    qualters.add("edfor");
    xunhuanLayer.pop();
    // qualters.cancelInterupt();
}

void action21(Token* nowToken) {
    if (xunhuanLayer.empty())
        error_analyse("break只能在循环中使用");
    qualters.add("break");
}

void action22(Token* nowToken) {
    if (xunhuanLayer.empty())
        error_analyse("continue只能在循环中使用");
    qualters.add("continue");
}

void action23(Token* nowToken) {

    string ident = tokens.top().val; tokens.pop();
    string type = tokens.top().val; tokens.pop();
    // 检查符号是否重定义
    if (symbL.redefine(ident))
        error_analyse("符号重定义：" + ident);
    // 将变量符号加入符号表
    SymbLPointer vsid = symbL.addVariable(ident, getValType(type), layer);
    Token result = opnd.top(); opnd.pop();
    SymbLPointer jieguo = symbL.findSymb(result.val);
    qualters.add("=", jieguo, vsid);

}

void action24(Token* nowToken) {
    string num = tokens.top().val; tokens.pop();
    string ident = tokens.top().val; tokens.pop();
    string type = tokens.top().val; tokens.pop();
    symbL.addArray(ident, ValType::INT, atoi(num.c_str()), layer);
}

void action25(Token* nowToken) {
    // 检查标识符未定义问题
    Token iter = tokens.top(); tokens.pop();
    SymbLPointer sid = symbL.findSymb(iter.val);
    // cout << "iter is " << iter.val << endl;
    Token ident = tokens.top(); tokens.pop();
    SymbLPointer biaozhifu = symbL.findSymb(ident.val);
    if (biaozhifu == -1)
        error_analyse("未定义标识符：" + ident.val);
    Token result = opnd.top(); opnd.pop();
    // cout << "result is " << result.val << endl;
    SymbLPointer jieguo = symbL.findSymb(result.val);
    qualters.add("[]=", jieguo, sid, biaozhifu);
}

void action26(Token* nowToken) {
    layer++;
}

void action27(Token* nowToken) {
    layer--;
}

void action28(Token* nowToken) {
    // 添加函数定义四元式
    // cout << "a28" << endl;
    SymbLPointer fid = sids.top(); sids.pop();
    // cout << "fid " << fid << endl;
    qualters.add("func", fid);
    // cout << 2 << endl;
    funcL.setDefined(symbL.getPointer(fid));
    // cout << 2 << endl;
}

void action29(Token* nowToken) {
    // cout << "a29" << endl;
    sids.top(); sids.pop();
}

void action30(Token* nowToken) {
    // cout << "栈顶值" << opnd.top().val << endl;
    tokens.push(opnd.top()); opnd.pop();
}
