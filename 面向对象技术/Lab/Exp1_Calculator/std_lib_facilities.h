/*
   std_lib_facilities.h
*/
/*
	simple "Programming: Principles and Practice using C++ (second edition)" course header to
	be used for the first few weeks.
	It provides the most common standard headers (in the global namespace)
	and minimal exception/error support.
	Students: please don't try to understand the details of headers just yet.
	All will be explained. This header is primarily used so that you don't have
	to understand every concept all at once.
	By Chapter 10, you don't need this file and after Chapter 21, you'll understand it
	Revised April 25, 2010: simple_error() added
	
	Revised November 25 2013: remove support for pre-C++11 compilers, use C++11: <chrono>
	Revised November 28 2013: add a few container algorithms
	Revised June 8 2014: added #ifndef to workaround Microsoft C++11 weakness
*/
#ifndef H112
#define H112 251113L
#include<iostream>          //
#include<iomanip>           //流操作库
#include<fstream>           //文件流
#include<sstream>           //字符串流
#include<cmath>
#include<cstdlib>
#include<string>
#include<list>              //链表基础库
#include<forward_list>     //单链表库
#include<vector>
#include<unordered_map>     //哈希表
#include<algorithm>         //标准算法库
#include <array>
#include <regex>            //正则表达
#include<random>
#include<stdexcept>         //标准异常类
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
typedef long Unicode;       //8字节unicode
//------------------------------------------------------------------------------
using namespace std;                    //标准命名空间
template<class T> string to_string(const T& t)
{
    ostringstream os;
    os << t;
    return os.str();
}
struct Range_error : out_of_range {	            //vector range错误报告，例如下面检查越界时会用到
    int index;
    Range_error(int i) :out_of_range("Range error: "+to_string(i)), index(i) { }
};
//范围检查向量（不进行迭代器检查）：
template< class T> struct Vector : public std::vector<T> {          //模版类
    using size_type = typename std::vector<T>::size_type;
#ifdef _MSC_VER
    // microsoft doesn't yet support C++11 inheriting constructors
Vector() { }
explicit Vector(size_type n) :std::vector<T>(n) {}
Vector(size_type n, const T& v) :std::vector<T>(n,v) {}
template <class I>
Vector(I first, I last) : std::vector<T>(first, last) {}
Vector(initializer_list<T> list) : std::vector<T>(list) {}
#else
    using std::vector<T>::vector;	// inheriting constructor
#endif
    T& operator[](unsigned int i) // rather than return at(i);
    {                                               //调用operator[]方法，数组是否越界
        if (i<0||this->size()<=i)
            throw Range_error(i);                   //抛出异常报告
        return std::vector<T>::operator[](i);
    }
    const T& operator[](unsigned int i) const       //加const表示不修改类中的成员变量
    {
        if (i<0||this->size()<=i) throw Range_error(i);
        return std::vector<T>::operator[](i);
    }
};
// disgusting macro hack to get a range checked vector:
#define vector Vector
// trivially range-checked string (no iterator checking):
struct String : std::string {                           //继承
    using size_type = std::string::size_type;           //using别名指定
//	using string::string;                               //string的越界检查
    char& operator[](unsigned int i) // rather than return at(i);
    {
        if (i<0||size()<=i) throw Range_error(i);
        return std::string::operator[](i);
    }
    const char& operator[](unsigned int i) const
    {
        if (i<0||size()<=i) throw Range_error(i);
        return std::string::operator[](i);
    }
};
namespace std {
    template<> struct hash<String>
    {
        size_t operator()(const String& s) const
        {
            return hash<std::string>()(s);
        }
    };
} // of namespace std
struct Exit : runtime_error {
    Exit(): runtime_error("Exit") {}
};
// error() simply disguises throws:
inline void error(const string& s)          //inline函数，运行时错误异常类
{
    throw runtime_error(s);
}
inline void error(const string& s, const string& s2)
{
    error(s+s2);
}
inline void error(const string& s, int i)
{
    ostringstream os;
    os << s <<": " << i;
    error(os.str());
}
template<class T> char* as_bytes(T& i)	// needed for binary I/O
{
    void* addr = &i;	          //获取用于存储对象的内存的第一个字节的地址
    return static_cast<char*>(addr); // treat that memory as bytes
}
inline void keep_window_open()
{
    cin.clear();                            //更改cin的状态标示符
    cout << "Please enter a character to exit\n";
    char ch;                                //输入任意值结束
    cin >> ch;
    return;
}
inline void keep_window_open(string s)
{                                           //输入s结束
    if (s=="") return;
    cin.clear();
    cin.ignore(120,'\n');
    for (;;) {
        cout << "Please enter " << s << " to exit\n";
        string ss;
        while (cin >> ss && ss!=s)
            cout << "Please enter " << s << " to exit\n";
        return;
    }
}
// error function to be used (only) until error() is introduced in Chapter 5:
inline void simple_error(string s)	                // 报错误流s并退出程序
{
    cerr << "error: " << s << '\n';              //标准错误输出，输出s
    keep_window_open();		                // for some Windows environments
    exit(1);
}
// make std::min() and std::max() accessible on systems with antisocial macros:
#undef min
#undef max
// run-time checked narrowing cast (type conversion). See ???.
template<class R, class A> R narrow_cast(const A& a)
{                                               //运行时检查短截的强制类型转换
    R r = R(a);
    if (A(r)!=a) error(string("info loss"));
    return r;
}
// random number generators. See 24.7.
inline int randint(int min, int max)
{                                               //随机数生成器
    static default_random_engine ran;
    return uniform_int_distribution<>{min, max}(ran);
}
inline int randint(int max) { return randint(0, max); }
//inline double sqrt(int x) { return sqrt(double(x)); }	// to match C++0x
// container algorithms. See 21.9.
template<typename C>
using Value_type = typename C::value_type;
template<typename C>
using Iterator = typename C::iterator;
template<typename C>
// requires Container<C>()
void sort(C& c)
{
    std::sort(c.begin(), c.end());
}
template<typename C, typename Pred>
// requires Container<C>() && Binary_Predicate<Value_type<C>>()
void sort(C& c, Pred p)
{                                       //调用排序接口函数实现排序
    std::sort(c.begin(), c.end(), p);
}
template<typename C, typename Val>
// requires Container<C>() && Equality_comparable<C,Val>()
Iterator<C> find(C& c, Val v)
{                                       //调用查找接口函数实现查找
    return std::find(c.begin(), c.end(), v);
}
template<typename C, typename Pred>
// requires Container<C>() && Predicate<Pred,Value_type<C>>()
Iterator<C> find_if(C& c, Pred p)
{
    return std::find_if(c.begin(), c.end(), p);
}
#endif //H112