







Compilação dos programas em Linux (necessário g++ e C++11):

gcc test.cpp -lstdc++

g++ -std=c++11 -Wall montador.cpp -o montador
g++ -std=c++11 -Wall simulador.cpp -o simulador

Caso não haja, instalação:

sudo apt install g++
sudo apt install build-essential



Execuçao dos programas (Linux):
./montador teste.asm
./simulador teste.obj



