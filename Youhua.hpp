#ifndef _YOUHUA
#define YOUHUA

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Siyuanshi.hpp"

using namespace std;
#define TEMPDAG 0 //临时变量 ok
#define CONSTDAG 1 //常量 ok
#define ADNET 2 //标识符 ok
#define BLANK 3 //空位 ok
// #define OTHERS 4//其他（包含跳转语句）的相关

int tttttt;

struct TokenDAG {
public:
	int type;
	string val;
    SymbLPointer sid;
	TokenDAG(){}
	TokenDAG(SymbLPointer sid){
		this->sid = sid;
		if(sid==-1){
			this->type = BLANK;
			this->val = "_";
		}
		else{
			if(symbL.get(sid).identType == IdentType::CONST){
				this->type = CONSTDAG;
				this->val = constL.get(symbL.get(sid).pointer).val;
			}
			else if(symbL.get(sid).blockType == BlockType::TEMP){
				this->type = TEMPDAG;
				this->val = symbL.get(sid).ident;
			}
			else{
				this->type = ADNET;
				this->val = symbL.get(sid).ident;
			}
		}
	}
	bool operator==(TokenDAG tt) {
		if (this->type == tt.type && this->val == tt.val)return true;
		else return false;
	}

	void init() {
		type = -1;
		val.clear();
	}
};

struct Item {
	string opt;
	TokenDAG val1;
	TokenDAG val2;
	TokenDAG result;

	void init() {
		opt.clear();
		val1.init();
		val2.init();
		result.init();
	}
};

class Circle {
public:
	int search_Ai(string ss) {
		for (int i = 0; i < Ai.size(); i++)
		{
			if (Ai[i].val == ss)return i+1;
		}
		return 0;
	}

	vector<Circle*> find_parent() {
		return parent;
	}

	vector<Circle*> find_left_child() {
		return left_child;
	}

	vector<Circle*> find_right_child() {
		return right_child;
	}

	TokenDAG find_M() {
		return M;
	}

	string find_w() {
		return w;
	}

	vector<TokenDAG>& find_Ai() {
		return Ai;
	}

	int find_ni() {
		return ni;
	}

	void set_left_child(vector<Circle*> c) {
		left_child = c;
	}

	void set_right_child(vector<Circle*> c) {
		right_child = c;
	}

	void set_parent(vector<Circle*> c) {
		parent = c;
	}

	void add_left_child(Circle* c) {
		left_child.push_back(c);
	}

	void add_right_child(Circle* c) {
		right_child.push_back(c);
	}

	void add_parent(Circle* c) {
		parent.push_back(c);
	}

	void set_M(TokenDAG m) {
		M = m;
	}

	void set_w(string m) {
		w = m;
	}

	void add_Ai(TokenDAG a) {
		Ai.push_back(a);
	}

	void set_ni(int n) {
		ni = n;
	}

	void init() {
		left_child.clear(); //左孩子
		right_child.clear();//右孩子
		parent.clear();     //父结点
		M.type = 0;           //主标记
		M.val.clear();
		Ai.clear();  //附加标记
		w.clear();           //运算符
		ni = -1;
	}

	void show_child_parent() {

		cout << "L:" << " ";
		if (left_child.size() == 0)cout<< -1 << " ";
		for (int i = 0; i < left_child.size(); i++) {
			if (find_left_child()[i] != NULL) cout << find_left_child()[i]->find_ni()+1;
			else cout << -1;
			cout << " ";
		}

		cout << "R:" << " ";
		if (right_child.size() == 0)cout << -1 << " ";
		for (int i = 0; i < right_child.size(); i++) {
			if (find_right_child()[i] != NULL) cout << find_right_child()[i]->find_ni()+1;
			else cout << -1;
			cout << " ";
		}

		cout << "P:" << " ";
		if (parent.size() == 0)cout<< -1 << " ";
		for (int i = 0; i < parent.size(); i++) {
			if (find_parent()[i] != NULL) cout << find_parent()[i]->find_ni()+1;
			else cout << -1;
			cout << " ";
		}
	}

	void show_Ai() {
		for (int i = 0; i < Ai.size(); i++)
		{
			cout << Ai[i].val << " ";
		}
		cout << endl;
	}


private:
	vector<Circle*> left_child; //左孩子
	vector<Circle*> right_child;//右孩子
	vector<Circle*> parent;     //父结点
	TokenDAG M;           //主标记
	vector<TokenDAG> Ai;  //附加标记
	string w;           //运算符
	int ni;             //结点编号
};

//Circle* newCircle = new Circle[50];
vector<Item> qualtersztr;
vector<vector<Item>> blockk;

void get_put() {
	TokenDAG cons[4];
	cout << "请输入四元式组" << endl;
	string putt;
	int k = 0;
	int t = 0;
	while (getline(cin, putt)) {
		for (int i = 1; putt[i + 1] != '|'; i++) {
			if (putt[i] == ' ')
				t++;
			else {
				cons[t].val.push_back(putt[i]);
			}
			k = i + 1;
		}
		cons[1].type = (int)putt[k + 2] - (int)'0';
		cons[2].type = (int)putt[k + 4] - (int)'0';
		cons[3].type = (int)putt[k + 6] - (int)'0';
		qualtersztr.push_back({ cons[0].val, cons[1], cons[2], cons[3] });
		cons[0].val = "";
		cons[1].val = "";
		cons[2].val = "";
		cons[3].val = "";
		t = 0;
	}

	//输出读入的原四元式
	/*for (int i = 0; i < record.size(); i++) {
		cout << "(" << record[i].opt << " " << record[i].val1 << " " << record[i].val2 << " " << record[i].result << ")" << endl;
	}*/
}

bool is_out(const string& op) {
	return op == "gt" || op == "if" || op == "el" || op == "if" || op == "do";
}

bool is_in(const string& op) {
	return op == "lb" || op == "ie";
	//|| op == "el" || op == "if" || op == "do";
}

vector<vector<Item>> blockQuadruples(const vector<Item>& quadruples)//分块
{
	vector<vector<Item>> blocks;
	vector<Item> currentBlock;
	bool isEntryStatement = true;

	for (size_t i = 0; i < quadruples.size(); ++i) {

		// 检查是否是分块的起始位置
		if (isEntryStatement || is_out(quadruples[i - 1].opt) || is_in(quadruples[i].opt)) {
			if (!currentBlock.empty()) {
				blocks.push_back(currentBlock);
				currentBlock.clear();
			}
			isEntryStatement = false;
		}

		currentBlock.push_back(quadruples[i]);
	}

	// 处理最后一个基本块
	if (!currentBlock.empty()) {
		blocks.push_back(currentBlock);
	}

	return blocks;
}

void output_block(vector<vector<Item>>& blocks)// 输出分块结果
{
	for (int i = 0; i < blocks.size(); i++) {
		cout << "Block " << i + 1 << ":" << endl;
		for (const auto& quadruple : blocks[i]) {
			cout << "(" << quadruple.opt << " " << quadruple.val1.val << " " << quadruple.val2.val << " " << quadruple.result.val << ")";
			cout << "|" << quadruple.val1.type << " " << quadruple.val2.type << " " << quadruple.result.type <<  endl;
		}
		cout << endl;
	}

}

int find_same_num(int num, string Goal, Circle* cc) {
	for (int i = 0; i < num; i++) {
		if (cc[i].find_M().val == Goal)return i;
		if (cc[i].search_Ai(Goal))return i;
	}
	return -1;
}

bool is_TEMP(int num, Circle* cc) {
	if (cc[num].find_M().type == TEMPDAG)return true;
	else return false;
}

int find_same_parent(int num, Item ff, Circle* cc) {
	for (int i = 0; i < cc[num].find_parent().size(); i++) {
		if (cc[num].find_parent()[i]->find_w()==ff.opt)return cc[num].find_parent()[i]->find_ni();
	}
	return -1;
}

void delete_Ai(string ss,Circle* cc) {
	for (int i = 0; cc[i].find_ni() >= 0; i++) {
		for (int j = 0; j < cc[i].find_Ai().size(); j++) {
			if (cc[i].find_Ai()[j].val == ss) {
				cc[i].find_Ai().erase(cc[i].find_Ai().begin() + j);
				j--;
			}
		}
	}
}

bool is_OK(vector<Item> four_sent) {
	for (int i = 0; i < four_sent.size(); i++) {
		if (four_sent[i].opt == "+" && four_sent[i].val1.type == CONSTDAG && four_sent[i].val2.type == CONSTDAG)return false;
		
	}
	 return true;
}

Circle* build_DAG(vector<Item> four_sent) {
	int num = 0;
	int ttt = 0;
	Circle* root = NULL;
	int same = -1;
	int same1 = -1;
	int same2 = -1;
	int same3 = -1;
	int same4 = -1;
	int ss = -1;
	// Circle cs[50];
	Circle* newCircle = new Circle[500];
	for (int i = 0; i < 500; i++)
	{
		newCircle[i].init();
	}
	for (int i = 0; i < four_sent.size(); i++) {
		delete_Ai(four_sent[i].result.val, newCircle);
		if (four_sent[i].opt == "=") {
			same = find_same_num(num, four_sent[i].val1.val, newCircle);
			if (same != -1) {
				if (is_TEMP(same, newCircle) && four_sent[i].result.type == ADNET) {
					newCircle[same].add_Ai(newCircle[same].find_M());
					newCircle[same].set_M(four_sent[i].result);
					//delete_Ai(four_sent[i].result.val,newCircle);
				}
				else {
					newCircle[same].add_Ai(four_sent[i].result);
				}
			}
			else {
				newCircle[num].set_ni(num);
				newCircle[num].set_M(four_sent[i].val1);
				newCircle[num].add_Ai(four_sent[i].result);
				num++;
			}
		}
		
		else if ((four_sent[i].val1.type == CONSTDAG && four_sent[i].val2.type == CONSTDAG) && (four_sent[i].opt == "+" || four_sent[i].opt == "-" || four_sent[i].opt == "*" || four_sent[i].opt == "/")) {
			stringstream ss1,ss2;
			ss1 << four_sent[i].val1.val;
			ss2 << four_sent[i].val2.val;
			float ans1,ans2;
			ss1 >> ans1;
			ss2 >> ans2;
			ttt++;

				if (four_sent[i].opt == "+") {
				four_sent[i].opt = "=";
				four_sent[i].val1.type = CONSTDAG;
				four_sent[i].val1.val=to_string(stoi(four_sent[i].val1.val)+ stoi(four_sent[i].val2.val));
				four_sent[i].val2.type = BLANK;
				four_sent[i].val2.val = "_";
			}
			else if (four_sent[i].opt == "-") {
				four_sent[i].opt = "=";
				four_sent[i].val1.type = CONSTDAG;
				four_sent[i].val1.val = to_string(stoi(four_sent[i].val1.val) - stoi(four_sent[i].val2.val));
				four_sent[i].val2.type = BLANK;
				four_sent[i].val2.val = "_";
			}
			else if (four_sent[i].opt == "*") {
				four_sent[i].opt = "=";
				four_sent[i].val1.type = CONSTDAG;
				four_sent[i].val1.val = to_string(stoi(four_sent[i].val1.val) * stoi(four_sent[i].val2.val));
				four_sent[i].val2.type = BLANK;
				four_sent[i].val2.val = "_";
			}
			else if (four_sent[i].opt == "/") {
				four_sent[i].opt = "=";
				four_sent[i].val1.type = CONSTDAG;
				four_sent[i].val1.val = to_string(stoi(four_sent[i].val1.val) / stoi(four_sent[i].val2.val));
				four_sent[i].val2.type = BLANK;
				four_sent[i].val2.val = "_";
			}

			// if (four_sent[i].opt == "+") {
			// 	four_sent[i].opt = "=";
			// 	four_sent[i].val1.type = CONSTDAG;
			// 	four_sent[i].val1.val = to_string(ans1 + ans2);
			// 	four_sent[i].val2.type = BLANK;
			// 	four_sent[i].val2.val = "_";
			// }
			// else if (four_sent[i].opt == "-") {
			// 	four_sent[i].opt = "=";
			// 	four_sent[i].val1.type = CONSTDAG;
			// 	four_sent[i].val1.val = to_string(ans1 - ans2);
			// 	four_sent[i].val2.type = BLANK;
			// 	four_sent[i].val2.val = "_";
			// }
			// else if (four_sent[i].opt == "*") {
			// 	four_sent[i].opt = "=";
			// 	four_sent[i].val1.type = CONSTDAG;
			// 	four_sent[i].val1.val = to_string(ans1 * ans2);
			// 	four_sent[i].val2.type = BLANK;
			// 	four_sent[i].val2.val = "_";
			// }
			// else if (four_sent[i].opt == "/") {
			// 	four_sent[i].opt = "=";
			// 	four_sent[i].val1.type = CONSTDAG;
			// 	four_sent[i].val1.val = to_string(ans1 / ans2);
			// 	four_sent[i].val2.type = BLANK;
			// 	four_sent[i].val2.val = "_";
			// }
			string name;
			name="DAG"+to_string(tttttt);
			tttttt++;
			SymbLPointer ss=symbL.addConst(name, ValType::INT, four_sent[i].val1.val, 0);
			// throw string(four_sent[i].val1.val);
			four_sent[i].val1.sid=ss;
			i--;
		}
		
		else if ((four_sent[i].val1.type!=CONSTDAG||four_sent[i].val2.type!=CONSTDAG) && (four_sent[i].opt == "+" || four_sent[i].opt == "-" || four_sent[i].opt == "*" || four_sent[i].opt == "/")) {
		//else if (four_sent[i].opt == "+" || four_sent[i].opt == "-" || four_sent[i].opt == "*" || four_sent[i].opt == "/"){	
			same1 = find_same_num(num, four_sent[i].val1.val, newCircle);
			if (same1 != -1) {
				if (is_TEMP(same1, newCircle) && four_sent[i].val1.type == ADNET) {
					newCircle[same1].add_Ai(newCircle[same1].find_M());
					newCircle[same1].set_M(four_sent[i].val1);
				}
			}
			else {
				newCircle[num].set_M(four_sent[i].val1);
				//cout << newCircle[num].find_M().val << endl;
				//newCircle[num].show_Ai();
				same1 = num;
				newCircle[num].set_ni(num);
				num++;
			}

			same2 = find_same_num(num, four_sent[i].val2.val, newCircle);
			if (same2 != -1) {
				if (is_TEMP(same2, newCircle) && four_sent[i].val2.type == ADNET) {
					newCircle[same2].add_Ai(newCircle[same2].find_M());
					newCircle[same2].set_M(four_sent[i].val2);
				}
			}
			else {
				newCircle[num].set_M(four_sent[i].val2);
				//cout << newCircle[num].find_M().val << endl;
				//newCircle[num].show_Ai();
				same2 = num;
				newCircle[num].set_ni(num);
				num++;
			}

			same3 = find_same_parent(same1, four_sent[i], newCircle);
			same4 = find_same_parent(same2, four_sent[i], newCircle);

			if ((same3 == same4)&&same3!=-1 ){
				ss = same3;
				if (is_TEMP(ss, newCircle) && four_sent[i].result.type == ADNET) {
					newCircle[ss].add_Ai(newCircle[ss].find_M());
					newCircle[ss].set_M(four_sent[i].result);
					goto tip1;
				}
				else {
					newCircle[ss].add_Ai(four_sent[i].result);
					goto tip1;
				}
				newCircle[ss].add_left_child(&newCircle[same1]);
				newCircle[ss].add_right_child(&newCircle[same2]);
				newCircle[same1].add_parent(&newCircle[ss]);
				newCircle[same2].add_parent(&newCircle[ss]);
			}
			else {
				ss = num;
				newCircle[ss].set_M(four_sent[i].result);
				newCircle[ss].set_w(four_sent[i].opt);
				//cout << newCircle[ss].find_M().val << endl;
				//newCircle[ss].show_Ai();
				newCircle[ss].add_left_child(&newCircle[same1]);
				newCircle[ss].add_right_child(&newCircle[same2]);
				newCircle[same1].add_parent(&newCircle[ss]);
				newCircle[same2].add_parent(&newCircle[ss]);
				newCircle[num].set_ni(num);
				num++;
			}
		tip1:;
		}
		// else {
		// 	cout << "(" << four_sent[i].opt << " " << four_sent[i].val1.val << " " << four_sent[i].val2.val << " " << four_sent[i].result.val << ")\n";
		// }
	}

	vector<Item>* to_four_sent1(Circle* cc);

	root = newCircle;
	//show_DAG(newCircle);
	if(!is_OK(*to_four_sent1(newCircle)))root = build_DAG(*to_four_sent1(newCircle));
	
	return root;
}

vector<Item>* to_four_sent(Circle* cc) {
	vector<Item>* root = NULL;
	int num = 0;
	int tt = 0;
	vector<Item>* four_sent = new vector<Item>;

	while (cc[num].find_ni() >= 0) {
		if (cc[num].find_left_child().size() != 0 && cc[num].find_right_child().size() != 0) {
			(*four_sent).push_back({ cc[num].find_w(),cc[num].find_left_child()[0]->find_M(),cc[num].find_right_child()[0]->find_M(), cc[num].find_M() });
			for (int i = 0; i < cc[num].find_Ai().size(); i++) {
				if (cc[num].find_Ai()[i].type != TEMPDAG) {
					(*four_sent).push_back({"=",cc[num].find_M(),TokenDAG(-1),cc[num].find_Ai()[i] });
				}
			}
		}
		else if (cc[num].find_left_child().size() == 0 && cc[num].find_right_child().size() == 0 && cc[num].find_Ai().size() != 0) {
			for (int i = 0; i < cc[num].find_Ai().size(); i++) {
				if (cc[num].find_Ai()[i].type != TEMPDAG) {
					(*four_sent).push_back({ "=",cc[num].find_M(),TokenDAG(-1),cc[num].find_Ai()[i] });
				}
			}
		}

		num++;
	}

	/*for (int i = 0; i < four_sent->size(); i++) {
		cout << "(" << (*four_sent)[i].opt << " " << (*four_sent)[i].val1.val << " " << (*four_sent)[i].val2.val << " " << (*four_sent)[i].result.val << ")";
		cout << "|" << (*four_sent)[i].val1.type << " " << (*four_sent)[i].val2.type << " " << (*four_sent)[i].result.type << endl;
	}*/

	root = four_sent;

	return root;
}


vector<Item>* to_four_sent1(Circle* cc) {
	vector<Item>* root = NULL;
	int num = 0;
	int tt = 0;
	int k=0;
	vector<Item>* four_sent = new vector<Item>;

	while (cc[num].find_ni() >= 0) {
		if (cc[num].find_left_child().size() != 0 && cc[num].find_right_child().size() != 0) {
			(*four_sent).push_back({ cc[num].find_w(),cc[num].find_left_child()[0]->find_M(),cc[num].find_right_child()[0]->find_M(), cc[num].find_M() });
			for (int i = 0; i < cc[num].find_Ai().size(); i++) {
				if (cc[num].find_Ai()[i].type != TEMPDAG) {
					(*four_sent).push_back({"=",cc[num].find_M(),TokenDAG(-1),cc[num].find_Ai()[i]});
				}
			}
		}
		else if (cc[num].find_left_child().size() == 0 && cc[num].find_right_child().size() == 0 && cc[num].find_Ai().size() != 0) {
			for (int i = 0; i < cc[num].find_Ai().size(); i++) {
				if (cc[num].find_Ai()[i].type != TEMPDAG) {
					(*four_sent).push_back({ "=",cc[num].find_M(),TokenDAG(-1),cc[num].find_Ai()[i] });
				}
			}
		}

		num++;
	}

	root = four_sent;

	return root;
}


void show_DAG(Circle* cc) {
	int num = 0;
	while (cc[num].find_ni() >= 0) {
		cout << cc[num].find_ni()+1 << " " <<cc[num].find_w()<<" " << cc[num].find_M().val << " ";
		cc[num].show_Ai();
		cout << " ";
		cc[num].show_child_parent();
		cout << endl<<endl;
		num++;
	}
}

bool is_opt(string oo) {
	if (oo == "=" || oo == "+" || oo == "-" || oo == "*" || oo == "/")return true;
	else return false;
}

void show_four_sent(vector<Item> bb) {
	for (int i = 0; i < bb.size(); i++) {
		cout << "(" << bb[i].opt << " " << bb[i].val1.val << " " << bb[i].val2.val << " " << bb[i].result.val << ")" << endl;
	}
}
void show_four_sent1(vector<vector<Item>> bb) {
	for (int i = 0; i < bb.size(); i++) {
		for (int j = 0; j < bb[i].size(); j++) {
			cout << "(" << bb[i][j].opt << " " << bb[i][j].val1.val << " " << bb[i][j].val2.val << " " << bb[i][j].result.val << ")" << endl;
		}
	}
}

//vector<vector<Item>> blockk;

vector<vector<Item>>* promote(vector<vector<Item>>& blocks) {
	// cout << "block size " << blocks.size() << endl;
    for (int i = 0; i < blocks.size(); i++) {
        // cout << "当前块" << i << endl;
        //cout << "block" << i + 1 << ":" << endl;
		vector<Item> bb;//所有四元式
		bb.clear();
		vector<Item> gg;//只含有运算符的向量
		gg.clear();
		// cout << "j size " << blocks[i].size() << endl;
		for (int j = 0; j < blocks[i].size() ; j++)
        {
            // cout << "j = " << j << endl;
			if (!is_opt(blocks[i][j].opt)) {
				// cout << "00" << endl;
				if (gg.size() != 0) {
					Circle* cc1 = build_DAG(gg);
					vector<Item>* II2 = to_four_sent(cc1);
					for (int k = 0; k < II2->size(); k++)
					{
						bb.push_back((*II2)[k]);
					}
				}
				gg.clear();
				bb.push_back(blocks[i][j]); 
			}
			else {
				// cout << "11" << endl;
				gg.push_back(blocks[i][j]);
			}
			// cout << "out" << endl;
		}
		if (gg.size() != 0) {
			Circle* cc1 = build_DAG(gg);
	
			vector<Item>* II2 = to_four_sent(cc1);
			for (int k = 0; k < II2->size(); k++)
			{
				bb.push_back((*II2)[k]);
			}
		}
		gg.clear();

		//show_four_sent(bb);

		//Circle* root1 = build_DAG(blocks[i]);
		////show_DAG(root1);
		//Item* root2=to_four_sent(root1);
		////show_four_sent(root2);
		blockk.push_back(bb);
	}
	
	//blocks = blockk;
	// cout<<"---------------"<<endl;
	// show_four_sent1(blockk);
	// cout<<"---------------"<<endl;
	return &blockk;
}

void main0() {
	get_put();
	vector<vector<Item>> blocks = blockQuadruples(qualtersztr);
	//output_block(blocks);

	promote(blocks);
}

// Siyuanshi youhua(Siyuanshi origin){
// 	vector<Item> tiantian;
// 	for(int i= 0;i < origin.size(); ++i){
// 		Siyuanshi::STRUCT zzitem = origin.get(i);
// 		TokenDAG val1(zzitem.val1);
// 		TokenDAG val2(zzitem.val2);
// 		TokenDAG result(zzitem.result);
// 		tiantian.push_back(Item{zzitem.opt, val1, val2, result});
// 		cout << "转义后四元式：" << zzitem.opt << " " << val1.val << "(" << val1.type << ")" << " " << val2.val << "(" << val2.type << ")" << result.val << "(" << result.type << ")" << endl;
// 	}
// 	cout << "转义完毕" << endl;
// 	vector<vector<Item>> blocks = blockQuadruples(tiantian);
// 	// output_block(blocks);
//     promote(blocks);

//     Siyuanshi ans;
// 	for(int i= 0;i < blockk.size(); ++i){
// 		for(int j = 0; j < blockk[i].size(); ++j){
// 			ans.add(blockk[i][j].opt, blockk[i][j].val1.sid, blockk[i][j].val2.sid, blockk[i][j].result.sid);
// 		}
// 	}

// 	// for(int i= 0;i < blockk.size(); ++i){
// 	// 	for(int j = 0; j < blockk[i].size(); ++j){
// 	// 		ans.add(blockk[i][j].opt, blockk[i][j].val1.sid, blockk[i][j].val2.sid, blockk[i][j].result.sid);
// 	// 	}
// 	// }
  
//     return ans;
// }

Siyuanshi youhua(Siyuanshi origin){
	vector<Item> tiantian;
	for(int i= 0;i < origin.size(); ++i){
		Siyuanshi::STRUCT zzitem = origin.get(i);
		TokenDAG val1(zzitem.val1);
		TokenDAG val2(zzitem.val2);
		TokenDAG result(zzitem.result);
		tiantian.push_back(Item{zzitem.opt, val1, val2, result});
		// cout << "转义后四元式：" << zzitem.opt << " " << val1.val << "(" << val1.type << ")" << " " << val2.val << "(" << val2.type << ")" << result.val << "(" << result.type << ")" << endl;
	}
	// cout << "转义完毕" << endl;
	vector<vector<Item>> blocks = blockQuadruples(tiantian);
    // cout<<"-----------"<<endl;
	// output_block(blocks);
    // cout << "-----------" << endl;
    // cout << "进入promote" << endl;
    promote(blocks);
    // cout << "出promote" << endl;
    Siyuanshi ans;
	// for(int i= 0;i < blocks.size(); ++i){
	// 	for(int j = 0; j < blocks[i].size(); ++j){
	// 		ans.add(blocks[i][j].opt, blocks[i][j].val1.sid, blocks[i][j].val2.sid, blocks[i][j].result.sid);
	// 	}
	// }

	for(int i= 0;i < blockk.size(); ++i){
		for(int j = 0; j < blockk[i].size(); ++j){
			ans.add(blockk[i][j].opt, blockk[i][j].val1.sid, blockk[i][j].val2.sid, blockk[i][j].result.sid);
		}
	}
  
    return ans;
}




#endif