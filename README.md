# TP_BUS_RESEAU_LEMA_VOZZ
Voici le compte-rendu de nos TP en bus et réseau de Lemarignier et Vozzola
### **TP 1 - Bus I2C**

Objectif: Interfacer un STM32 avec des capteurs I2C

À partir de la datasheet du [BMP280](https://moodle.ensea.fr/mod/resource/view.php?id=19907), identifiez les éléments suivants:

1. les adresses I²C possibles pour ce composant : Page 28 de la datasheet : On a 7 bits d'adresses dont 6 fixées 111011x. Le dernier bit correspond à la connexion avec SDO. Sur la carte on relève 0x77  donc le SDO est connecté au VDDIO et donc le dernier bit est à 1 donc on a 1110111.
   
   
2. le registre et la valeur permettant d'identifier ce composant :
Comme on peut le voir grâce à la Memory MAP, l'identification se trouve dans le registre "id" visible à l'adresse 0xD0
3. le registre et la valeur permettant de placer le composant en mode normal

On peut activer / skipped la measure de la pression en écrivant dans le registre 0xF4 : osrs_t + osrs_p + mode
Pour avoir une mesure de la température avec un oversampling de *1 , on met osrs_t = 0b001
Pour avoir une mesure de la pression avec un oversampling de *1 , on met osrs_p = 0b001
Pour se mettre en mode normal , on met mode = 0b01

4. les registres contenant l'étalonnage du composant :
   <img width="1208" height="474" alt="image" src="https://github.com/user-attachments/assets/7e267f68-d0de-48b2-94b9-70948f23ce14" />
   On lit l'adresse de calibration de 0xA1 à 0x88.

5. les registres contenant la température (ainsi que le format)
 <img width="1098" height="460" alt="image" src="https://github.com/user-attachments/assets/5d940431-123b-497b-a349-7d6bad3d244e" />
 On relève 0xFA et oxFB

7. les registres contenant la pression (ainsi que le format)
De même que pour la température, pour la pression il faudra lire les registres 0XF8 et 0xF7
8. les fonctions permettant le calcul de la température et de la pression compensées, en format entier 32 bits.
