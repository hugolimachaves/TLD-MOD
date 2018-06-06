import os
import argparse as arg


'''Gera arquivos contendo apenas BB *INICIAIS* baseados nas anotacoes fornecidas 
pelo banco de dados alov300.'''

encode = "utf-8"
def _getArgs():
    parser = arg.ArgumentParser(description = "fornece os bounding boxes com as anotacoes e escreve um arquivo .txt com o BB inicial")
    parser.add_argument("caminho", help = "caminho do arquivo de onde sera tirado a informacao inicial do bounding box")
    parser.add_argument("gt", help="nome do arquivo que tem o bounding box inicial")
    parser.add_argument("--escrita","-e", help="Nome da subpasta de escrita", default = 'inicial' )
    return parser.parse_args()

#arquivo para gerar a BB a partir de uma lista
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
    pontos.append(str(min(width)-1))
    pontos.append(str(min(height)-1))
    pontos.append(str(max(width)-1))
    pontos.append(str(max(height)-1))
    bBIncicial = []
    virgula = ","
    bBIncicial = virgula.join(pontos)
    return bBIncicial
    

def _escrever(caminho,gt,lista,escrita):
    file = open(os.path.join(os.path.join(caminho,escrita),gt)+'.txt',"w")
    file.writelines(lista+'\n')
    file.close()

def _ler(arquivo):
    file = open(arquivo,"r")
    k = file.read().split('\n')
    file.close()
    return k

#passa o caminho e o nome do arquivo GT (ground truth)
def _main(caminho,gt,escrita):
    _escrever(caminho,gt,gerarBBInicial(_ler(caminho+'/'+gt)),escrita)

if __name__ == '__main__' :
    args = _getArgs()
    _main(args.caminho,args.gt,args.escrita)
