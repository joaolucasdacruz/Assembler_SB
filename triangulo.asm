	TRIANGULO: equ     1  ; comentario
SECTION TEXT
		INPUT		B+1
		INPUT		H
		LOAD		B+1
		MULT		p
		copy R,dois
		IF TRIANGULO
		DIV		DOIS
		STORE		R
copy h,r
		OUTPUT	R
		STOP
SECTION DATA
g: const 5
	B:		SPACE 2
	H:		SPACE
	R:		SPACE
	DOIS:	CONST		2
f: const   9
a: const	1
k: space 2
p: const 7