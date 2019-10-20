/*Trabalho 1 da disciplina software básico UnB*/		
			/*Autores*/
/*Alex Alves Bernardes    14/0015213*/       
/*João Lucas da Cruz      14/0166939*/






#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;


/* Lista de opcodes */

#define ADD       1
#define SUB       2
#define MULT      3
#define DIV       4
#define JMP       5
#define JMPN      6
#define JMPP      7
#define JMPZ      8
#define COPY      9
#define LOAD      10
#define STORE     11
#define INPUT     12
#define OUTPUT    13
#define STOP      14



void divExec(vector <int> **mem,int **acc,int arg_1){
	
	cout<<"DIV "<<arg_1<<endl;
	
	if ((int) (*mem)->size() <= arg_1)
		(*mem)->resize(arg_1 + 1,0);
		
	**acc = **acc/(*mem)->at(arg_1);
	cout<<"	ACC = "<<**acc<<"\n"<<endl;
}

void multExec(vector <int> **mem,int **acc,int arg_1){
	
	cout<<"MULT "<<arg_1<<endl;
	
	if ((int) (*mem)->size() <= arg_1)
		(*mem)->resize(arg_1 + 1,0);
		
	**acc = **acc * (*mem)->at(arg_1);
	cout<<"	ACC = "<<**acc<<"\n"<<endl;
}

/*-----------------------------------------------------subExec--------------------------------------------------------------------------*/

void subExec(vector <int> **mem,int **acc,int arg_1){
	
	cout<<"SUB "<<arg_1<<endl;
	
	if ((int) (*mem)->size() <= arg_1)
		(*mem)->resize(arg_1 + 1,0);
		
	**acc = **acc - (*mem)->at(arg_1);
	cout<<"	ACC = "<<**acc<<"\n"<<endl;
}

/*------------------------------------------------------addExec-------------------------------------------------------------------------*/

void addExec(vector <int> **mem,int **acc,int arg_1){
	
	cout<<"ADD "<<arg_1<<endl;
	
	if ((int) (*mem)->size() <= arg_1)
		(*mem)->resize(arg_1 + 1,0);
		
	**acc = **acc + (*mem)->at(arg_1);
	cout<<"	ACC = "<<**acc<<"\n"<<endl;
}
/*------------------------------------------------------copyExec------------------------------------------------------------------------*/

void copyExec (vector <int> **mem,int acc,int arg_1,int arg_2){
	
	cout<<"COPY "<<arg_1<<" "<<arg_2<<endl;
	
		if ((int) (*mem)->size() <= arg_1 || (int) (*mem)->size() <= arg_1){
				
				if(arg_1 >= arg_2){
					(*mem)->resize(arg_1 + 1,0);
				}else{
					(*mem)->resize(arg_2 + 1,0);
				}	
		}
		
		(*mem)->at(arg_2) = (*mem)->at(arg_1);
		cout<<"	MEM["<< arg_2 <<"] = "<< (*mem)->at(arg_2)<<"\n"<<endl;
}

/*------------------------------------------------------loadExec------------------------------------------------------------------------*/

void loadExec(vector <int> **mem,int **acc,int arg_1){
	
	cout<<"LOAD "<<arg_1<<endl;
	
	if ((int) (*mem)->size() <= arg_1)
		(*mem)->resize(arg_1 + 1,0);
		
		**acc = (*mem)->at(arg_1);
		
	cout<<"	MEM["<< arg_1 <<"] = "<< (*mem)->at(arg_1)<<endl;
	cout<<"	ACC = "<<**acc<<"\n"<<endl;
	
}
/*-----------------------------------------------------outputExec----------------------------------------------------------------------*/

void outputExec(vector <int> **mem,int acc,int arg_1){
	
	cout<<"OUTPUT "<<arg_1<<endl;
	
	if ((int) (*mem)->size() <= arg_1)
		(*mem)->resize(arg_1 + 1,0);
	
	cout<<"	O valor de saída é: "<< (*mem)->at(arg_1)<<"\n"<<endl;
}


/*------------------------------------------------------inputExec----------------------------------------------------------------------*/

void inputExec(vector <int> **mem,int acc,int arg_1){
	
	int val;
	
	cout<<"INPUT "<<arg_1<<endl;
	
	if ((int) (*mem)->size() <= arg_1){
		
		(*mem)->resize(arg_1 + 1,0);
	}
	cout<<"	Favor, inserir um valor numérico: ";
	cin >> val;
	(*mem)->at(arg_1) = val;
	cout<<"	MEM["<< arg_1 <<"] = "<< (*mem)->at(arg_1)<<"\n"<<endl;
}

/*------------------------------------------------------storeExec---------------------------------------------------------------------*/

void storeExec(vector <int> **mem,int acc,int arg_1){
	
		int tamanho;
		tamanho = (int) (*mem)->size();
		
		cout<<"STORE "<<arg_1<<endl;
		
	if (tamanho <= arg_1){
		
		(*mem)->resize(arg_1 + 1,0);
		tamanho=arg_1;
	}	
	(*mem)->at(arg_1) = acc;
	
	cout<<"	ACC = "<<acc<<endl;
	cout<<"	MEM["<< arg_1 <<"] = "<< (*mem)->at(arg_1)<<"\n"<<endl;	
}


/*-------------------------------------------------------getInst----------------------------------------------------------------------*/


void getInst (std::fstream *objFile, int *acc, vector <int> *mem){
	
	int arg_1,arg_2,i=0,aux;
	vector <int> inst_mem;
	string space;
	
	if(!(*objFile)){
		cout<<"Não foi possível abrir o arquivo objeto"<<endl;
	}
	
	
	while(*objFile>>space){
		if(space == "OO"){
			mem->resize(mem->size()+1,0);
		}else{
			aux = stoi(space);
			mem->push_back(aux);
		}
	}
	while(i < (int) mem->size()) {
		
		switch (mem->at(i)){	
			case ADD:
				arg_1 = mem->at(i+1);	
				addExec(&mem,&acc,arg_1);
				i+=2;
			break;
			
			case SUB:
				arg_1 = mem->at(i+1);	
				subExec(&mem,&acc,arg_1);
				i+=2;
			break;
			
			case MULT:
				arg_1 = mem->at(i+1);	
				multExec(&mem,&acc,arg_1);
				i+=2;
			break;
			
			case DIV:
				arg_1 = mem->at(i+1);	
				divExec(&mem,&acc,arg_1);
				i+=2;
			break;
			
			case JMP:
				cout<<"JMP"<<endl;
				cout<<"	ACC = "<<*acc<<"\n"<<endl;
				arg_1 = mem->at(i+1);
				i = arg_1;
			break;
			
			case JMPN:
				cout<<"JMPN"<<endl;
				cout<<"	ACC = "<<*acc<<"\n"<<endl;
				if (*acc < 0){
					arg_1 = mem->at(i+1);
					i = arg_1;
				}else{
						i+=2;
				}
			break;
			
			case JMPP:
				cout<<"JMPP"<<endl;
				cout<<"	ACC = "<<*acc<<"\n"<<endl;
				if (*acc > 0){
					arg_1 = mem->at(i+1);
					i = arg_1;
				}else{
						i+=2;
				}
			break;
			
			case JMPZ:
				cout<<"JMPZ"<<endl;
				cout<<"	ACC = "<<*acc<<"\n"<<endl;
				if (*acc == 0){
					arg_1 = mem->at(i+1);
					i = arg_1;
				}else{
						i+=2;
				}
			break;
			
			case COPY:
				arg_1 = mem->at(i+1);
				arg_2 = mem->at(i+2);	
				copyExec(&mem,*acc,arg_1,arg_2);
				i+=3;	
			break;
			
			case LOAD:
				arg_1 = mem->at(i+1);	
				loadExec(&mem,&acc,arg_1);
				i+=2;	
			break;
			
			case STORE:
				arg_1 = mem->at(i+1);	
				storeExec(&mem,*acc,arg_1);
				i+=2;		
			break;
		
			case INPUT:
				arg_1 = mem->at(i+1);	
				inputExec(&mem,*acc,arg_1);
				i+=2;
			break;
			
			case OUTPUT:
				arg_1 = mem->at(i+1);
				outputExec(&mem,*acc,arg_1);
				i+=2;
			break;
			
			case STOP:
			cout<<"STOP \n"<<endl;
				i = (int)mem->size() + 1;
			break;
			
		default:
			i++;
		}
	}	
}

/*--------------------------------------------------------------MAIN-----------------------------------------------------------------*/

int main(int argc, char **argv) {
	
	int acc = 0;
	fstream objFile;
	objFile.open(argv[1]);
	vector <int> mem;
	
	getInst(&objFile,&acc,&mem);
	objFile.close();
	return (0);
}
