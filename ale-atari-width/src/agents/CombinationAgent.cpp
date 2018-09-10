/* *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2012 by Yavar Naddaf, Joel Veness, Marc G. Bellemare
 * Released under GNU General Public License www.gnu.org/licenses/gpl-3.0.txt
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 *  CombinationAgent.cpp
 *
 * The implementation of the CombinationAgent class, which uses Search and learning Algorithms
 * to act in the game
 **************************************************************************** */

//mio
#include "CombinationAgent.hpp"
#include "CNNController.hpp"
#include "SearchTree.hpp"

//#include "game_controller.h"
#include "random_tools.h"
#include "Serializer.hxx"
#include "Deserializer.hxx"
#include "System.hxx"
#include <sstream>

#include "BreadthFirstSearch.hpp"
#include "IW1Search.hpp"

#include "UniformCostSearch.hpp"
#include "BestFirstSearch.hpp"

#include "UCTSearchTree.hpp"
#include "time.hxx"


/////////////////////////////////////////////////////////////////////
//#include "CNNController.hpp"
/////////////////////////////////////////////////////////////////////


CombinationAgent::CombinationAgent(OSystem* _osystem, RomSettings* _settings, StellaEnvironment* _env, bool player_B) : 
    PlayerAgent(_osystem, _settings),
  m_curr_action(UNDEFINED), m_current_episode(0)
{	
	
	
	/////////////////////////////////////////////////////////////////////////////
	
	video = new VideoController(_osystem, _env);
	cnn = new CNNController(_osystem, _env);  //inicializacion del controlador de la red DQN
	
	/////////////////////////////////////////////////////////////////////////////
	
	search_method = p_osystem->settings().getString("search_method", true); 

	if(player_B){
		available_actions =  _settings->getAllActions_B() ;
		search_method = p_osystem->settings().getString("search_method_B", false); 
	}

	// Depending on the configuration, create a SearchTree of the requested type
	if (search_method == "brfs") {
		search_tree = new BreadthFirstSearch(	_settings, _osystem->settings(),
					 		available_actions, _env);
		m_trace.open( "brfs.search-agent.trace" );
	} else if ( search_method == "ucs" ) {
		search_tree = new UniformCostSearch(	_settings, _osystem->settings(),
							available_actions, _env);
	
		m_trace.open( "ucs.search-agent.trace" );
			
	}else if( search_method == "iw1"){
		search_tree = new IW1Search(	_settings, _osystem->settings(),
						available_actions, _env);
	
		search_tree->set_novelty_pruning();
		m_trace.open( "iw1.search-agent.trace" );
    
	}else if( search_method == "bfs"){
		search_tree = new BestFirstSearch(	_settings, _osystem->settings(),
						available_actions, _env);
	
		search_tree->set_novelty_pruning();
		m_trace.open( "bfs.search-agent.trace" );
    
	}  else if (search_method == "uct") {
		search_tree = new UCTSearchTree(_settings, _osystem->settings(),
					    available_actions, _env);
		m_trace.open( "uct.search-agent.trace" );
	} else {
		cerr << "Unknown search Method: " << search_method << endl;
		exit(-1);
	}


	m_rom_settings = _settings;
	m_env = _env;    

  
	search_tree->set_player_B ( player_B );
	
	
	Settings &settings = _osystem->settings();
	sim_steps_per_node = settings.getInt("sim_steps_per_node", true);
}

CombinationAgent::~CombinationAgent() {
	m_trace.close();
}

int CombinationAgent::num_available_actions() {
  return available_actions.size();
}

ActionVect& CombinationAgent::get_available_actions() {
  return available_actions;
}

Action CombinationAgent::agent_step() {
  Action act = PlayerAgent::agent_step();

  state.incrementFrame();

  return act;
}


/* *********************************************************************
    Returns a random action from the set of possible actions
 ******************************************************************** */
Action CombinationAgent::act() {


  std::cout << std::endl << "===========================================" << std::endl;

  
  cnn->insertScreen(); //--> Insertar nueva screen en el buffer
  
  //VIDEO -> GUARDAR NUEVA IMAGEN
  if(p_osystem->settings().getBool("images", true)) {
    video->callImageGenerator(); //--> Llamar al generador de imagenes para el video
  }
  
  
  

  // Generate a new action every sim_steps_per node; otherwise return the
	//  current selected action 
		
	// should be NO_OP, otherwise it sends best action every frame for sim_steps_frames!!!!
	if (frame_number % sim_steps_per_node != 0) {	
		return m_curr_action;
	}
  
	
	//MODOS
	
  int contador = frame_number / sim_steps_per_node; //sim_steps_per_node=5 por lo general
	
	int search_frequency = p_osystem->settings().getInt("search_frequency", true);  //5 por defecto
	if ((contador % search_frequency) == 0) { //si en este frame debe ejecutarse solo el algoritmo de planning
	  
	  m_curr_action = simulate(); //ejecucion algoritmo de planning
	
	}
	
	else {
	
    //CONSENSUS
    if(p_osystem->settings().getString("mode", true) == "consensus") {
      m_curr_action = modeConsensus();      
      std::cout << "confimation --> " << m_curr_action << std::endl;
      
      //ESTADISTICAS DE CONSENSO SUM
      Action iw_action = search_tree->get_best_action();
      Action cnn_action = cnn->get_best_action();
      
      //std::cout << iw_action << " - " << cnn_action << " - " << m_curr_action << std::endl;
      
      stats->addSumConsensus(iw_action, cnn_action, m_curr_action);
      
      /*std::cout << "  match >>>>  " << stats->getPercentageSumActionChoice("match");
      std::cout << "  iw >>>>  " << stats->getPercentageSumActionChoice("iw");
      std::cout << "  cnn >>>>  " << stats->getPercentageSumActionChoice("cnn");
      std::cout << "  accord >>>>  " << stats->getPercentageSumActionChoice("accord");*/
    }

    //PROBABILITY
    else if(p_osystem->settings().getString("mode", true) == "probability") {
      m_curr_action = modeProbability();
      std::cout << "confimation --> " << m_curr_action << std::endl;
    }	  
	
	}
	
	return m_curr_action;
}






////////////////////////////////////////////////////////////////////////////////////////



Action CombinationAgent::simulate() {

	std::cout << "Search Agent action selection: frame=" << frame_number << std::endl;
	std::cout << "Is Terminal Before Lookahead? " << m_rom_settings->isTerminal() << std::endl;
	std::cout << "Evaluating actions: " << std::endl;

  //----SIMULACION----
  
	state = m_env->cloneState();

	float t0 = aptk::time_used();
	
	if (search_tree->is_built ) {
		// Re-use the old tree
		search_tree->move_to_best_sub_branch();
		//assert(search_tree->get_root()->state.equals(state));
		if( search_tree->get_root()->state.equals(state) ){
		    //assert(search_tree->get_root()->state.equals(state));
			//assert (search_tree->get_root_frame_number() == state.getFrameNumber());
			search_tree->update_tree();
			
		}
		else{
			//std::cout << "\n\n\tDIFFERENT STATE!\n" << std::endl;
			search_tree->clear(); 
			search_tree->build(state);
		}
	} else 
	    {
		// Build a new Search-Tree
		search_tree->clear(); 
		search_tree->build(state);
	}

	Action best = search_tree->get_best_action();
  search_tree->print_action_rewards(); //imprime los refuerzos asociados a cada accion

	m_env->restoreState( state );
	
	std::cout << "Is Terminal After Lookahead? " << m_rom_settings->isTerminal() << std::endl;

	float tf = aptk::time_used();

  float elapsed = tf - t0;
  
  //ESTADISTICAS DE SIMULACION
	stats->addSimTime(elapsed);
	stats->addSimExpanded(search_tree->expanded_nodes());
	stats->addSimGenerated(search_tree->generated_nodes());
	stats->addSimDepthTree(search_tree->max_depth());
	stats->addSimTreeSize(search_tree->num_nodes());  
	
	search_tree->print_frame_data( frame_number, elapsed, m_curr_action, m_trace );
	search_tree->print_frame_data( frame_number, elapsed, m_curr_action, std::cout );


	return best;
}


/*
  Ejecuta el modo Consensus de la combinacion IW+DQN
*/
Action CombinationAgent::modeConsensus() {
  
  double iw_ratio = 1.0 - cnn->cnn_ratio;

  //LLAMAR A CNN
  cnn->callCNN(); //Recibe refuerzos de CNN normalizados
  
  //NORMALIZACION CNN
  
  for (int i = 0; i < cnn->q_values.size(); i++) {
    std::cout << "CNN -> " << cnn->q_values[i] << std::endl; 
  }
  
  cnn->normalize();
  
  std::cout << std::endl; 
  for (int i = 0; i < cnn->q_values.size(); i++) {
    std::cout << cnn->q_values[i] << std::endl; 
  }
  std::cout << std::endl; 
  
  //-----------------------------
  
  //SIMULAR CON ALG PLANNING
  simulate();
  
  //NORMALIZACION IW
  
  std::vector<return_t> branch_rewards = search_tree->get_branch_rewards(); //rewards de cada rama (es una lista desordenada, es decir, las acciones no estan en orden, sino de mayor a menor profundidad de la simulacion)


  for (int i = 0; i < branch_rewards.size(); i++) {
    std::cout << "IW -> " << branch_rewards[i] << std::endl; 
  }

  branch_rewards = search_tree->normalize(branch_rewards);
  
  std::cout << std::endl; 
  for (int i = 0; i < branch_rewards.size(); i++) {
    std::cout << branch_rewards[i] << std::endl; 
  }
  std::cout << std::endl; 
 
  //ACCION DEFINITIVA
  
  //Suma ponderada de refuerzos
  std::vector<return_t> rewards_finales;
  //matching de acciones CNN e IW, para la suma de sus respectivos rewards
  for(int i = 0; i < cnn->actions_order.size(); i++) {
    for(int j = 0; j < search_tree->get_root()->available_actions.size(); j++) {
      if(cnn->actions_order[i] == search_tree->get_root()->available_actions[j]) { //falla el available_actions
        rewards_finales.push_back((cnn->cnn_ratio * cnn->q_values[i]) + (iw_ratio * branch_rewards[j]));  //suma ponderada
        std::cout << cnn->cnn_ratio << " * " << cnn->q_values[i] << " + " << iw_ratio << " * " << branch_rewards[j] << std::endl;
      }
    }
  }
  
  //Maximo refuerzo final     
  Action best = get_best_action(rewards_finales, cnn->actions_order);  //mejor accion
  std::cout << "--> " << best << std::endl; 
  return best;
    
}


/*
  Ejecuta el modo Probability de la combinacion IW+DQN
*/
Action CombinationAgent::modeProbability() {

  srand (time(NULL));  //importante para obtener numeros pseudoaleatorios
  double random = (double) (rand() / (RAND_MAX + 1.));
  std::cout << random << std::endl;
  std::cout << std::endl;
  if(random < cnn->cnn_ratio) {  //si la probabilidad cae a favor de CNN
    
	  //LLAMAR A CNN
    cnn->callCNN(); //Recibe refuerzos de CNN 
    
    for (int i = 0; i < cnn->q_values.size(); i++) {
      std::cout << cnn->q_values[i] << std::endl; 
    }
    std::cout << std::endl; 
       
    //Obtener la mejor accion para la CNN
    Action best = cnn->get_best_action();  //mejor accion
    std::cout << "-----> " << best << std::endl; 
    return best; 
    
    
  }
  
  else {  //probabilidad cae a favor de IW
    Action best = simulate();
    std::cout << "=====> " << best << std::endl; 
    return best;
  }
  
}


/*
  Dada una lista de refuerzos y una lista de orden de acciones, devuelve 
  la accion de mayor refuerzo
*/
Action CombinationAgent::get_best_action(vector<float> lista, vector<int> order) {
  
  float maxFinal = -INFINITY;
  vector<int> maximos;
  for(int i = 0; i < lista.size(); i++) {
    if(maxFinal < lista[i] ) {
      maximos.clear();  //borra todas las acciones de la lista que tenian el mayor refuerzo hasta el momento
      maxFinal = lista[i];
      maximos.push_back(order[i]);   //añade la accion de mayor refuerzo
    }
    else if(maxFinal == lista[i]) {
      maximos.push_back(order[i]);
    }
  }
  
  return (Action) maximos[rand() % (maximos.size())];
  
}




////////////////////////////////////////////////////////////////////////////////////////







/* *********************************************************************
    This method is called when the game ends. 
 ******************************************************************** */
void CombinationAgent::episode_end(void) {
  PlayerAgent::episode_end();
	// Our search-tree is useless now. Clear it
	search_tree->clear();
}

Action CombinationAgent::episode_start(void) {
  Action a = PlayerAgent::episode_start();

  state.incrementFrame();
  m_current_episode++;
  return a;
}

