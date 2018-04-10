import os
import argparse as arg

def _getArgs():
    parser = arg.ArgumentParser()
    parser.add_argument("caminho", help = "caminho do arquivo de onde sera tirado a informacao inicial do bounding box")
    parser.add_argument("gt", help="nome do arquivo que tem o bounding box inicial")
    return parser.parse_args()

def gerarBBInicial(lista):
    linha = lista[0].split()
    linhaInt = []
    for i in range(0,len(linha)-1):
        i +=1
        linhaInt.append(int(round(float(linha[i]))))
    
    width = []
    height = []
    for i in range(0,len(linhaInt),2):
        width.append(linhaInt[i])
    for i in range(1,len(linhaInt),2):
        height.append(linhaInt[i])

    print(width)
    print(height)
    pontos = [] 
    pontos.append(str(min(width)))
    pontos.append(str(min(height)))
    pontos.append(str(max(width)))
    pontos.append(str(max(height)))
    bBIncicial = []
    virgula = ","
    bBIncicial = virgula.join(pontos)
    return bBIncicial
    

def _escrever(caminho,gt,lista):
    file = open(caminho+gt+'.txt',"w")
    #file.writelines(str(len(lista))+'\n')
    file.writelines(lista+'\n')
    file.close()

def _ler(arquivo):
    file = open(arquivo,"r")
    k = file.read().split('\n')
    file.close()
    return k

def _main(caminho,gt):
    _escrever(caminho,gt,gerarBBInicial(_ler(caminho+'/'+gt)))

if __name__ == '__main__' :
    args = _getArgs()
    _main(args.caminho,args.gt)