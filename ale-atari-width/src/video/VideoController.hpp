#ifndef __VIDEO_CONTROLLER_HPP__
#define __VIDEO_CONTROLLER_HPP__

#include "../emucore/OSystem.hxx"
#include "../emucore/m6502/src/System.hxx"
#include "../environment/stella_environment.hpp"


class VideoController {
  
  public:
  
    //Metodos
    VideoController(OSystem* osystem, StellaEnvironment* env);
    virtual ~VideoController() {}
    void writeImage();
    void callImageGenerator();
    
    //Atributos
    ExportScreen* es;
    int screen_size;
    StellaEnvironment* m_env;
    string command;
    int counter;

};


#endif // __VIDEO_CONTROLLER_HPP__
