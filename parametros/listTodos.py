# coding=utf-8
import os
import argparse as arg

'''
Chama as funções de listGenerator.py para gerar lista de frames para cada video.
Gera diversas lista ao mesmo tempo, desde que seja obdecido o padrao de arquivos abaixo:
 
Este arquivo so possui sentido se o banco de dados esta com caminhos padronizados
(.../)  |-videos
        |   |-categorias
        |   |   |-categoria 1
        |       |     |-video 1
        |       |     |   .
        |       |     |   .
        |       |     |   .
        |       |     | -video N
        |       |  .   
        |       |  .
        |       |  .
        |       |-categoria N
        |
        |-BB
        |    | ... (mesmo formato de cima)
        |    .
        |    .
        |    .
        |-anotacoes
        |   | ... (mesmo formato de cima)
        |   .
        |   .
        |   .
        |
Deve-se obedecer esse formato de pastas para que este arquivo seja util.
'''

encode = "utf-8"

def _getArgs():
    parser = arg.ArgumentParser(description = "Gerar os BB iniciais para todas a anataçoes")
    parser.add_argument("caminho", help="Caminho comum a todas as pastas")
    parser.add_argument("--subpasta", '-s', help ="Nome da subpasta", default = "videos")
    #parser.add_argument("-extensao", "-e", help="Extensao dos arquivos que serao uma lista", default = ".jpg")
    return parser.parse_args()

def _main(args):
    caminho = args.caminho
    subpasta = args.subpasta
    categorias = os.listdir(os.path.join(caminho,subpasta))
    for categoria in categorias:
        videos = os.listdir(os.path.join(os.path.join(caminho,subpasta),categoria))
        for video in videos:
            comando = 'python ' + 'listGenerator.py ' + caminho + ' ' + subpasta + ' ' + categoria + ' ' + video
            os.system(comando)

if __name__ == '__main__':
    args =_getArgs()
    _main(args)