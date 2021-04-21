
//
// This is example code from Chapter 6.6 "Trying the first version" of
// "Software - Principles and Practice using C++" by Bjarne Stroustrup
//

#include "std_lib_facilities.h"

//------------------------------------------------------------------------------

class Token {
public:
    char kind;        // what kind of token
    double value;     // for numbers: a value
    Token(char ch)    // make a Token from a char
            :kind(ch), value(0) { }
    Token(char ch, double val)     // make a Token from a char and a double
            :kind(ch), value(val) { }
};

//------------------------------------------------------------------------------

class Token_stream {
public:
    Token_stream() :full(0), buffer(0) { }   // make a Token_stream that reads from cin
    Token get_token();      // get a Token (get() is defined elsewhere)
    void putback(Token t);    // put a Token back
    void ignore(char);
private:
    bool full;        // is there a Token in the buffer?
    Token buffer;     // here is where we keep a Token put back using putback()
};

//------------------------------------------------------------------------------
// Token °∞kind°± values:
const char number = '8'; // a floating-point number
const char quit = 'q';
const char print = ';';
// User interaction strings:
const string prompt = "> ";
const string result = "= "; // indicate that a result follows

//------------------------------------------------------------------------------

// The putback() member function puts its argument back into the Token_stream's buffer:
void Token_stream::putback(Token t)
{
    if (full) error("putback() into a full buffer");
    buffer = t;       // copy t to buffer
    full = true;      // buffer is now full
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

Token Token_stream::get_token()    // read a token from cin
{
    if (full) {       // do we already have a Token ready?
        // remove token from buffer
        full = false;
        return buffer;
    }

    char ch;
    cin >> ch;    // note that >> skips whitespace (space, newline, tab, etc.)

    switch (ch) {
        case ';':    // for "print"
        case 'q':    // for "quit"
        case '(': case ')': case '+': case '-': case '*': case '/':
            return Token(ch);        // let each character represent itself
        case '.':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            cin.putback(ch);         // put digit back into the input stream
            double val;
            cin >> val;              // read a floating-point number
            return Token(number, val); // rather than Token('8',val)
        }
        default:
            error("Bad token");
    }
}

void Token_stream::ignore(char c)
{
    if (full && c == buffer.kind) {
        full = false;
        return;
    }
    full = false;

    char ch;
    while (cin >> ch)
        if (ch == c) return;
}

//------------------------------------------------------------------------------
Token_stream ts;        // provides get() and putback()
//------------------------------------------------------------------------------

double expression();  // read and evaluate a Expression

//------------------------------------------------------------------------------

double term();        // read and evaluate a Term

//------------------------------------------------------------------------------

double primary()     // read and evaluate a Primary
{
    Token t = ts.get_token();
    switch (t.kind) {
        case '(':    // handle '(' expression ')'
        {
            double d = expression();
            t = ts.get_token();
            if (t.kind != ')') error("')' expected");
            return d;
        }
        case number:			// rather than case '8':
            return t.value;  // return the number's value
        default:
            error("primary expected");
    }
}

void clean_up_mess()
{
    ts.ignore(print);
}
//------------------------------------------------------------------------------
void calculate()
{
    while (cin)
        try{
            cout << prompt;
            Token t = ts.get_token();
            while (t.kind == print) t = ts.get_token(); // first discard all °∞prints°±
            if (t.kind == quit) return; // quit
            ts.putback(t);
            cout << result << expression() << endl;
        }
        catch (exception& e) {
            cerr << e.what() << endl; // write error message
            //clean_up_mess(); // <<< The tricky part!
        }

}
//------------------------------------------------------------------------------

int main()
try {
    calculate();
    keep_window_open("~0"); // cope with Windows console mode
    return 0;
}

catch (exception& e) {
    cerr << e.what() << endl;
    keep_window_open("~1");
    return 1;
}
catch (...) {
    cerr << "exception \n";
    keep_window_open("~2");
    return 2;
}

//------------------------------------------------------------------------------

double expression()
{
    double left = term();      // read and evaluate a Term
    Token t = ts.get_token();     // get the next token
    while (true) {
        switch (t.kind) {
            case '+':
                left += term();    // evaluate Term and add
                t = ts.get_token();
                break;
            case '-':
                left -= term();    // evaluate Term and subtract
                t = ts.get_token();
                break;
            default:
                ts.putback(t);
                return left;       // finally: no more + or -: return the answer
        }
    }
}

//------------------------------------------------------------------------------

double term()
{
    double left = primary();
    Token t = ts.get_token();     // get the next token

    while (true) {
        switch (t.kind) {
            case '*':
                left *= primary();
                t = ts.get_token();
                break;
            case '/':
            {
                double d = primary();
                if (d == 0) error("divide by zero");
                left /= d;
                t = ts.get_token();
                break;
            }
            default:
                ts.putback(t);
                return left;
        }
    }
}

//------------------------------------------------------------------------------

