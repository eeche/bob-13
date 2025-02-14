calc: fb3-2func.c fb3-2.l fb3-2.y
	bison -d fb3-2.y
	flex fb3-2.l
	gcc -o $@ fb3-2func.c fb3-2.tab.c lex.yy.c -lfl -lm

clean:
	rm -f fb3-2.tab.* lex.yy.c