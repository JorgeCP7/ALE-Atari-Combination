/* *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2012 by Yavar Naddaf, Joel Veness, Marc G. Bellemare
 * Released under GNU General Public License www.gnu.org/licenses/gpl-3.0.txt
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 *
 * *****************************************************************************
 *  CombinationAgent.hpp 
 *
 * The implementation of the CombinationAgent class, which uses Search and learning Algorithms
 * to act in the game
 **************************************************************************** */

#ifndef __COMBINATION_AGENT_HPP__
#define __COMBINATION_AGENT_HPP__

#include "Constants.h"
#include "PlayerAgent.hpp"
#include "OSystem.hxx"
#include "../environment/ale_state.hpp"
#include <fstream>

#include "VideoController.hpp"

#include "CNNController.hpp"
#include "SearchTree.hpp"


class CombinationAgent : public PlayerAgent {
    public:
	CombinationAgent(OSystem * _osystem, RomSettings * _settings, StellaEnvironment* _env, bool player_B = false);
        virtual ~CombinationAgent();
		
        /* *********************************************************************
            This method is called when the game ends. 
         ******************************************************************** */
        virtual void episode_end(void);
        
        virtual Action episode_start(void);

        virtual Action agent_step();
        
        /////////////////////////////////////////
        
        virtual Action simulate();
        virtual Action modeConsensus();
        virtual Action modeProbability();
        virtual Action get_best_action(vector<float> lista, vector<int> order);        
        
        /////////////////////////////////////////
        

	protected:
        /* *********************************************************************
            Returns the best action from the set of possible actions
         ******************************************************************** */
        virtual Action act();
        
        int num_available_actions();
        ActionVect &get_available_actions();


protected:
	Action m_curr_action;
	ALEState state;
	RomSettings * m_rom_settings;
	StellaEnvironment* m_env;
	int sim_steps_per_node;
	
	string search_method;
	unsigned m_current_episode;
	std::ofstream m_trace;
	
	/////////////////////////////////
  public:
  
    CNNController * cnn;
    SearchTree* search_tree;
	
	  VideoController * video;
	
	/////////////////////////////////
	
};

#endif // __COMBINATION_AGENT_HPP__

