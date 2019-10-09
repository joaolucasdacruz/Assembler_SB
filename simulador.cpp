#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

int decodeInst (int &pos, std::ifstream *objFile){
	
	int inst;
	char str;
	
	if(objFile->is_open()){
		cout<<"Arquivo aberto com sucesso"<<endl;
	}
	
	objFile->get(str);
	cout<<str<<endl;
	inst = pos;
	return inst;
}



int main(int argc, char **argv) {
	
	//std::ifstream inFile ("listas/lista1.txt");
	std::ifstream objFile (*argv);
	int a =1,inst;
	
	inst = decodeInst(a, &objFile);
	cout<<inst<<std::endl;
	
	return (0);
}
