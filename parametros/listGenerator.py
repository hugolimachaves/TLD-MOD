import os 
import argparse as arg


def _getArgs():
    parser = arg.ArgumentParser()
    parser.add_argument("caminho", help="Caminho de onde sera gerado os arquivos")
    parser.add_argument("-extensao", "-e", help="Extensao dos arquivos que serao uma lista", default = ".jpg")
    return parser.parse_args()

def _escrever(caminho,lista):
    file = open(caminho+'listaImagens.list',"w")
    file.writelines(str(len(lista))+'\n')
    for i in lista:
        file.writelines(i+'\n')
    file.close()


def _gerarLista(caminho,extensao):
    lista = os.listdir(caminho)
    listaFinal = []
    for i in lista:
        if i.endswith(extensao):
            listaFinal.append(caminho + i)
    _escrever(caminho,listaFinal)

def _main(args):
    caminho = args.caminho
    ext = args.extensao
    _gerarLista(caminho,ext)


if __name__ == '__main__':
    args = _getArgs()
    _main(args)
