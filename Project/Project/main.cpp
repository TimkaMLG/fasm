#include <iostream>
#include <map>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <sstream>

#define CONST_20 1048576u
#define CONST_16 65536u

using namespace std;

struct commands {
    const string command;
    const string type;
    const unsigned int code;
};
//Переделать
vector<string> split(const string &s, char delim) {
    vector<string> VecStr;
    istringstream ss(s);
    string String;
    while(ss>>String)
        VecStr.push_back(String);
    return VecStr;
}
//Заполняем стэк коммандами(Почему-то не работает) UPD Заработало
int push_line(const vector<string> &commands, int address, int false_address, unsigned int *r, map <string, unsigned int> &commandcode, map <unsigned int, string> &codetype, map <string,unsigned int> &marks, vector <unsigned int> &fupm_stack){//maps, steck, marks
    unsigned int kol, res = commandcode[commands[0]];
    if (commands[0] == "end"){
        r[15] = (unsigned int) marks[commands[1]];
        return 0;
    }
    if (commands[0] == "word"){
        return 0;
    }
    string type = string(codetype[res]);
    if (commands[0] == "ret"){
        kol = (unsigned int) stoi(commands[1]);
        fupm_stack[address] = (res << 24u) | kol;
        return 0;
    }
    if (type == "RM"){
        auto registr = (unsigned char) stoi(commands[1].substr(1, 2));
        kol = (unsigned int) stoi(commands[2]);
        if (marks.find(commands[2]) != marks.end()) {
                kol = marks.find(commands[2])->second;
            }
        res = (((res << 4u) | registr) << 20u) | kol;
    }
    else if (type == "RR"){
            auto registr1 = (unsigned char) stoi(commands[1].substr(1, 2));
            auto registr2 = (unsigned char) stoi(commands[2].substr(1, 2));
            kol = (unsigned int) stoi(commands[3]);
            if (marks.find(commands[3]) != marks.end()){
                kol = marks.find(commands[3])->second;
            }
            res = (((((res << 4u) | registr1) << 4u) | registr2) << 16u) | kol;
    }
    else if (type == "RI") {
            auto registr = (unsigned char) stoi(commands[1].substr(1, 8));
            kol = (unsigned int) stoi(commands[2]);
            if (marks.find(commands[2]) != marks.end()) {
                    kol = marks.find(commands[2])->second;
            }
            res = (((res << 4u) | registr) << 20u) | kol;
    }
    else if (type == "J") {
            kol = (unsigned int) stoi(commands[1]);
            if (marks.find(commands[1]) != marks.end()) {
                kol = marks.find(commands[1])->second;
            }
            res = (res << 24u) + kol;
    }
    //print(res);
    fupm_stack[address] = res;
    return 0;
}

int main()
{
    map <string, unsigned int> commandcode; //Словарь "Комманда" -> "Код"
    map <unsigned int, string> codetype; //Словарь "Код" -> "Тип"
    map <string,unsigned int> marks; //Словарь меток
    vector <unsigned int> fupm_stack; //Память
    fupm_stack.resize(CONST_20);
    enum SYSCALLS {
        EXIT = 0, SCANINT = 100, SCANDOUBLE = 101, PRINTINT = 102, PRINTDOUBLE = 103, GETCHAR = 104, PUTCHAR = 105
    };
    const struct commands Commanders[52] = {{"halt", "RI",    0},
                                           {"syscall", "RI", 1},
                                           {"add", "RR",     2},
                                           {"addi", "RI",    3},
                                           {"sub", "RR",     4},
                                           {"subi", "RI",    5},
                                           {"mul", "RR",     6},
                                           {"muli", "RI",    7},
                                           {"div", "RR",     8},
                                           {"divi", "RI",    9},
                                           {"lc", "RI",      12},
                                           {"shl", "RR",     13},
                                           {"shli", "RI",    14},
                                           {"shr", "RR",     15},
                                           {"shri", "RI",    16},
                                           {"and", "RR",     17},
                                           {"andi", "RI",    18},
                                           {"or", "RR",      19},
                                           {"ori", "RI",     20},
                                           {"xor", "RR",     21},
                                           {"xori", "RI",    22},
                                           {"not", "RI",     23},
                                           {"mov", "RR",     24},
                                           {"addd", "RR",    32},
                                           {"subd", "RR",    33},
                                           {"muld", "RR",    34},
                                           {"divd", "RR",    35},
                                           {"itod", "RR",    36},
                                           {"dtoi", "RR",    37},
                                           {"push", "RI",    38},
                                           {"pop", "RI",     39},
                                           {"call", "RR",    40},
                                           {"calli", "J",    41},
                                           {"ret", "RI",     42},
                                           {"cmp", "RR",     43},
                                           {"cmpi", "RI",    44},
                                           {"cmpd", "RR",    45},
                                           {"jmp", "J",      46},
                                           {"jne", "J",      47},
                                           {"jeq", "J",      48},
                                           {"jle", "J",      49},
                                           {"jl", "J",       50},
                                           {"jge", "J",      51},
                                           {"jg", "J",       52},
                                           {"load", "RM",    64},
                                           {"store", "RM",   65},
                                           {"load2", "RM",   66},
                                           {"store2", "RM",  67},
                                           {"loadr", "RR",   68},
                                           {"storer", "RR",  69},
                                           {"loadr2", "RR",  70},
                                           {"storer2", "RR", 71}};
    for(auto i:Commanders){ //Загружаем коды комманд
        commandcode[i.command] = i.code;
        codetype[i.code] = i.type;
    }
    ifstream fasm; //Читаем файл
    fasm.open("input.fasm",ios::in);
    string s;
    unsigned int r[16]; //регистры
    for(int i = 0; i < 16; i++) //Оформляем себе регистры
        r[i] = 0;
    r[14] = 1048575;
    bool flag = 0; //Результаты сравнений
    while(!fasm.eof()){ //Первый проход по коду программы
        getline(fasm,s);
        s = s.substr(0, s.find_first_of(';')); //Убираем комментарии
        vector<string> commands = split(s, ' ');
        flag = false;
        for (auto &i:commands){
            //cout << i << '\n';
            if (i[i.length() - 1] == ':'){
                i.erase(i.length() - 1);
                marks[i] = r[15];
            } else{
                flag = true;
            }
        }
        r[15] += flag;
    }
    fasm.close();
    fasm.open("input.fasm",ios::in);
    r[15] = 0;
    int false_address = 1;
    while(!fasm.eof()){ //Второй проход по файлу
        getline(fasm,s);
        s = s.substr(0, s.find_first_of(';')); //Убираем комментарии
        vector<string> commands = split(s, ' ');
        for (int i = 0; i < commands.size(); i++){ //Убираем метки
            if (commands[i][commands[i].size() - 1] == ':'){
                commands.erase(commands.begin() + i);
                i--;
            }
        }
        if (commands.empty()) continue; //Если пустая строка, то идем дальше
        if (push_line(commands, r[15], false_address, r, commandcode, codetype, marks, fupm_stack) == 1) return 1; //Записываем команду в стэк
        false_address++;
        r[15]++;
    }
    fasm.close();
    ofstream  out;
    out.open("output.txt",ios::out);
    out.close();
    return 0;
}
