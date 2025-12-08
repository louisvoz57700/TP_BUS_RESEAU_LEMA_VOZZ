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
8. les fonctions permettant le calcul de la température et de la pression compensées, en format entier 32 bits. :
Les fonctions sont données page 22 :
 <img width="1184" height="718" alt="image" src="https://github.com/user-attachments/assets/d74e283e-354a-47ce-916a-c790aa5e2566" />

**2.2. Setup du STM32**
```c
/* USER CODE BEGIN 0 */
int __io_putchar(int chr)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)&chr, 1, HAL_MAX_DELAY);
	return chr;
}
/* USER CODE END 0 */


  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  printf("Bonjour Antonio y Louis\r\n");
  /* USER CODE END 2 */
```
<img width="458" height="140" alt="image" src="https://github.com/user-attachments/assets/927dc102-edb2-4256-abe4-cf5be4d48baa" />

**2.3. Communication I²C**
Communication avec le BMP280
Identification du BMP280

L'identification du BMP280 consiste en la lecture du registre ID

En I²C, la lecture se déroule de la manière suivante:

envoyer l'adresse du registre ID
recevoir 1 octet correspondant au contenu du registre :
```c
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t BMP280_ADDR = 0x77 << 1;         // Adresse I2C shiftée
uint8_t BMP280_REG_ID = 0xD0;           // Adresse du registre ID
/* USER CODE END PD */

  // Id identification

  uint8_t id = 0;
  HAL_StatusTypeDef status;

  status = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &BMP280_REG_ID, 1, HAL_MAX_DELAY);

  if (status == HAL_OK) {
      status = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, &id, 1, HAL_MAX_DELAY);
  }

  if (status == HAL_OK) {
      printf("BMP280 ID = 0x%02X\r\n", id);
  } else {
      printf("Erreur I2C lors de la lecture du registre ID\r\n");
  }

```
<img width="454" height="144" alt="image" src="https://github.com/user-attachments/assets/10df4822-dcc1-49a7-aa4f-edeee5d9ce29" />

Vérifiez que le contenu du registre correspond bien à la datasheet.
Oui on le voit bien avec la memory map du dessus. C'est bien 0x58.
Vérifiez à l'oscilloscope que la formes des trames I²C est conforme.

**Configuration du BMP280**

**TP2**


**Premier démarrage**
ssh lemvoz@192.168.4.213
mdp :lemvoz

3.2. Port Série
Loopback
photo du loopback

**4. TP3 - Interface REST**
on modifie les fichiers config.txt et cmdline.txt
initialisation : ssh antonio@192.168.4.213
mdp :antonio

<img width="2266" height="344" alt="image" src="https://github.com/user-attachments/assets/1c351c7b-d574-4291-bd83-e82dc84bc267" />


