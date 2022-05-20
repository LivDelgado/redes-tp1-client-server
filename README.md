# redes-tp1-client-server
Implementação do trabalho prático 1 de Redes de Computadores (Sistemas de Informação UFMG, 2022/1)

# Instruções de compilação
> O sistema foi desenvolvido para ser executado em máquinas que rodam o Linux.

Para compilar o código:
```bash
make
```

A compilação resulta em 2 arquivos executáveis, os quais podem ser executados em máquinas distintas na mesma rede.

Para remover os arquivos de compilação:

```bash
make clean
```

# Executando o código

## Servidor

`./server <tipo do endereço> <porta do servidor>`

Exemplo: `./server v6 51511`

## Client

`./client <endereço do servidor> <porta do servidor>`

Exemplo: `./client ::1 51511`

## Comandos aceitos pelo servidor

Adicionar um sensor a um equipamento:
`add sensor [sensorId] in [equipmentid]`

Remover um sensor de um equipamento:
`remove sensor [sensorId] in [equipmentid]`

Listar os sensores em um equipamento:
`list sensor in [equipmentid]`

Consultar variáveis de processo (valores dos sensores):
`read [sensorId] [sensorId]...[sensorId] in [equipmentId]`

Encerrar o servidor:
`kill`


> Caso o cliente envie ao servidor um comando diferente, o servidor *desconectará este cliente**
