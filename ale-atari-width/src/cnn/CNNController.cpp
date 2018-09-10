#include "CNNController.hpp"


/*
  Inicializa el controlador de CNN en ALE
  - Inicializacion de atributos
  - Segun el juego pasado como parametro, se establecen el orden las acciones (actions_order)
*/
CNNController::CNNController(OSystem* osystem, StellaEnvironment* env):
  es(osystem->p_export_screen),
  m_env(env),
  action_repeat(osystem->settings().getInt("cnn_action_repeat", true)),
  num_input(osystem->settings().getInt("cnn_num_input", true)),
  buffer_size(osystem->settings().getInt("cnn_action_repeat", true) * osystem->settings().getInt("cnn_num_input", true)),
  screen_size(210 * 160),
  command("python ../dqn-tensorflow/main.py --env_name=" + osystem->settings().getString("game", true) + " --mode=ale --use_gpu=False"),
  cnn_ratio(osystem->settings().getFloat("ratio", true))
  {
    string game_name = osystem->settings().getString("game", true);
    if(game_name == "Breakout-v0" || game_name == "SpaceInvaders-v0") {  //Breakout & Space Invaders
      actions_order.push_back(0);
      actions_order.push_back(1);
      actions_order.push_back(3);
      actions_order.push_back(4);
      actions_order.push_back(11);
      actions_order.push_back(12);
    }
    else if(game_name == "BeamRider-v0") {  //Beam Rider
      actions_order.push_back(0);
      actions_order.push_back(1);
      actions_order.push_back(2);
      actions_order.push_back(3);
      actions_order.push_back(4);
      actions_order.push_back(6);
      actions_order.push_back(7);
      actions_order.push_back(11);
      actions_order.push_back(12);
    }
    else if(game_name == "Frostbite-v0" || game_name == "StarGunner-v0") {  //Frostbite & Star Gunner
      for(int i = 0; i < 18; i++) {
        actions_order.push_back(i);
      }
    }
    else if(game_name == "Qbert-v0") {  //Q* Bert
      actions_order.push_back(0);
      actions_order.push_back(1);
      actions_order.push_back(2);
      actions_order.push_back(3);
      actions_order.push_back(4);
      actions_order.push_back(5);      
    }
  }

   
/*
  Inserta un screen al buffer de imagenes (screen_buffer)
*/
void CNNController::insertScreen() {
  //std::cout << "-- INSERT" << std::endl;
  ALEScreen *s = new ALEScreen(m_env->getScreen());
  pixel_t* x = s->getArray(); //Obtiene el array de pixeles 210*160 (no RGB)

  screen_buffer.push_back(x); //Insertar al final
  //std::cout << "-- BUFFER_SIZE: " << buffer_size << std::endl;
  //std::cout << "-- ACTION_REPEAT: " << action_repeat << std::endl;
  //std::cout << "-- NUM_IMPUT: " << num_input << std::endl;
  //std::cout << "-- SCREEN_BUFFER SIZE: " << screen_buffer.size() << std::endl;
  //Si el buffer esta completo se eliminan las screen mas antiguas
  if(screen_buffer.size() > buffer_size) {
    screen_buffer.erase(screen_buffer.begin()); //para eliminar el primer elemento del vector
  }
  
  //std::cout << "-- SCREEN_BUFFER SIZE: " << screen_buffer.size() << std::endl;
}


/*
  Escribe el estado ("num_input" screens del buffer) en un fichero de salida
*/
void CNNController::writeState() {
	std::ofstream file ("../dqn-tensorflow/cnn-input.txt");
	
	if (file.is_open()) {
  
    //PARTE 1
    std::vector<pixel_t*> state_buffer;
    //std::cout << "-- SCREEN_BUFFER SIZE: " << screen_buffer.size() << std::endl;
    for (int k = screen_buffer.size() - 1; k >= 0; k = k - action_repeat) {
      state_buffer.insert(state_buffer.begin(), screen_buffer.at(k));
      //std::cout << "-- ENTRA AL INICIO: " << k << std::endl;
    }
    
    //PARTE 2
    while (state_buffer.size() < num_input) {
      state_buffer.insert(state_buffer.begin(), state_buffer.at(0));
      //std::cout << "-- RELLENAR CON 0" << std::endl;
    }
    
    //PARTE 3
	  for (int j = 0; j < num_input; j++) {
      //std::cout << "-- ESCRITO: " << j << std::endl;
      
      //Transformacion a RGB y escritura de una imagen completa 
	    for (int i = 0; i < screen_size; i++) {
      	int r, g, b;  //Componentes RGB
	      es->get_rgb_from_palette(state_buffer.at(j)[i], r, g, b);
	      //Escritura de componentes RGB del pixel en el fichero de salida
		    file << r << "\n";
		    file << g << "\n";
		    if (i == screen_size - 1 && j == num_input - 1) {  //Ultimo pixel
		      file << b;
		    }
		    else {
			    file << b << "\n";
		    }
      }
    }
    
	  file.close();

  }

}


/*
  Llama a la CNN para que genere una salida a partir del estado contenido 
  en el fichero de salida
*/
void CNNController::callCNN() {

  //----ESCRITURA FICHERO----
  
  writeState();


  //----EJECUTAR CNN----
  
  char * call = new char [command.length()+1];
  strcpy (call, command.c_str());

  system(call); //genera un fichero con la salida de la CNN
	
	
	//Leer la salida
  std::ifstream file ("../dqn-tensorflow/cnn-output.txt"); 
    
  std::string linea;
  std::vector<float> lista;
  
  if (file.is_open()) {
    while (getline(file,linea)) {
      lista.push_back(std::stod(linea));
    }
    file.close();
  }


  q_values = lista;
	
}



/*
  Dada una lista de valores numericos, devuelve la misma lista con los valores normalizados [0,1]
*/
void CNNController::normalize() {

  //minimo iw
  float minimo = INFINITY;
  for(int i = 0; i < q_values.size(); i++) {
    if(minimo > q_values[i]) {
      minimo = q_values[i];
    }
  }
  
  //resta iw
  for(int i = 0; i < q_values.size(); i++) {
    q_values[i] = q_values[i] - minimo;
  }  
  
  //maximo iw
  float maximo = -INFINITY;
  for(int i = 0; i < q_values.size(); i++) {
    if(maximo < q_values[i]) {
      maximo = q_values[i];
    }
  }
    
  //normalizacion iw
  for(int i = 0; i < q_values.size(); i++) {
    if(maximo != 0.0) {
      q_values[i] = q_values[i] / maximo;
    }
    else if(maximo == 0.0 && minimo > 0.0) {  //si el reward minimo es positivo, todas las acciones son aptas, por lo que se normalizan como reward 1.0
      q_values[i] = 1.0;
    }
    else if(maximo == 0.0 && minimo <= 0.0) { //si el minimo es negativo o 0.0, ninguna accion es apta, por lo que todas las acciones se normalizan como reward 0.0. Caso hipotetico poco probable
      q_values[i] = 0.0;
    }
  }

}



/*
  Dada una lista de refuerzos y una lista de orden de acciones, devuelve 
  la accion de mayor refuerzo
*/
Action CNNController::get_best_action() {
  
  float maxFinal = -INFINITY;
  vector<int> maximos;
  for(int i = 0; i < q_values.size(); i++) {
    if(maxFinal < q_values[i] ) {
      maximos.clear();  //borra todas las acciones de la lista que tenian el mayor refuerzo hasta el momento
      maxFinal = q_values[i];
      maximos.push_back(actions_order[i]);   //añade la accion de mayor refuerzo
    }
    else if(maxFinal == q_values[i]) {
      maximos.push_back(actions_order[i]);
    }
  }
  
  return (Action) maximos[rand() % (maximos.size())];
  
}
  

