#ifndef __CNN_CONTROLLER_HPP__
#define __CNN_CONTROLLER_HPP__

#include "../emucore/OSystem.hxx"
#include "../emucore/m6502/src/System.hxx"
#include "../environment/stella_environment.hpp"


class CNNController {
  
  public:
  
    //Metodos
    CNNController(OSystem* osystem, StellaEnvironment* env);
    virtual ~CNNController() {}
    void insertScreen();
    void writeState();
    void callCNN();
    void normalize();
    Action get_best_action();
    
    //Atributos
    std::vector<pixel_t*> screen_buffer;  //Buffer de arrays de pixeles (pixel_t)
    ExportScreen* es;   //Necesario para la transformacion a RGB del array de pixeles
    //ALEScreen *s;
    int screen_size;  //Tamaño de la imagen (160*210)
    int buffer_size;  //Tamaño maximo del buffer
    StellaEnvironment* m_env; //Environment (compartido con el resto de clases de ALE)
    string command;   //String que representa el comando para ejecutar la CNN (python)
    std::vector<int> actions_order; //Lista para representar el orden de las acciones del juego
    int action_repeat; //debe coincidir con el parametro "action_repeat" de DQN (4 normalmente)
    int num_input; //numero de entradas de la red
    
    double cnn_ratio;
    std::vector<float> q_values;
};


#endif // __CNN_CONTROLLER_HPP__
