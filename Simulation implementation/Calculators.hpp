#include <map>
#include <string>
#include <assert.h>
#include <vector>
#include <stack>

using namespace std;
class solution
{
public:
    //map<char, int> _operatorPrecedence = { { '+', 1 }, { '-', 1 }, { '*', 2 }, { '/', 2 } };
    int operatorPrecedence(char op){
        struct oppd
        {
            char _op;
            int _pd;
        };
        struct oppd oppds[] = { { '+', 1 }, { '-', 1 }, { '*', 2 }, { '/', 2 } };
        for(auto p : oppds){
            if (op == p._op)
            {
                return p._pd;
            }            
        }
        assert(false); // operator not found in map
        return -1; // to avoid warning
    }
    void toRPN(const string& s, size_t& i, vector<string>& v)
    {
        stack<char> st;
        int n = s.size();;
        while (i < n)
        {
            if(isdigit(s[i]))//操作数输出
            {
                string num = "";
                while (i < n && isdigit(s[i])){
                    num += s[i];
                    ++i;
                }
                v.push_back(num);
            }
            else
            {
                if(s[i] == '(')//递归处理括号中的子表达式
                {
                    ++i;
                    toRPN(s, i, v);
                }
                else if(s[i] == ')')//遇到右括号，弹出栈中所有操作符
                {
                    ++i;;
                    while(!st.empty()){
                        v.push_back(string(1, st.top()));
                        st.pop();
                    }
                    //结束递归
                    return;
                }
                else
                {
                    //操作符
                    // 1如果栈为空或者栈不为空且当前运算符⽐栈顶运算符优先级⾼，则当前运算符⼊栈
                    // 2、如果栈不为为空且⽐栈顶运算符优先级低或相等，说明栈顶的运算符可以运算了
                    // 输出栈顶运算符，当前运算符继续⾛前⾯遇到运算符的逻辑
                    if(st.empty()||operatorPrecedence(s[i])>operatorPrecedence(st.top())){
                        st.push(s[i]);
                        ++i;
                    }
                    else{
                        while(!st.empty()&&operatorPrecedence(s[i])<=operatorPrecedence(st.top())){
                            v.push_back(string(1, st.top()));
                            st.pop();
                        }
                    }
                }
            }
        }
        //栈中剩余的运算符全部输出
        while(!st.empty()){
            v.push_back(string(1, st.top()));
            st.pop();
        }   
    } 
    int evalRPN(vector<string>& tokens) {
        stack<int> s;
        for(auto str:tokens){
            if(str!="+"&&str!="-"&&str!="*"&&str!="/"){
                s.push(stoi(str));
            }
            else{
                int right=s.top();
                s.pop();
                int left=s.top();
                s.pop();
                switch (str[0]){
                    case '+':
                        s.push(left + right);
                        break;
                    case '-':
                        s.push(left - right);
                        break;
                    case '*':
                        s.push(left * right);
                        break;
                    case '/':
                        s.push(left / right);
                        break;
                    default:
                }
            }
        }
        return s.top();
    } 
    int calculate(string s) 
    {
        //去除空格
        string news;
        news.reserve(s.size());
        for(auto c:s){
            if(c!=' '){
                news+=c;
            }
        }
        s.swap(news);
        news.clear();
        //所有负数处理为0-x
        for(int i=0;i<s.size();++i){
            if(s[i]=='-'&&(i==0||(!isdigit(s[i-1])&&s[i-1]!=')')))
            {
                news+="0-";
            }
            else{
                news+=s[i];
            }
        }
        //中缀表达式转后缀表达式
        vector<string> v;
        size_t i=0;
        toRPN(news, i, v);

        //计算后缀表达式
        return evalRPN(v); 
    }        
};

