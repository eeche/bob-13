
calc: fb3-1func.c fb3-1.l fb3-1.y
	bison -d fb3-1.y
	flex fb3-1.l
	gcc -o $@ fb3-1func.c fb3-1.tab.c lex.yy.c -lfl -lm

clean:
	rm -f fb3-1.tab.* lex.yy.c