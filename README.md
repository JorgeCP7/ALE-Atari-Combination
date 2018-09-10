# ALE-Atari-Combination

Implementación de un nuevo agente basado en técnicas de Inteligencia Artificial para la resolución de videojuegos de Atari.


## Introducción

El sistema implementa un agente denominado *CombinationAgent* que combina dos enfoques para resolver el dominio de los videojuegos de Atari: *reinforcement learning* y *planning*. El agente funciona de la siguiente forma: en cada estado de la partida, el agente llama al algoritmo de planificación elegido y a una red convolucional entrenada con DQN para que determinen por separado cuál sería la mejor acción. Las decisiones de ambas técnicas se combinan para ofrecer un acción definitiva que el agente pueda ejecutar.

Para implementar el agente, se ha tomado como base el código de dos repositorios de *Github*:

- `Ale-Atari-Width`[1] implementa un nuevo algoritmo de planificación denominado IW dentro del framework *Atari Learning Environment* (ALE) [2]. El código fue construido y utilizado por los autores del artículo *Classical Planning with Simulators: Results on the Atari Video Games* [3] para estudiar el algoritmo IW en el ámbito de los videojuegos de Atari.

- `DQN-tensorflow`[4] implementa el algoritmo DQN sobre las librerías *Gym* y *Tensorflow*. Es fiel a las especificaciones del equipo de *DeepMind* en su artículo *Human-level control through deep reinforcement learning* [5], en el que evalúan el rendimiento de DQN en el dominio de los videojuegos de Atari.

## Instalación

El sistema puede instalarse a partir de lo especificado en los repositorios `Ale-Atari-Width` [1] y `DQN-tensorflow` [4] de Github. Sin embargo, se resumirá el proceso a continuación.

Para instalar el subsistema `Ale-Atari-Width` se ejecutadan los siguientes comandos en la consola de Ubuntu:
    
    $ sudo apt-get install cmake
    $ sudo apt-get install libsdl1.2-dev
    $ sudo apt-get install libsdl-gfx1.2-dev
    $ sudo apt-get install libsdl-image1.2-dev 
    
En la carpeta raíz del subsistema `Ale-Atari-Width` se ejecutan los siguientes comandos en la consola de Ubuntu:

    $ make -B

Para instalar el subsistema `DQN-tensorflow` se ejecutadan los siguientes comandos en la consola de Ubuntu:

    $ sudo pip install tensorflow==0.12.0
    $ sudo pip install gym==0.7.0
    $ pip install atari-py==0.0.21
    $ python -m pip install --user numpy scipy matplotlib ipython jupyter pandas sympy nose
    $ sudo pip install Pillow
    $ sudo pip install tqdm

## Ejecución

### Entrenamiento de redes convolucionales

A continuación, se adjunta un ejemplo de comando para entrenar una red convolucional con DQN. El comando debe ejecutarse en la raíz del subsistema `DQN-tensorflow`:

    $ python main.py --env_name=Breakout-v0 --mode=train --display=True --use_gpu=False

Donde `--env_name` es el título de la ROM del videojuego de Atari escogido. Los posibles valores son: `Breakout-v0`, `SpaceInvaders-v0`, `BeamRider-v0`, `Frostbite-v0`, `Qbert-v0`, `StarGunner-v0`, etc.

Los parámetros relacionados con el entrenamiento de la red se ajustan desde el fichero `config.py`.

Para probar la red entrenada, se debe ejecutar el siguiente comando en la raíz del subsistema`DQN-tensorflow`:

    $ python main.py --env_name=Breakout-v0 --mode=test --display=True --use_gpu=False

### Ejecución de *CombinationAgent*

El sistema ya dispone de redes convolucionales entrenadas con DQN para los siguientes videojuegos: *Breakout*, *Space Invaders*, *Beam Rider*, *Frostbite*, *Q\* Bert* y *Star Gunner*. Por ello, el código puede ejecutarse una vez instalado.

A continuación, se adjuntan algunos comandos de ejemplo para utilizar el agente *CombinationAgent*. Los comandos deben ejecutarse en la raíz del subsistema `Ale-Atari-Width`:

    $ ./ale -display_screen true -discount_factor 0.995 -randomize_successor_novelty true -max_sim_steps_per_frame 10000 -max_num_frames 8000 -player_agent combination_agent -search_method iw1 -game Breakout-v0 -ratio 0.25 -mode consensus -images false -restricted_action_set true supported_roms/group_3/breakout.bin
    
    $ ./ale -display_screen true -discount_factor 0.995 -randomize_successor_novelty true -max_sim_steps_per_frame 10000 -max_num_frames 8000 -player_agent combination_agent -search_method iw1 -game Frostbite-v0 -ratio 0.75 -mode probability -images false -restricted_action_set true supported_roms/group_1/frostbite.bin
    
    $ ./ale -display_screen true -discount_factor 0.995 -randomize_successor_novelty true -max_sim_steps_per_frame 10000 -max_num_frames 8000 -player_agent combination_agent -search_method iw1 -game BeamRider-v0 -ratio 0.5 -mode consensus -images false -restricted_action_set true supported_roms/group_3/beam_rider.bin

Los parámetros más relevantes son:

- `-player_agent`: Tipo de agente escogido para resolver la partida. Los posibles valores son: `combination_agent`, `search_agent`, etc.

- `-search_method`: Algoritmo de planificación elegido para resolver la partida. El parámetro es necesario para los agentes `combination_agent` y `search_agent`. Solo se ha experimentado con el valor `iw1`, que se corresponde con el algoritmo de planificación IW.

- `-game`: Título de la ROM del videojuego de Atari escogido. El videojuego elegido en este parámetro debe coincidir con el de la ruta `supported_roms/`. Los posibles valores son: `Breakout-v0`, `SpaceInvaders-v0`, `BeamRider-v0`, `Frostbite-v0`, `Qbert-v0`, `StarGunner-v0`, etc.

- `-mode`: Criterio del agente *CombinationAgent* para elegir la mejor acción, dado un estado de la partida. Los posibles valores son: `consensus`, `probability`.

- `-ratio`: Porcentaje de influencia de la red de DQN en las decisiones del algoritmo de planificación. El parámetro solo es necesario cuando el agente es `combination_agent`.

- `-max_sim_steps_per_frame`: Indica el grado de exhaustividad de la simulación del algoritmo de planificación utilizado. El parámetro es necesario para los agentes `combination_agent` y `search_agent`.

- `-max_num_frames`: Límite de frames de la partida. Cuando se alcanza el límite, la partida termina aunque el agente no haya ganado o perdido.


## Referencias

[1] N. Lipovetzky, M. Ramirez y H. Geffner, «GitHub, repositorio "ALE-Atari-Width",» 3 Diciembre 2015. [En línea]. Available: https://github.com/miquelramirez/ALE-Atari-Width.

[2] M. G. Bellemare, Y. Naddaf, J. Veness y M. Bowling, «Github, repositorio "Arcade-Learning-Environment",» 29 12 2017. [En línea]. Available: https://github.com/mgbellemare/Arcade-Learning-Environment.

[3] N. Lipovetzky, M. Ramírez y H. Geffner, «Classical Planning with Simulators: Results on the Atari Video Games,» de International Joint Conference on Artificial Intelligence (IJCAI), Buenos Aires, Julio 2015. 

[4] devsisters, «GitHub, repositorio "DQN-tensorflow",» 28 Junio 2017. [En línea]. Available: https://github.com/devsisters/DQN-tensorflow.

[5] V. Mnih, K. Kavukcuoglu, D. Silver, A. A. Rusu, J. Veness, M. G. Bellemare, A. Graves, M. Riedmiller, A. K. Fidjeland, G. Ostrovski, S. Petersen, C. Beattie, A. Sadik, I. Antonoglou, H. King, D. Kumaran, D. Wierstra, S. Legg y D. Hassabis, «Human-level control through deep reinforcement learning,» Nature, vol. 518, nº 7540, pp. 529-533, 2015. 

