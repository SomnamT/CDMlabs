#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <cmath>
#include"z3++.h"
using std::string;
using std::vector;
using std::map;
using std::stack;
using namespace z3;

#include "multree.h"
#include "cfg.h"
#include "minisee.h"


//下面两个print函数或许能帮你debug
//输出程序流图root对应的代码
void print_cfg(cfg_node* root);
//输出表达式树root存储的表达式
void print_exp(exp_node* root);

extern stack<see_state> state_queue;


//把字符串表示的数字转成int类型带符号整数
//s可能是10进制无符号整数、10进制带符号整数或16进制整数
int mystoi(string s) {
    int int_value = 0;
    int length = s.length();
    string first_two = s.substr(0, 2);
    int index = 0;
    if(first_two.compare("0x") && first_two.compare("0X"))
    {
        bool is_minus = false;
        if(first_two[0] == '-')
        {
            is_minus = true;
            s.erase(0, 1);
            length = s.length();
        }
        for(int i = 0; i < length; i++)
        {
            index = length - i - 1;
            int_value += (s[index] - '0') * int(pow(10, i));
        }
        if(is_minus)    
            int_value = -int_value;    
    }
    else 
    {
        s.erase(0, 2);
        assert(s.length() <= 8);
        length = s.length();        
        for(int i = 0; i < length && i < 7; i++)
        {
            index = length - i - 1;
            if(s[index] >= '0' && s[index] <= '9')
                int_value += (s[index] - '0') * int(pow(16, i));
            else if(s[index] >= 'A' && s[index] <= 'F')
                int_value += ((s[index] - 'A') + 10) * int(pow(16, i));
            else 
                int_value += ((s[index] - 'a') + 10) * int(pow(16, i));                
        }
        if(length == 8)
        {
            if(s[0] < '8')
            {
                int_value += (s[0] - '0') * int(pow(16, 7));
            }
            else if(s[0] <= '9')
            {
                int_value += (s[0] - '8') * int(pow(16, 7));
                int_value -= int(pow(2, 31));
            }
            else if(s[0] >= 'A' && s[0] <= 'F')
            {
                int_value += (s[0] - 'A') * int(pow(16, 7));
                int_value -= int(pow(2, 31));
            }
            else 
            {
                int_value += (s[0] - 'a') * int(pow(16, 7));
                int_value -= int(pow(2, 31));
            }           
        }
    }
    return int_value;
}


//root是一棵表达式树
//如果root为null，就返回null
//否则，就生成一棵新的树，这棵树表示的表达式和root表示的完全一致
//但是这棵树的所有节点都是new出来的，不可以复用root中的任何节点
exp_node* copy_exp_tree(exp_node* root) {
    if(root == NULL)
        return NULL;    
    exp_node* new_root = new exp_node(root->type, root->val, NULL);
    new_root->child[0] = copy_exp_tree(root->child[0]);
    new_root->child[1] = copy_exp_tree(root->child[1]);    
    return new_root;
}


//这个函数用于处理if语句
//state是if语句对应的符号执行节点
//path_const是state里面的path constraint
//condition表示if条件表达式
//branch是true时返回if条件为真对应的新的path constraint
//branch是false时返回if条件为假对应的新的path constraint
exp_node* update_path_const(see_state* state, exp_node* path_const, bool branch, exp_node* condition) {
    
    //old_const是和state里面的path constraint一样的表达式
    exp_node* old_const = copy_exp_tree(path_const);
    //cur_const是当前if条件对应的表达式
    exp_node* cur_const = update_exp_tree(state, condition);

    //exp是新的path constraint
    exp_node* exp = NULL;
    if(old_const) {
        //如果当前state里面的path constraint不是true
        exp = new exp_node(exp_op, "&&", old_const);
        if(branch) {
            exp->child[1] = cur_const;
        }
        else {
            exp->child[1] = new exp_node(exp_op, "!", cur_const);
        }
    }
    else {
        if(branch) {
            exp = cur_const;
        }
        else {
            exp = new exp_node(exp_op, "!", cur_const);
        }
    }

    return exp;
}


//处理if语句
//state中的cfg第一条语句就是一个if语句
void analyze_if(see_state* state) {
    cfg_node* root = state->cfg;
    exp_node* cur_const = state->path_const;
    exp_node* condition = root->exp_tree;

    state->path_const = update_path_const(state, cur_const, false, condition);
    state->cfg = root->next[1];
    state_queue.push(*state);

    state->path_const = update_path_const(state, cur_const, true, condition);
    state->cfg = root->next[0];
    state_queue.push(*state);

    //your code here, you may need to use update_path_const
    
    return;
}



