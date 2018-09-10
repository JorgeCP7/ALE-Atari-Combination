#include "VideoController.hpp"

VideoController::VideoController(OSystem* osystem, StellaEnvironment* env):
  es(osystem->p_export_screen),
  m_env(env),
  screen_size(210 * 160),
  counter(0),
  command("python out/images/image_generator.py out/images/" + osystem->settings().getString("game", true) + "/max_sim_steps_per_frame=" + to_string(osystem->settings().getInt("max_sim_steps_per_frame", true)) + "/ratio=" + to_string(osystem->settings().getFloat("ratio", true)))
  {}
   

void VideoController::writeImage() {
  ALEScreen *s = new ALEScreen(m_env->getScreen());
  pixel_t* imgdata = s->getArray(); //unsigned char*
  std::ofstream file ("out/images/image-data.txt");

	if (file.is_open()) {
	  for (int i = 0; i < screen_size; i++) {
    	int r, g, b;
      es->get_rgb_from_palette(imgdata[i], r, g, b);
	    file << r << "\n";
	    file << g << "\n";
	    if(i != screen_size - 1) {
		    file << b << "\n";
		  }
	    else {
	      file << b;
	    }
	  }
  }
  
  file.close();

}


void VideoController::callImageGenerator() {
  
  writeImage();
  
  string command2 = command + " " + to_string(counter);
  
  //std::cout << command2 << std::endl;
  
  char * call = new char [command2.length()+1];
  strcpy (call, command2.c_str());
  
  system(call);
  
  counter++;

}

  

