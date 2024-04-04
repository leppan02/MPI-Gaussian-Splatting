#include "arr.h"
#include<bits/stdc++.h>
using namespace std;

int main(){
    Arr<3, float> test, test2;
    test = (float)-1.;
    test2 = (float)1.;
    test2+=test;
    auto res = test + test2;
    cout<<res<<endl;
    res = test * test2;
    cout<<res<<endl;
    res = (float)1. + test2;
    cout<<res<<endl;
    res = (float)2. * test2;
    cout<<res<<endl;
}

/*
xx    yy zz
xz xy yz 
*/