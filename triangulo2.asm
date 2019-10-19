	TRIANGULO: equ     0x01  ; comentario
SECTION DATA
	B:		SPACE 2
g: const -0x0c
	H:		SPACE
	R:		SPACE
	DOIS:	CONST		2
f: const   9
a: const	-1
k: space 2
p: const 7
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