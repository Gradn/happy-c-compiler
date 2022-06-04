string macro(string code) {
    stringstream ss(code);
    vector<string> content; // 存储修改后的文件内容
    bool in_comment = false; // 标记是否在注释中
    string define_name; // 存储当前处理的宏定义名称（如果有的话）
    string define_value; // 存储当前处理的宏定义取值（如果有的话）

    string line;
    while(getline(ss, line)) {
        if(in_comment) {
            // 如果在注释中，查找注释结束符号
            size_t end_pos = line.find("*/");
            if(end_pos != string::npos) {
                // 如果找到了注释结束符号，更新行内容和标记变量
                line = line.substr(end_pos + 2);
                in_comment = false;
            } else {
                // 如果没有找到注释结束符号，跳过此行
                continue;
            }
        }

        // 查找注释起始符号
        size_t comment_pos = line.find("//");
        size_t comment_start_pos = line.find("/*");
        if(comment_pos != string::npos && (comment_start_pos == string::npos || comment_pos < comment_start_pos)) {
            // 如果找到了单行注释，截取注释前的部分
            line = line.substr(0, comment_pos);
        } else if(comment_start_pos != string::npos) {
            // 如果找到了多行注释，标记变量，并截取注释前的部分
            in_comment = true;
            line = line.substr(0, comment_start_pos);
        }

        size_t pos = line.find("#include"); // 查找#include语句
        if(pos != string::npos) {
            // 如果找到了#include语句，处理文件名
            size_t start_pos = line.find_first_of("<\"") + 1; // 获取包含文件名的起始位置
            size_t end_pos = line.find_last_of(">\"") - 1; // 获取包含文件名的结束位置
            string include_filename = line.substr(start_pos, end_pos - start_pos + 1); // 提取包含文件名
            // cout<<"包含文件名"<<include_filename<<endl;

            // 打开包含文件
            ifstream include_file(include_filename);
            if(include_file.is_open()) {
                // 读取包含文件的内容，并处理宏定义
                string include_content((istreambuf_iterator<char>(include_file)), istreambuf_iterator<char>());
                include_content = macro(include_content); // 递归处理包含文件的内容
                if(!define_name.empty()) {
                    // 如果有待替换的宏定义名称，执行替换操作
                    size_t define_pos = 0;
                    while((define_pos = include_content.find(define_name, define_pos)) != string::npos) {
                        include_content.replace(define_pos, define_name.length(), define_value);
                    }
                }
                content.push_back(include_content); // 将处理后的包含文件内容添加到修改后的文件内容中
                include_file.close(); // 关闭包含文件
            } else {
                throw string("宏-找不到包含文件 " + include_filename);
            }
        }
         else if(line.find("#define") != string::npos) {
            // 如果找到了#define语句，提取宏定义名称和取值，并更新标记变量
            size_t start_pos = line.find("#define") + 7;
            size_t end_pos = line.find_last_not_of(" \t\r\n");
            string define_statement = line.substr(start_pos, end_pos - start_pos + 1);
            stringstream define_ss(define_statement);
            define_ss >> define_name;
            define_value.clear();
            while(define_ss >> define_value) {
                }
            }
        else {
            if(!define_name.empty()) {
                // 如果有待替换的宏定义名称，执行替换操作
                size_t define_pos = 0;
                while((define_pos = line.find(define_name, define_pos)) != string::npos) {
                    line.replace(define_pos, define_name.length(), define_value);
                }
            }
            content.push_back(line); // 将非#include语句添加到修改后的文件内容中
        }
    }

    // // 将修改后的文件内容写入原文件中
    // ofstream(outfile(filename);
    // if(outfile.is_open()) {
    //     for(size_t i = 0; i < content.size(); i++) {
    //         outfile << content[i] << endl;
    //     }
    //     outfile.close();
    // } else {
    //     cout << "Error: failed to write to file " << filename << endl;
    //     return 1;
    // }
    string ans = "";
    for (int i = 0; i < content.size(); ++i) {
        ans += content[i] + "\n";
    }
    return ans;
}
