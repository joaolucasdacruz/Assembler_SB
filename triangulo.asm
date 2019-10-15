	TRIANGULO: equ     1  ; comentario
SECTION TEXT
		INPUT		B+1
		INPUT		H
		LOAD		B+1
		MULT		H
		copy R,dois
		IF TRIANGULO
		DIV		DOIS
		STORE		R
copy h,r+1
		OUTPUT	R
		STOP
SECTION DATA
	B:		SPACE 2
	H:		SPACE
	R:		SPACE
	DOIS:	CONST		2
a: const 1