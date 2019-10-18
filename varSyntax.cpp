#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;


int varSyntax(string var){
	
	int i, aux;
	
	if(var.size() >= 50){
		cout<<"erro, o tamanho máximo da label é de 50 caracteres: "<<var<<endl;
		return(-1);
	}
	aux = var[0];
	if(48<= aux  && aux<= 57){
		cout<<"erro, o primeiro caractere da LABEL não pode ser um número: "<<var<<endl;
		return(-1);
	}else{
		for(i=0;i<(int)var.size();i++){			
			aux = var[i];
			if(!(48 <= aux && aux<= 57) && !(65<= aux && aux<=90) && !(97<= aux && aux<=122) && var[i] != '_' ){
				cout<<"erro, token com caractere inválido: "<<var<<endl;
				return(-1);
			}
		}
	}
	
	return (0);
}


int main(){
	
	int erro = 0;
	
	erro = varSyntax("piccolo");
	cout<<"erro = "<<erro<<endl;

return(0);	
}
