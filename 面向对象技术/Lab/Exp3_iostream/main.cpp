#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
//运用读写流读取文件，并实现一些ppt上的处理小功能：例如数字的转换进制，或者读一个范围的数并显示在屏幕上，或者转换精度。最后把输出写回到文件中。

struct Reading {	// a temperature reading
    int hour;		// hour after midnight [0:23]
    double temperature;	// in Fahrenheit
    Reading(int h, double t) :hour(h), temperature(t) { }
};

int main() {
    vector<Reading> temps;	// create a vector to store the readings
    int hour;
    double temperature;
    cout << "Please enter input file name: ";
    string name;
    cin >> name;
    ifstream ifs(name.c_str());	// 读文件
    if (!ifs) cout<<"can’t open input file "<<name<<endl;

    while (ifs >> hour >> temperature) {                // 把ifs文件流数据读入到Reading
        if (hour < 0 || 23 < hour)
            cout << "hour out of range "<< endl;
        temps.push_back(Reading(hour, temperature));        // store
    }

    ofstream ofs(name.c_str(),ios_base::app); // 写文件,在文件末尾写
    if (!ofs) cout<<"can’t open input file "<<name<<endl;

    ofs <<"以上数字转为16进制：\n";
    for(int i=0;i<temps.size();i++){
        cout <<dec<<temps[i].hour<<"\t"<<showbase<< hex<<(int)temps[i].temperature<<"\n";
        ofs  <<dec<<temps[i].hour<<"\t"<<showbase<< hex<<(int)temps[i].temperature<<"\n";
    }
    return 0;
}
