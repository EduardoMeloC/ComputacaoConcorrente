#########################
## Programa Principal  ##
#########################

> Para compilar o programa principal (Sensores e Atuadores), digite:
    make

> O executável (main.out) estará na pasta bin/

> Outro executável (verbose.out) também estará na pasta bin/, nele, é possível ver os dados
  escritos no buffer

#########################
## Programa de Teste   ##
#########################

> Na pasta test/ existe o source do código que gera um script em lua para fazer testes
  automáticos da implementação de leitores e escritores com prioridade para a escrita (RnWriter).

> Para compilar o teste, entre na pasta test/ e digite:
    make

> O executável do test (test.out) estará na pasta test/

> Execute-o redirecionando a saída para um arquivo
    ./test.out > tester.lua

> Execute o script do teste com
    lua tester.lua

> Caso não seja impresso nada na saída padrão, não houve erros.
