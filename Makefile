all: funcoes_gerais.o listaencadeada.o createtable.o createindex.o insertinto.o delete.o select.o select_where.o funcoes_fornecidas.o programaTrab.o
	gcc funcoes_gerais.o listaencadeada.o createtable.o createindex.o insertinto.o delete.o select.o select_where.o funcoes_fornecidas.o programaTrab.o -o programaTrab -std=c99 -Wall 

insertinto.o:
	gcc -c insertinto.c -o insertinto.o

delete.o:
	gcc -c delete.c -o delete.o

createindex.o:
	gcc -c createindex.c -o createindex.o

createtable.o:
	gcc -c createtable.c -o createtable.o

select.o:
	gcc -c select.c -o select.o

select_where.o:
	gcc -c select_where.c -o select_where.o

funcoes_fornecidas.o:
	gcc -c funcoes_fornecidas.c -o funcoes_fornecidas.o

funcoes_gerais.o:
	gcc -c funcoes_gerais.c -o funcoes_gerais.o

listaencadeada.o:
	gcc -c listaencadeada.c -o listaencadeada.o
	 
programaTrab.o:
	gcc -c programaTrab.c -o programaTrab.o
	 
clean:
	rm *.o programaTrab

run:
	./programaTrab
