# TP_BUS_RESEAU_LEMA_VOZZ
Voici le compte-rendu de nos TP en bus et réseaux de Lemarignier et Vozzola.

La dernière version du code est ici : https://github.com/louisvoz57700/TP_BUS_RESEAU_LEMA_VOZZ/tree/a2229581cd5c942088ec48e2374f5042c583e567/Software/TP_LOUIS/TP_BUS_DE_RESEAU

## **TP 1 - Bus I2C**

Objectif: Interfacer un STM32 avec des capteurs I2C

À partir de la datasheet du [BMP280](https://moodle.ensea.fr/mod/resource/view.php?id=19907), identifiez les éléments suivants:

### 1.
Les adresses I²C possibles pour ce composant :
Page 28 de la datasheet : On a 7 bits d'adresses dont 6 fixées 111011x. Le dernier bit correspond à la connexion avec SDO. Sur la carte on relève 0x77  donc le SDO est connecté au VDDIO et donc le dernier bit est à 1 donc on a 1110111.
   
   
### 2.

Le registre et la valeur permettant d'identifier ce composant :
Comme on peut le voir grâce à la Memory MAP, l'identification se trouve dans le registre "id" visible à l'adresse 0xD0

### 3. 

le registre et la valeur permettant de placer le composant en mode normal :

On peut activer / skipped la measure de la pression en écrivant dans le registre 0xF4 : osrs_t + osrs_p + mode
Pour avoir une mesure de la température avec un oversampling de *1 , on met osrs_t = 0b001
Pour avoir une mesure de la pression avec un oversampling de *1 , on met osrs_p = 0b001
Pour se mettre en mode normal , on met mode = 0b01

### 4. 
les registres contenant l'étalonnage du composant :
   <img width="1208" height="474" alt="image" src="https://github.com/user-attachments/assets/7e267f68-d0de-48b2-94b9-70948f23ce14" />
   On lit l'adresse de calibration de 0xA1 à 0x88.

### 5.
les registres contenant la température (ainsi que le format) :
 <img width="1098" height="460" alt="image" src="https://github.com/user-attachments/assets/5d940431-123b-497b-a349-7d6bad3d244e" />
 On relève 0xFA et oxFB

### 6.

les registres contenant la pression (ainsi que le format) :
De même que pour la température, pour la pression il faudra lire les registres 0XF8 et 0xF7
<img width="1110" height="494" alt="image" src="https://github.com/user-attachments/assets/ebcfc410-13e2-41a5-ab1f-3b527d708f36" />

### 7.

les fonctions permettant le calcul de la température et de la pression compensées, en format entier 32 bits. :
Les fonctions sont données page 22 :
 <img width="1184" height="718" alt="image" src="https://github.com/user-attachments/assets/d74e283e-354a-47ce-916a-c790aa5e2566" />

### 2.2. Setup du STM32

On va juste chercher à print nos prénoms, on redéfinit putchar avec le bon huart ici, c'est le 2.
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

### 2.3. Communication I²C
Communication avec le BMP280
Identification du BMP280

L'identification du BMP280 consiste en la lecture du registre ID
En I²C, la lecture se déroule de la manière suivante:
envoyer l'adresse du registre ID :

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

Vérifiez que le contenu du registre correspond bien à la datasheet:

Oui on le voit bien avec la memory map du dessus. C'est bien 0x58.

**Configuration du BMP280**

La configuration se retrouve ici : https://github.com/louisvoz57700/TP_BUS_RESEAU_LEMA_VOZZ/blob/643206734aac2e1833452727166aa53dd605094a/Software/TP4_CAN/tp_bus_F446RE/Core/Src/BMP280.c

Globalement, on initialise BMP280 et on lit les valeurs dans le while(1).
```c
BMP280_Init(&hi2c1);
while (1){
BMP280_Read_Raw(&hi2c1, &raw_p, &raw_t);

// 2. Compensate (Temp FIRST)
comp_t = bmp280_compensate_T_int32(raw_t);
comp_p = bmp280_compensate_P_int64(raw_p);

// 3. Print (Raw & Readable)
//datasheet page 22, division par 100 pour la température et 25600 pour pression
printf("RAW: T=%ld P=%ld | FINAL: T=%.2f C, P=%.2f hPa\r\n",raw_t, raw_p, comp_t / 100.0f, comp_p / 25600.0f);
}
```


**TP2**

**3.1. Mise en route du Raspberry PI Zéro**

**Premier démarrage**

(Notes à nous mêmes ) 
ssh lemvoz@192.168.4.213
mdp :lemvoz

Raspberry configurée.
Pour récupérer son adresse, il faut bien sûr se connecter au même wifi et avec un logiciel comme angry ip scanner, on peut regarder les IP sur le réseau.
L'IP a été configurée de cette manière : 192.168.4 est l'identifiant du réseau et .213 c'est l'identifiant unique de notre raspberry.


### 3.2. Port Série
**Loopback**
photo du loopback : 
<img width="1146" height="384" alt="image" src="https://github.com/user-attachments/assets/e859f66c-bac8-4323-a354-bc1766bc4d58" />


**Communication avec la STM32 :**

Pour la réception sur la rasperry pi, on crée une fonction parse et un buffer qui traîte la requête suivant la demande de la raspberry.
```c
void PARSE(uint8_t* buffer, uint8_t size)
{
	for (int i = 0; i < size; i++)
	{
		if (buffer[i] == 'G' && buffer[i + 1] == 'E' && buffer[i + 2] == 'T' && buffer[i + 3] == '_')
		{
			if (buffer[i + 4] == 'T')
			{
				char ligne[30];
				int len = snprintf(ligne, sizeof(ligne), "Valeur de T:%.2f \r\n", temp1);
				HAL_UART_Transmit(&huart1, (uint8_t*)ligne, len, 10);

			}
			if (buffer[i + 4] == 'P')
			{
				char ligne[30];
				int len = snprintf(ligne, sizeof(ligne), "Valeur de P:%.2f \r\n", pres1);
				HAL_UART_Transmit(&huart1, (uint8_t*)ligne, len, 10);
			}
		}
	}
}
```
Petite photo des valeurs que nous obtenons, ici en brut mais pour les prochains TP, les valeurs compensées :
<img width="1511" height="714" alt="image" src="https://github.com/user-attachments/assets/817c3f6b-31f5-4b93-8f6a-1bb69971f663" />

### TP3 - Interface REST

**Premier fichier Web**

on modifie les fichiers config.txt et cmdline.txt

initialisation : ssh antonio@192.168.4.213
mdp :antonio

Créons notre nouveau serveur web : 
sur un terminal :
<img width="2266" height="344" alt="image" src="https://github.com/user-attachments/assets/1c351c7b-d574-4291-bd83-e82dc84bc267" />
Sur le deuxième terminal :
<img width="1019" height="124" alt="image" src="https://github.com/user-attachments/assets/0ca12703-8fc6-459c-b5cd-4f47c698e6d9" />
en ajoutant les options : 
<img width="1188" height="377" alt="image" src="https://github.com/user-attachments/assets/17ecbff1-3a14-4bde-afda-face8821cb97" />
On le voit bien aussi sur le navigateur web : 
<img width="1390" height="405" alt="image" src="https://github.com/user-attachments/assets/d4d96e7c-3a85-4c6b-ae0f-34c3467fccd3" />

**Première route**

Le rôle de @app.route est de connecter une adresse URL à cette fonction Python. Il crée une route

L'expression <int:index> :
Elle dit "Tout ce qui est écrit à cet endroit précis de l'URL, mets-le dans une variable nommée index" et "Transforme ce texte en un nombre entier (integer)". Si l'utilisateur tape "5", Python reçoit le nombre 5 (mathématique), pas le texte "5".
Si l'utilisateur tape quelque chose qui n'est pas un nombre (par exemple /api/welcome/toto), Flask refusera la connexion (Erreur 404) avant même de lancer notre fonction.

### 4.2. Première page REST
On aperçoit bien le welcome et le retour avec l'index :

<img width="578" height="66" alt="Screenshot 2025-12-08 at 3 02 59 PM" src="https://github.com/user-attachments/assets/8bdecfd9-d343-43bc-9755-3e3ca4dbd948" />

Comme on peut le voir, nos requêtes sont lues en html mais pas en JSON :

<img width="1470" height="658" alt="image" src="https://github.com/user-attachments/assets/7d2ba4cd-0190-4c86-a455-c70ccde5778e" />


**Première route**

Après l'ajout des lignes ... on obtient :

<img width="1470" height="680" alt="image" src="https://github.com/user-attachments/assets/f39414a3-ba9d-4c20-ad4a-2e7f4e76a3dc" />

**Deuxième route** 

On utilise ici jsonify et on voit que c'est bien détecter en JSON :

<img width="1469" height="613" alt="image" src="https://github.com/user-attachments/assets/0120a5f3-5528-43d7-8e5d-b7d91b0ebbb7" />

**On implémente maintenant la page "error 404"**

<img width="1470" height="702" alt="image" src="https://github.com/user-attachments/assets/85c13df9-d6fc-4dff-9e77-f74465df2aa2" />

Not allowed pour le moment

<img width="574" height="108" alt="image" src="https://github.com/user-attachments/assets/4f1fa88c-93f4-427f-bab8-af7fb20d771e" />

**Méthode POST:**
<img width="1370" height="333" alt="image" src="https://github.com/user-attachments/assets/4dc9677c-6408-4483-a57e-05179595efa7" />

**Méthode GET :**

<img width="1470" height="643" alt="image" src="https://github.com/user-attachments/assets/1a4d72f9-f249-41c8-9f06-6ec5b067270a" />

**Suite méthode POST :**
<img width="1370" height="994" alt="image" src="https://github.com/user-attachments/assets/30bc8888-6075-4b15-97a6-55dc47cba05a" />

**méthode :API CRUD**

<img width="1370" height="852" alt="image" src="https://github.com/user-attachments/assets/61e4dfec-3552-4827-a9a9-3d0c1e9b9433" />



On peut tester nos méthodes HTTP :

<img width="574" height="105" alt="Screenshot 2025-12-15 at 2 12 03 PM" src="https://github.com/user-attachments/assets/0fb189ab-67ba-4008-8dbf-68dc2590190a" />
<img width="554" height="104" alt="Screenshot 2025-12-15 at 2 10 12 PM" src="https://github.com/user-attachments/assets/e20b083e-289f-45c5-bead-06b65747a73e" />



##  TP4 - Bus CAN
### 5.1. Pilotage du moteur

Pour avoir 500kbit/s pour la vitesse CAN, on règle en conséquence dans l'IOC dans le CAN1 les valeurs du time quantum: 

<img width="810" height="286" alt="image" src="https://github.com/user-attachments/assets/1ed69093-f502-436e-8cfc-6e36bb2a40de" />

Commencez par mettre en place un code simple, qui fait bouger le moteur de 90° dans un sens, puis de 90° dans l'autre, avec une période de 1 seconde :
On utilisera cette datasheet : https://github.com/louisvoz57700/TP_BUS_RESEAU_LEMA_VOZZ/blob/1fa86e8d7999bb4583587c6866a9c4e86809af35/datasheet/moteur%20(1).pdf

```c
	HAL_CAN_Start(&hcan1);

	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;
	uint8_t TxData[8];


	TxHeader.StdId = 0x60;
	TxHeader.ExtId = 0;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 3;
	TxHeader.TransmitGlobalTime = DISABLE;

	dans le while : 
	TxData[0] = 0x00;
		TxData[1] = 0xB4;
		TxData[2] = 0x01;
		HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);

		HAL_Delay(1000);

		TxData[0] = 0x01;
		TxData[1] = 0xB4;
		TxData[2] = 0x01;

		HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
		HAL_Delay(1000);
```
On fera attention à se mettre en bout de ligne et prendre 120 ohms sinon on aura des problèmes de réflexion.

**5.2. Interfaçage avec le capteur**
On reprend le code qu'on avait fait pour lire les valeurs de la température et on crée une fonction  process_temperature qui prend en argument la température lut. On garde en mémoire la denière position et on la compare avec la nouvelle position. Cette différence sera appelé step, si step est positif on tourne dans un sens, sinon dans l'autre et le nombre de step, dépend de la différence de température, on se basera sur le graphique suivant : 
![WhatsApp Image 2025-12-18 à 14 08 34_1a0db8f0](https://github.com/user-attachments/assets/55a3fcd5-7c1b-486f-a970-91e356fc3e10)

Voici une vidéo du fonctionnement :
https://github.com/user-attachments/assets/54dca179-4a62-4eaa-8a0a-a004caa852ee


## 6. TP5 - Rassemblement
On regroupe l'ensemble de nos codes.
En s'aidant d'une IA, on arrive à créer une page html qui va pouvoir intérroger le STM32. Le stm32 va répondre avec la température/pression selon la demande
et on va voir la valeur sur notre page web.
Lorsqu'on appuie sur le bouton "Get Temperature", on va envoyer au STM32 GET_T et il va répondre avec la température actuelle :

[Voir la page](Software/code_API_rest.html)

<img width="1705" height="849" alt="image" src="https://github.com/user-attachments/assets/640c5225-2e94-4002-89fb-e3b9c2156801" />


