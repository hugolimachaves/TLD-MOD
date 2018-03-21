FILTER="2 4 6"
WINDOW="9 15 21"
NORM="0 1"
INTERP="0 1 2"
VALID="0 1 2" #NCC MEDIA ALIEN
CONF="0 1" #NCC MEDIA ALIEN

DATASET="06_car 07_motocross 08_volkswagen 09_carchase 10_panda"

A=1;
for D in $DATASET; do
	COUNT=1;
	./geraYML.sh 2 21 dataset/$D/alienMediaSimMax.txt dataset/list/$D.list dataset/$D/init.txt dataset/$D/_gt.txt 2 1 > dataset/$D/alienMediaSimMax.yml
	./tracker dataset/$D/alienMediaSimMax.yml
	let COUNT+=1						
done
