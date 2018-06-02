# coding=utf-8

import os 
import argparse as arg
import utilidades as utl


'''
Este arquivo gere individualmente uma lista de frames, para uma extensaso especifica, por padrao .jpg
considerando um diretorio infomrado.
Para gerar para diversas listas, deve-se chamar um programa auxiliar, como listTodos.py
'''

def _getArgs():
    parser = arg.ArgumentParser()
    parser.add_argument("caminho", help="Caminho comum de onde sera lido os arquivos")
    parser.add_argument("subpasta" , help = "Subpasta, escolha entre: anotacao, BB ou videoS")
    parser.add_argument("categoria", help ="nome da pasta da categoria do video" )
    parser.add_argument("video", help = "Nome do video a ser lido")
    parser.add_argument("-extensao", "-e", help="Extensao dos arquivos que serao uma lista", default = ".jpg")
    return parser.parse_args()

def _escrever(caminho,lista):
    print(caminho)
    file = open(os.path.join(caminho,'lista.list'),"w")
    file.writelines(str(len(lista))+'\n')
    for i in lista:
        file.writelines(i+'\n')
    file.close()


def linhasAnotadas(caminhoAnotacoes,lista):
    
    # abre o arquivo, passando o caminho no mesmo argumento
    file = open(caminhoAnotacoes,'r')
    linhas = file.readlines()
    file.close()
    linhaEspaco = []
    listaDeFrames = []
    for linha,i in zip(linhas,range(len(linhas))):
        linhaEspaco.append(linha.split(' '))
        listaDeFrames.append(linhaEspaco[i][0]) # lista que contem apenas os numeros da linhas das anotaçoes
    primeiroFrame = int(listaDeFrames[0]) -1
    del lista[0:primeiroFrame]
    return lista

# caminho, categoria, video
def _gerarLista(caminho,subpasta,categoria,video,extensao):
    caminhoLer = os.path.join(os.path.join(os.path.join(caminho,subpasta),categoria),video)
    print('Processando... 'caminhoLer)
    lista = os.listdir(caminhoLer)
    listaFinal = []
    for i in lista:
        if i.endswith(extensao):
            listaFinal.append(os.path.join(caminhoLer,i))
    listaFinal.sort()
    #nessa linha é fornecido o endereço referente a anotação do respectivo video - hardcoded
    caminhoAnotacoes = os.path.join(os.path.join(os.path.join(caminho,'anotacao'),categoria),video+'.ann')
    #elimina os frames lixo, que não possuem anotação inicial
    listaFinal = linhasAnotadas(caminhoAnotacoes,listaFinal)
    _escrever(caminhoLer,listaFinal)

def _main(args):
    caminho = args.caminho
    subpasta = args.subpasta
    categoria = args.categoria
    video = args.video
    ext = args.extensao
    _gerarLista(caminho,subpasta,categoria,video,ext)

if __name__ == '__main__':
    args = _getArgs()
    _main(args)


