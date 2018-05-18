import os 
import argparse as arg


def _getArgs():
    parser = arg.ArgumentParser()
    parser.add_argument("caminhoLer", help="Caminho de onde sera lido os arquivos")
    parser.add_argument("-extensao", "-e", help="Extensao dos arquivos que serao uma lista", default = ".jpg")
    return parser.parse_args()

def _escrever(caminho,lista):
    print(caminho)
    file = open(os.path.join(caminho,'lista.list'),"w")
    file.writelines(str(len(lista))+'\n')
    for i in lista:
        file.writelines(i+'\n')
    file.close()


def _gerarLista(caminhoLer,extensao):
    lista = os.listdir(caminhoLer)
    listaFinal = []
    for i in lista:
        if i.endswith(extensao):
            listaFinal.append(os.path.join(caminhoLer,i))
    listaFinal.sort()
    _escrever(caminhoLer,listaFinal)

def _main(args):
    caminhoLer = args.caminhoLer
    ext = args.extensao
    _gerarLista(caminhoLer,ext)


if __name__ == '__main__':
    args = _getArgs()
    _main(args)
