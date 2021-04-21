#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> array(10);
    for(int i=0;i<array.size();i++)
        array[i]=101;
    cout<<array[0]<<array[1]<<array.size()<<endl;

    return 0;
}
