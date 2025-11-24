# TP_BUS_RESEAU_LEMA_VOZZ
Voici le compte-rendu de nos TP en bus et réseau de Lemarignier et Vozzola
### **TP 1 - Bus I2C**

Objectif: Interfacer un STM32 avec des capteurs I2C

À partir de la datasheet du [BMP280](https://moodle.ensea.fr/mod/resource/view.php?id=19907), identifiez les éléments suivants:

1. les adresses I²C possibles pour ce composant : Page 28 de la datasheet : On a 7 bits d'adresses dont 6 fixées 111011x. Le dernier bit correspond à la connexion avec SDO. Sur la carte on relève 0x77  donc le SDO est connecté au VDDIO et donc le dernier bit est à 1 donc on a 1110111.
   
   
2. le registre et la valeur permettant d'identifier ce composant

Comme on peut le voir grâce à la BIT MAP, l'identification se trouve dans le registre "id" visible à l'adresse 0xD0
4. le registre et la valeur permettant de placer le composant en mode normal
5. les registres contenant l'étalonnage du composant
6. les registres contenant la température (ainsi que le format)
7. les registres contenant la pression (ainsi que le format)
8. les fonctions permettant le calcul de la température et de la pression compensées, en format entier 32 bits.
