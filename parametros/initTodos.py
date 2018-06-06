import os
import argparse as arg

encode = "utf-8"

"""
Chama automaticamente o arquivo initGenerator.py para gerar diversas anotacoes iniciais.
Atente-se ao padrao de arquivos.
Padrao semelhante ao presente em listTodos.py, porém com um nivel a menos.
"""

def _getArgs():
    parser = arg.ArgumentParser(description = "Gerar os BB iniciais para todas a anataçoes")
    parser.add_argument("caminho", help="Caminho comum a todas as pastas")
    parser.add_argument("-p","--pastaEscrita", help="Nome da subpasta de anotaçoes", default = "inicial")
    #parser.add_argument("-extensao", "-e", help="Extensao dos arquivos que serao uma lista", default = ".jpg")
    return parser.parse_args()

def _main(caminho,pastaEscrita):
    categorias = os.listdir(caminho)
    for categoria in categorias:
        anotacoes = os.listdir(os.path.join(caminho,categoria))
        for anotacao in anotacoes:        
            comando = 'python ' + 'initGenerator.py ' + os.path.join(caminho,categoria) + ' ' + anotacao
            os.system(comando)

if __name__ == '__main__':
    args =_getArgs()
    _main(args.caminho,args.pastaEscrita)