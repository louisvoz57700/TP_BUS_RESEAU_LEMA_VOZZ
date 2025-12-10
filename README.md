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
sur un terminal

<img width="2266" height="344" alt="image" src="https://github.com/user-attachments/assets/1c351c7b-d574-4291-bd83-e82dc84bc267" />
Sur le deuxième terminal :
<img width="1019" height="124" alt="image" src="https://github.com/user-attachments/assets/0ca12703-8fc6-459c-b5cd-4f47c698e6d9" />
en ajoutant les options : 
<img width="1188" height="377" alt="image" src="https://github.com/user-attachments/assets/17ecbff1-3a14-4bde-afda-face8821cb97" />
On le voit bien aussi sur le navigateur web : 
<img width="1390" height="405" alt="image" src="https://github.com/user-attachments/assets/d4d96e7c-3a85-4c6b-ae0f-34c3467fccd3" />

Le rôle de @app.route est de connecter une adresse URL à cette fonction Python. Il crée une route

L'expression <int:index> fait trois choses en même temps :
Capture : Elle dit "Tout ce qui est écrit à cet endroit précis de l'URL, mets-le dans une variable nommée index".
Conversion (int) : Elle dit "Transforme ce texte en un nombre entier (integer)". Si l'utilisateur tape "5", Python reçoit le nombre 5 (mathématique), pas le texte "5".
Filtrage (Sécurité) : C'est le plus important. Si l'utilisateur tape quelque chose qui n'est pas un nombre (par exemple /api/welcome/toto), Flask refusera la connexion (Erreur 404) avant même de lancer votre fonction. Cela protège votre code contre des erreurs.
4. TP3 - Interface REST
4.2. Première page REST
On aperçcoit bien le welcom et le retour avec l'index :

<img width="578" height="66" alt="Screenshot 2025-12-08 at 3 02 59 PM" src="https://github.com/user-attachments/assets/8bdecfd9-d343-43bc-9755-3e3ca4dbd948" />

Comme on peut le voir, nos requêtes sont lu en html mais pas en JSON :

<img width="1470" height="658" alt="image" src="https://github.com/user-attachments/assets/7d2ba4cd-0190-4c86-a455-c70ccde5778e" />


Première route

Après l'ajout des lignes ... on obtient :

<img width="1470" height="680" alt="image" src="https://github.com/user-attachments/assets/f39414a3-ba9d-4c20-ad4a-2e7f4e76a3dc" />

Deuxième route 

On utilise ici jsonify et on voit que c'est bien détecter en JSON :

<img width="1469" height="613" alt="image" src="https://github.com/user-attachments/assets/0120a5f3-5528-43d7-8e5d-b7d91b0ebbb7" />

On implémente maintenant la page "error 404"

<img width="1470" height="702" alt="image" src="https://github.com/user-attachments/assets/85c13df9-d6fc-4dff-9e77-f74465df2aa2" />

Not allowed pour le moment

<img width="574" height="108" alt="image" src="https://github.com/user-attachments/assets/4f1fa88c-93f4-427f-bab8-af7fb20d771e" />

Méthode POST:
<img width="1370" height="333" alt="image" src="https://github.com/user-attachments/assets/4dc9677c-6408-4483-a57e-05179595efa7" />

Méthode GET :

<img width="1470" height="643" alt="image" src="https://github.com/user-attachments/assets/1a4d72f9-f249-41c8-9f06-6ec5b067270a" />

Suite méthode POST :
<img width="1370" height="994" alt="image" src="https://github.com/user-attachments/assets/30bc8888-6075-4b15-97a6-55dc47cba05a" />

méthode :API CRUD

<img width="1370" height="852" alt="image" src="https://github.com/user-attachments/assets/61e4dfec-3552-4827-a9a9-3d0c1e9b9433" />
'''py
antonio@LEMVOZ:~/serveur $ cat hello.py 
from flask import Flask, jsonify, render_template, abort, request

app = Flask(__name__)
welcome = "Welcome to 3ESE API and antonio y louis!"

@app.route('/api/request/', methods=['GET', 'POST'])
@app.route('/api/request/<path>', methods=['GET','POST'])
def api_request(path=None):
    resp = {
            "method":   request.method,
            "url" :  request.url,
            "path" : path,
            "args": request.args,
            "headers": dict(request.headers),
    }
    if request.method == 'POST':
        resp["POST"] = {
                "data" : request.get_json(),
                }
    return jsonify(resp)

@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404

@app.route('/api/welcome/', methods=['GET', 'POST', 'DELETE'])
def api_welcome_global():
    global welcome
    
    if request.method == 'GET':
        return jsonify({"val": welcome})
    
    elif request.method == 'POST':
        data = request.get_json()
        if data and 'val' in data:
            welcome = data['val']
            return jsonify({"val": welcome}), 200
        else:
            return jsonify({"error": "JSON invalide"}), 400

    elif request.method == 'DELETE':
        welcome = ""
        return "", 204

@app.route('/api/welcome/<int:index>', methods=['GET', 'PUT', 'PATCH', 'DELETE'])
def api_welcome_index(index):
    global welcome
    
    if index >= len(welcome) and request.method != 'PUT':
        abort(404)

    if request.method == 'GET':
        return jsonify({"index": index, "val": welcome[index]})

    elif request.method == 'PUT':
        data = request.get_json()
        word = data.get('val', '')
        welcome = welcome[:index] + word + welcome[index:]
        return jsonify({"val": welcome})

    elif request.method == 'PATCH':
        data = request.get_json()
        char = data.get('val', '')
        if len(char) != 1:
             return jsonify({"error": "Erreur longueur"}), 400
        
        welcome = welcome[:index] + char + welcome[index+1:]
        return jsonify({"val": welcome})

    elif request.method == 'DELETE':
        welcome = welcome[:index] + welcome[index+1:]
        return "", 204
antonio@LEMVOZ:~/serveur $

5. TP4 - Bus CAN
5.1. Pilotage du moteur

Pour avoir 500kbit/s : 
<img width="810" height="286" alt="image" src="https://github.com/user-attachments/assets/1ed69093-f502-436e-8cfc-6e36bb2a40de" />

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

   On fera attention à se mettre en bout de ligne et prendre 120 ohms sinon on aura des problèmes de réflexion
