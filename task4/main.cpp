#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <cctype>
#include <cmath>
using namespace std;

// Function to return precedence of operators
int precedence(char op) {
    if (op == '^') return 3;
    else if (op == '*' || op == '/') return 2;
    else if (op == '+' || op == '-') return 1;
    return -1;
}

// Check if operator is right-associative
bool isRightAssociative(char op) {
    return op == '^';
}

// Convert infix to postfix (space separated for multi-digit/float parsing)
string infixToPostfix(const string &expr) {
    stack<char> s;
    string output;
    for (size_t i = 0; i < expr.length();) {
        char c = expr[i];

        if (isspace(c)) {
            i++;
            continue;
        }
        // Number (multi-digit or float)
        if (isdigit(c) || c == '.') {
            string number;
            while (i < expr.length() && (isdigit(expr[i]) || expr[i] == '.')) {
                number += expr[i];
                i++;
            }
            output += number + " ";
        }
        // Left parenthesis
        else if (c == '(') {
            s.push(c);
            i++;
        }
        // Right parenthesis
        else if (c == ')') {
            while (!s.empty() && s.top() != '(') {
                output += s.top();
                output += " ";
                s.pop();
            }
            if (!s.empty()) s.pop(); // remove '('
            i++;
        }
        // Operator
        else {
            while (!s.empty() && precedence(c) <= precedence(s.top()) && 
                  !(isRightAssociative(c) && precedence(c) == precedence(s.top()))) {
                output += s.top();
                output += " ";
                s.pop();
            }
            s.push(c);
            i++;
        }
    }
    // Pop remaining operators
    while (!s.empty()) {
        output += s.top();
        output += " ";
        s.pop();
    }
    return output;
}

// Evaluate a postfix expression (supports floats)
double evaluatePostfix(const string &postfix) {
    stack<double> st;
    stringstream ss(postfix);
    string token;

    while (ss >> token) {
        // Number
        if (isdigit(token[0]) || (token.size() > 1 && (isdigit(token[1]) || token[0] == '.')) || token[0] == '.') {
            st.push(stod(token));
        } 
        // Operator
        else {
            double b = st.top(); st.pop();
            double a = st.top(); st.pop();
            switch (token[0]) {
                case '+': st.push(a + b); break;
                case '-': st.push(a - b); break;
                case '*': st.push(a * b); break;
                case '/': st.push(a / b); break;
                case '^': st.push(pow(a, b)); break;
                default: throw runtime_error("Invalid operator");
            }
        }
    }
    return st.top();
}

int main() {
    cout << "Enter an arithmetic expression (supports +, -, *, /, ^, parentheses, floats):\n";
    string infix;
    getline(cin, infix);

    try {
        string postfix = infixToPostfix(infix);
        cout << "Postfix: " << postfix << endl;
        double result = evaluatePostfix(postfix);
        cout << "Result: " << result << endl;
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
    return 0;
}
