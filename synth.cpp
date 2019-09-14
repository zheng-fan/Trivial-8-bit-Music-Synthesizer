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
const int F = 48000;  // 音乐采样率，单位Hz
const int DEPTH = 16; // 音乐位深
//const int DEPTH=8; // 音乐位深
const int LVL = (1 << 14) - 1; // max volume, 4 ch * LVL < unsigned short
//const int LVL=(1<<10)-1; // max volume, 4 ch * LVL < unsigned short
const double keyf[] = {
    27.5, 29.1352, 30.8677,
    32.7032, 34.6478, 36.7081, 38.8909, 41.2034, 43.6535, 46.2493, 48.9994, 51.9131, 55, 58.2705, 61.7354,
    65.4064, 69.2957, 73.4162, 77.7817, 82.4069, 87.3071, 92.4986, 97.9989, 103.826, 110, 116.541, 123.471,
    130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942,
    261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883,
    523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767,
    1046.5, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760, 1864.66, 1975.53,
    2093, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520, 3729.31, 3951.07,
    4186.01}; // 钢琴88键的频率表，按八度划分（十二平均律）
string keyname[] = {
    "A-0", "A#0", "B-0",
    "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
    "C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
    "C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
    "C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
    "C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
    "C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
    "C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "A-7", "A#7", "B-7",
    "C-8"};
string noisename[] = {"0-#", "1-#", "2-#", "3-#", "4-#", "5-#", "6-#", "7-#", "8-#", "9-#", "A-#", "B-#", "C-#", "D-#", "E-#", "F-#"};
map<string, int> nametokey;
const double SecondPerKey = 0.065;                    // 每个音符的持续时间
const int trimNum = 1;                                // 每个音符分割成的小块，1或8的整数倍
const double SecondPerBlock = SecondPerKey / trimNum; // 每个小块的时间
const int S = F * SecondPerBlock;                     // 每个小块需要的采样点个数
const int SK = S * trimNum;                           // 每个音符需要的采样点个数
typedef unsigned short levelval;                      // 位深为16所以使用short
// 给定时间t，不同波形的值
levelval getPulse(int key, double t) {
    double T = 1 / keyf[key];
    double percent = fmod(t, T) / T;
    if (percent < 0.5)
        return LVL;
    else
        return 0;
}
levelval getTriangle(int key, double t) {
    double T = 1 / keyf[key];
    double percent = fmod(t, T) / T;
    if (percent < 0.5)
        return LVL * percent;
    else
        return LVL * (1 - percent);
}
levelval rnd[50005];
levelval getNoise(int key, double t) {
    double T = 1 / keyf[key];
    double percent = fmod(t, T) / T;
    if (percent < rnd[(int)(percent * S)] * 1.0 / LVL)
        return rnd[(int)(percent * S)];
    else
        return 0;
}
void getWave(int type, int key, levelval res[], int vol = 15) {
    levelval (*f[])(int, double) = {getPulse, getPulse, getTriangle, getNoise};
    for (int i = 0; i < S; i++)
        res[i] = f[type](key, i * 1.0 / F) * 1.0 * vol / 15;
}
struct WAVHeader {
    char RIFF[4];               // 资源文件交换标志RIFF
    unsigned LEN;               // 从下个地址开始到文件末尾的字节数
    char WAV[4];                // WAV文件标志WAVE
    char FMT[4];                // 波形文件标志fmt ，最后一位是0x20空格
    unsigned SubchunkSize;      // 子Chunk的文件头大小，对于WAV这个子Chunk该值为0x10
    unsigned short DATATYPE;    // 格式类型，值为1时，表示数据为线性PCM编码
    unsigned short CH;          // 声道数
    unsigned F;                 // 采样频率
    unsigned BYTERATE;          // 波形文件每秒的字节数=采样率*PCM位深/8*声道数
    unsigned short DATAUNITLEN; // DATA数据块单位长度=声道数*PCM位深/8
    unsigned short BITDEPTH;    // PCM位深
    char DATA[4];               // 数据标志data
    unsigned DATALEN;           // 数据部分总长度（字节数）
};
WAVHeader getHeader(int num) {
    WAVHeader res;
    memcpy(res.RIFF, "RIFF", sizeof(res.RIFF));
    memcpy(res.WAV, "WAVE", sizeof(res.WAV));
    memcpy(res.FMT, "fmt ", sizeof(res.FMT));
    res.SubchunkSize = 0x10;
    res.DATATYPE = 1;
    res.CH = 2;
    res.F = F;
    res.BITDEPTH = DEPTH;
    res.BYTERATE = res.F * res.BITDEPTH / 8 * res.CH;
    res.DATAUNITLEN = res.CH * res.BITDEPTH / 8;
    memcpy(res.DATA, "data", sizeof(res.DATA));
    res.DATALEN = num * res.DATAUNITLEN;
    res.LEN = res.DATALEN + 44 - 8;
    return res;
}
// 得到单位数据块的个数
int getNum(int keynum) {
    return keynum * SK;
}
levelval table[8][4][88][S];
levelval emp[S] = {};
void getTable() {
    // 音量大小变化表
    int v[][8] = {{15, 15, 15, 15, 15, 10, 7, 7}, {15, 15, 15, 15, 15, 10, 7, 7}, {15, 15, 15, 15, 15, 15, 15, 15}, {15, 13, 11, 9, 7, 4, 2, 0}};
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 88; k++)
                getWave(j, k, table[i][j][k], v[j][i]);
}
void init() {
    srand(233333333);
    for (int i = 0; i < 50005; i++)
        rnd[i] = rand() % LVL;
    for (int i = 0; i < 88; i++)
        nametokey[keyname[i]] = i;
    for (int i = 0; i < 16; i++)
        nametokey[noisename[i]] = i * 5;
    getTable();
}
map<vector<levelval>, int> vmp;
int vmpsz = 0;
levelval sTable[8 * 4 * 88][S];
unsigned short addrtable[1000005][4];
unsigned short *big_endian(unsigned short &x) {
    unsigned char *a, *b;
    a = (unsigned char *)(&x);
    b = (unsigned char *)(&x) + 1;
    swap(*a, *b);
    return &x;
}
int main() {
    init();
    cout << S << endl;
    cout << sizeof(table) << endl;
    freopen("score/score2.txt", "r", stdin);
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
            levelval x;
            int volflag, emptyflag;
            for (int j = 0; j < 4; j++) {
                if (j == 2)
                    continue;
                volflag = emptyflag = 0;
                if (name[j] == "...") {
                    if (state[j] == "")
                        volflag = 0, emptyflag = 1, name[j] = "";
                    else
                        volflag = 1, emptyflag = 0, name[j] = state[j];
                }
                state[j] = name[j];
                int idx1, idx2, idx3;
                for (int k = 0; k < trimNum; k++) {
                    idx1 = k * 8 / trimNum;
                    if (volflag)
                        idx1 = 7;
                    idx2 = j;
                    idx3 = nametokey[state[j]];
                    levelval *val = table[idx1][idx2][idx3];
                    if (emptyflag)
                        val = emp;
                    vector<levelval> vval(val, val + S + 1);
                    int idx;
                    if (vmp.find(vval) != vmp.end())
                        idx = vmp[vval];
                    else {
                        vmp[vval] = vmpsz++;
                        idx = vmp[vval];
                        memcpy(sTable[idx], val, sizeof(levelval) * S);
                    }
                    addrtable[blocknum + k][j] = idx;
                }
            }
            blocknum += trimNum;
        }
        gets(s);
        gets(s);
    }
    cout << vmpsz, debug;
    cout << blocknum, debug;
    FILE *fp = fopen("test.wav", "wb");
    WAVHeader header = getHeader(blocknum * S);
    fwrite(&header, sizeof(header), 1, fp);
    for (int i = 0; i < blocknum; i++) {
        for (int k = 0; k < S; k++) {
            levelval sum = 0;
            for (int j = 0; j < 4; j++) {
                if (j == 2)
                    continue;
                sum += sTable[addrtable[i][j]][k];
            }
            fwrite(&sum, sizeof(sum), 1, fp);
            fwrite(&sum, sizeof(sum), 1, fp);
        }
    }
    FILE *fpsTable = fopen("sTable.dat", "wb");
    for (int i = 0; i < vmpsz; i++)
        for (int j = 0; j < S; j++)
            fwrite(big_endian(sTable[i][j]), sizeof(sTable[i][j]), 1, fpsTable);
    cout << hex << sTable[0][0], debug;
    FILE *fpaddrtable = fopen("addrtable.dat", "wb");
    for (int i = 0; i < blocknum; i++)
        for (int j = 0; j < 4; j++)
            fwrite(big_endian(addrtable[i][j]), sizeof(addrtable[i][j]), 1, fpaddrtable);
    return 0;
}
