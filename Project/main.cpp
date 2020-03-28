#include <iostream>
#include <map>
#include <fstream>
#include <vector>

using namespace std;

int main()
{
    map <string, int> cmcd; //Словарь "Комманда" -> "Код"
    map <int, string> cdtp; //Словарь "Код" -> "Тип"
    map <string, int> marks; //Словарь меток
    ifstream in;
    in.open("InputMaps.txt",ios::in);
    string cm, tp;
    int cd;
    for(int i = 0; i < 52; i++){ //Загружаем коды комманд
        in >> cm >> cd >> tp;
        cmcd[cm] = cd;
        cdtp[cd] = tp;
    }
    in.close();
    ifstream fasm;
    fasm.open("input.fasm",ios::in);
    string s1,s2;
    vector <int> stek;
    string reg1; //вспомогательный регистр
    string reg2; //вспомогательный регистр
    int num = 0; //вводимое число
    int r[16]; //регистры
    for(int i = 0; i < 16; i++) //Оформляем себе регистры
        r[i] = 0;
    r[14] = 1048575;
    bool flag = 0; //Результаты сравнений
    int form = 0; //Формируем добавление в стэк
    int a = 0; //Вспомогательное число
    while(!EOF){ //Первый проход по коду программы
        fasm >> s1;
        if(cdtp[cmcd[s1]]=="RI"){
            r[15]++;
            fasm >> reg1;
            fasm >> num;
            form = cmcd[s1]<<24;
            if (reg1.size() > 2)
                a = a*10 + reg1[2] - '0';
            form+=(a<<20);
            form+=num;
            stek.push_back(form);
        }
        if(cdtp[cmcd[s1]]=="RM"){
            r[15]++;
            fasm >> reg1;
            fasm >> num;
            form = cmcd[s1]<<24;
            a = reg1[1] - '0';
            if (reg1.size() > 2)
                a = a*10 + reg1[2] - '0';
            form+=(a<<20);
            form+=num;
            stek.push_back(form);
        }
        if(cdtp[cmcd[s1]]=="RR"){
            r[15]++;
            fasm >> reg1;
            fasm >> reg2;
            fasm >> num;
            form = cmcd[s1]<<24;
            int a = reg1[1] - '0';
            if (reg1.size() > 2)
                a = a*10 + reg1[2] - '0';
            form+=(a<<20);
            a = reg2[1] - '0';
            if (reg1.size() > 2)
                a = a*10 + reg2[2] - '0';
            form+=(a<<16);
            form+=num;
            stek.push_back(form);
        }
        if(cdtp[cmcd[s1]]=="J"){
            r[15]++;
            fasm >> num;
            form = cmcd[s1]<<24;
            form+=num;
            stek.push_back(form);
        }
        else{
            marks[s1] = r[15];
        }
    }
    fasm.close();
    ofstream  out;
    out.open("output.txt",ios::out);
    out.close();
    return 0;
}
