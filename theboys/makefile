# makefile The Boys
# Carlos Maziero - DINF/UFPR, 2024/2

CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -g -std=c99
LDLIBS  = -lm
MAIN    = theboys
ENTREGA = $(MAIN)

# lista de arquivos de cabeçalho (a completar)
HDR = fila.h fprio.h conjunto.h entidades.h auxiliares.h lef.h universo.h \
      eventos_base.h eventos_missao.h eventos_invasao.h relatorio.h \
      simulacao.h

# lista de arquivos-objeto (a completar)
# não inclua conjunto.o, senão ele será removido com "make clean"
OBJ = fila.o fprio.o theboys.o auxiliares.o lef.o universo.o \
      eventos_base.o eventos_missao.o eventos_invasao.o relatorio.o \
      simulacao.o

# construir o executável
$(MAIN): $(MAIN).o $(OBJ) conjunto.o

# construir os arquivos-objeto (a completar)
$(MAIN).o: $(MAIN).c $(HDR)

# construir os TADs
fila.o: fila.c fila.h
fprio.o: fprio.c fprio.h

# construir os modulos
auxiliares.o: auxiliares.c auxiliares.h conjunto.h
lef.o: lef.c lef.h entidades.h fprio.h
universo.o: universo.c universo.h entidades.h auxiliares.h lef.h
eventos_base.o: eventos_base.c eventos_base.h entidades.h auxiliares.h lef.h
eventos_missao.o: eventos_missao.c eventos_missao.h entidades.h auxiliares.h lef.h
eventos_invasao.o: eventos_invasao.c eventos_invasao.h entidades.h lef.h
relatorio.o: relatorio.c relatorio.h entidades.h auxiliares.h
simulacao.o: simulacao.c simulacao.h entidades.h eventos_base.h \
             eventos_missao.h eventos_invasao.h relatorio.h

# executar
run: $(MAIN)
	./$(MAIN)

# testar no Valgrind
valgrind: $(MAIN)
	valgrind --leak-check=full --track-origins=yes ./$(MAIN)

# gerar arquivo TGZ para entregar
tgz: clean
	-mkdir -p /tmp/$(USER)/$(ENTREGA)
	chmod 0700 /tmp/$(USER)/$(ENTREGA)
	cp *.c *.h conjunto.o makefile /tmp/$(USER)/$(ENTREGA)
	tar czvf $(ENTREGA).tgz -C /tmp/$(USER) $(ENTREGA)
	rm -rf /tmp/$(USER)
	@echo "Arquivo $(ENTREGA).tgz criado para entrega"

# limpar arquivos temporários
clean:
	rm -f *~ $(OBJ) $(MAIN) /tmp/$(USER)/$(ENTREGA) $(ENTREGA).tgz

