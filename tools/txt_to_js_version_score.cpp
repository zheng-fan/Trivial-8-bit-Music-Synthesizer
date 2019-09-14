#include <algorithm>
#include <bitset>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#define debug puts("-----")
#define pi (acos(-1.0))
#define eps (1e-8)
#define inf (1 << 30)
#define INF (1ll << 62)
using namespace std;
const int tot = 4 * 128;
string score[tot][4];
int main() {
    freopen("../score/score1.txt", "r", stdin);
    char s[10005];
    for (int i = 0; i < 40; i++)
        gets(s);
    int blocknum = 0;
    for (int sec = 0; sec < 4; sec++) {
        string state[4];
        string name[4];
        for (int i = 0; i < 128; i++) {
            gets(s);
            name[0] = string(s + 9, s + 12);
            name[1] = string(s + 24, s + 27);
            name[2] = string(s + 39, s + 42);
            name[3] = string(s + 54, s + 57);
            for (int k = 0; k < 4; k++)
                score[sec * 128 + i][k] = name[k];
        }
        gets(s);
        gets(s);
    }
    int k;
    int flag;
    string ret;
    int li;
    int outflag;
    // go
    li = 0;
    k = 0;
    flag = 0;
    ret = "";
    outflag = 0;
    cout << "Pulse1: ";
    for (int i = 0; i < tot; i++) {
        if (score[i][k] != "...")
            flag++;
        if (flag == 2) {
            if (outflag)
                cout << ", ";
            cout << score[li][k] << '/' << 16.0 / (i - li);
            flag--;
            outflag = 1;
            li = i;
        }
    }
    if (flag) {
        if (outflag)
            cout << ", ";
        cout << score[li][k] << '/' << 16.0 / (tot - li);
    }
    cout << endl
         << endl;
    // go
    li = 0;
    k = 1;
    flag = 0;
    ret = "";
    outflag = 0;
    cout << "Pulse2: ";
    for (int i = 0; i < tot; i++) {
        if (score[i][k] != "...")
            flag++;
        if (flag == 2) {
            if (outflag)
                cout << ", ";
            cout << score[li][k] << '/' << 16.0 / (i - li);
            flag--;
            outflag = 1;
            li = i;
        }
    }
    if (flag) {
        if (outflag)
            cout << ", ";
        cout << score[li][k] << '/' << 16.0 / (tot - li);
    }
    cout << endl
         << endl;
    // go
    li = 0;
    k = 2;
    flag = 0;
    ret = "";
    outflag = 0;
    cout << "Triangle: ";
    for (int i = 0; i < tot; i++) {
        if (score[i][k] != "...")
            flag++;
        if (flag == 2) {
            if (outflag)
                cout << ", ";
            cout << score[li][k] << '/' << 16.0 / (i - li);
            flag--;
            outflag = 1;
            li = i;
        }
    }
    if (flag) {
        if (outflag)
            cout << ", ";
        cout << score[li][k] << '/' << 16.0 / (tot - li);
    }
    cout << endl
         << endl;
    // go
    li = 0;
    k = 3;
    flag = 0;
    ret = "";
    outflag = 0;
    cout << "Noise: ";
    for (int i = 0; i < tot; i++) {
        if (score[i][k] == "..." && flag) {
            if (outflag)
                cout << ", ";
            cout << score[li][k] << '/' << 16;
            flag--;
            outflag = 1;
            li = i;
        } else if (score[i][k] != "..." && i) {
            if (outflag)
                cout << ", ";
            cout << "pause/" << 16.0 / (i - li);
            flag++;
            outflag = 1;
            li = i;
        }
    }
    if (outflag)
        cout << ", ";
    if (flag)
        cout << score[li][k] << '/' << 16.0 / (tot - li);
    else
        cout << "pause/" << 16.0 / (tot - li);
    cout << endl
         << endl;
    return 0;
}
