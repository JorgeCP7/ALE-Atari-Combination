/* *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2013 by Yavar Naddaf, Joel Veness, Marc G. Bellemare and 
 *   the Reinforcement Learning and Artificial Intelligence Laboratory
 * Released under the GNU General Public License; see License.txt for details. 
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 *  internal_controller.hpp
 *
 *  The InternalController class allows users to directly interface with ALE.
 *
 **************************************************************************** */

#include "internal_controller.hpp"
#include "time.hxx"
#include <fstream>
#include <sstream>
#include <time.h>

InternalController::InternalController(OSystem* osystem):
  ALEController(osystem),
  m_max_num_frames(0),
  m_episode_score(0),
  m_episode_number(0),
  m_agent_left(NULL),
  m_agent_right(NULL) {

  m_max_num_frames = m_osystem->settings().getInt("max_num_frames");
  m_max_num_episodes = m_osystem->settings().getInt("max_num_episodes");

  createAgents();
}

void InternalController::createAgents() {
    m_agent_left.reset(PlayerAgent::generate_agent_instance(m_osystem, m_settings.get(), &m_environment));

  // Right agent is set to NULL. While this isn't necessary, all of the currently implemented
  //  agents return actions for player A. One easy fix would be to add PLAYER_B_NOOP to actions
  //  returned by such agents... we'll get around to it.
    if (m_osystem->settings().getString("search_method_B",false) != "")
	    m_agent_right.reset(PlayerAgent::generate_agent_instance(m_osystem, m_settings.get(), &m_environment, true));
    else
	    m_agent_right.reset(NULL);
}

bool InternalController::isDone() {
  // Die once we reach enough samples
  return ((m_max_num_frames > 0 && m_environment.getFrameNumber() >= m_max_num_frames) ||
    (m_max_num_episodes > 0 && m_episode_number > m_max_num_episodes) ||
    (m_agent_left.get() != NULL && m_agent_left->has_terminated()) ||
    (m_agent_right.get() != NULL && m_agent_right->has_terminated()));
}

void InternalController::run() {
  Action action_a, action_b;

  bool firstStep = true;

  while (!isDone()) {
    // Start a new episode if we're in a terminal state... assume these agents need to be told
    //  about episode-end
    if (m_environment.isTerminal()) {
      episodeEnd();
      firstStep = true;
    }

    else {
      if (firstStep) {
        // Start a new episode; obtain actions
        episodeStart(action_a, action_b);
        firstStep = false;
      }
      else
        // Poll agents for actions
        episodeStep(action_a, action_b);
    
      // Apply said actions
      m_episode_score += applyActions(action_a, action_b);
    }

    // Display if necessary
    display();
  }
  /////////////////////////////////////////////////////////////
  //Si se supera el numero maximo de frames --> episodeEnd()
  if(m_max_num_frames > 0 && m_environment.getFrameNumber() >= m_max_num_frames) {
    episodeEnd();
  }
  /////////////////////////////////////////////////////////////
}

void InternalController::episodeStep(Action& action_a, Action& action_b) {
  // Request a new action
    if ( m_agent_left.get() != nullptr ){
	ALEState new_state = m_environment.cloneState();
	ALEState* p_new_state = new ALEState(new_state, new_state.serialized() );
	string kk = new_state.serialized();
	m_agent_left->update_state( p_new_state );
    }
    if ( m_agent_right.get() != nullptr ){
	ALEState new_state = m_environment.cloneState();
	ALEState* p_new_state = new ALEState(new_state, new_state.serialized() );
	m_agent_right->update_state( p_new_state );
    }
  action_b = (m_agent_right.get() != NULL) ? m_agent_right->agent_step() : PLAYER_B_NOOP;

  action_a = (m_agent_left.get() != NULL) ? m_agent_left->agent_step() : PLAYER_A_NOOP;
}

void InternalController::episodeStart(Action& action_a, Action& action_b) {
  // Poll the agents for their first action
    if ( m_agent_left.get() != nullptr ){
	ALEState new_state = m_environment.cloneState();
	ALEState* p_new_state = new ALEState(new_state, new_state.serialized() );
	m_agent_left->update_state( p_new_state );
    }
    if ( m_agent_right.get() != nullptr ){
	ALEState new_state = m_environment.cloneState();
	ALEState* p_new_state = new ALEState(new_state, new_state.serialized() );
	m_agent_right->update_state( p_new_state );
    }
  
  action_a = (m_agent_left.get() != NULL) ? m_agent_left->episode_start() : PLAYER_A_NOOP;
  action_b = (m_agent_right.get() != NULL) ? m_agent_right->episode_start() : PLAYER_B_NOOP;

  // Some bookkeeping
  m_episode_score = 0;
  m_episode_number++;
  ////////////////////////////////////////////
  m_episode_t0 = aptk::time_used();
  time(&m_episode_t0_total);
  ////////////////////////////////////////////
}

void InternalController::episodeEnd() {
	
	////////////////////////////////////////////
	m_episode_tf = aptk::time_used();
	time(&m_episode_tf_total);
	////////////////////////////////////////////
	
	// Notify the agents that this is the	 end
	if (m_agent_left.get() != NULL) m_agent_left->episode_end();
	if (m_agent_right.get() != NULL) m_agent_right->episode_end();
	
	// Reset environment
	m_environment.reset();

	// Produce some meaningful output
	fprintf (stderr, "Episode %d ended, score: %d\n", m_episode_number, m_episode_score);
	
	
	////////////////////////////////////////////////////////////////////////////////////////
	
	std::ifstream fileIn ("out/experiments/experiments-data.txt");
	
	if(!fileIn || fileIn.peek() == std::ifstream::traits_type::eof()) { //si el fichero no existe o esta vacio
    std::ofstream fileOut ("out/experiments/experiments-data.txt", std::ios::app);
    fileOut << "game;";
    fileOut << "max_sim_steps_per_frame;";
    fileOut << "ratio;";
    fileOut << "episode_score;";
    fileOut << "episode_frames;";
    fileOut << "episode_time_1;";
    fileOut << "episode_time_2;";
    fileOut << "sim_a_time;";
    fileOut << "sim_a_expanded;";
    fileOut << "sim_a_generated;";
    fileOut << "sim_a_depth_tree;";
    fileOut << "sim_a_tree_size;";
    fileOut << "sum_match;";
    fileOut << "sum_cnn;";
    fileOut << "sum_iw;";
    fileOut << "sum_accord";
    fileOut << "\n";
    fileOut.close();
  }

  std::ofstream fileOut ("out/experiments/experiments-data.txt", std::ios::app);

	if (fileOut.is_open()) {
	
	  //DATOS
	  string game = m_osystem->settings().getString("game", true);
	  int max_sim_steps_per_frame = m_osystem->settings().getInt("max_sim_steps_per_frame", true);
	  double ratio = m_osystem->settings().getFloat("ratio", true);
	  long episode_score = m_episode_score;
	  long episode_frames = m_environment.getFrameNumber();
	  double episode_time_1 = difftime(m_episode_tf_total, m_episode_t0_total); //double(m_episode_tf - m_episode_t0) / CLOCKS_PER_SEC;
	  double episode_time_2 = m_episode_tf - m_episode_t0;
    double sim_a_time = m_agent_left->stats->getAverageSimTime();
    double sim_a_expanded = m_agent_left->stats->getAverageSimExpanded();
    double sim_a_generated = m_agent_left->stats->getAverageSimGenerated();
    double sim_a_depth_tree = m_agent_left->stats->getAverageSimDepthTree();
    double sim_a_tree_size = m_agent_left->stats->getAverageSimTreeSize();
    double sum_match = -1.0;
    double sum_cnn = -1.0;
    double sum_iw = -1.0;
    double sum_accord = -1.0;
    if(m_osystem->settings().getString("player_agent", true) == "combination_agent" && m_osystem->settings().getString("mode", true) == "consensus") {
      sum_match = m_agent_left->stats->getPercentageSumActionChoice("match");
      sum_cnn = m_agent_left->stats->getPercentageSumActionChoice("cnn");
      sum_iw = m_agent_left->stats->getPercentageSumActionChoice("iw");
      sum_accord = m_agent_left->stats->getPercentageSumActionChoice("accord");
    }
	  
	  //std::cout << sim_a_time << " " << sim_a_expanded << " " << sim_a_generated << " " << sim_a_depth_tree << " " << sim_a_tree_size << std::endl;
	  
	  fileOut << game << ";";
	  fileOut << max_sim_steps_per_frame << ";";
	  fileOut << ratio << ";";
	  fileOut << episode_score << ";";
    fileOut << episode_frames << ";";
    fileOut << episode_time_1 << ";";
    fileOut << episode_time_2 << ";";
    fileOut << sim_a_time << ";";
    fileOut << sim_a_expanded << ";";
    fileOut << sim_a_generated << ";";
    fileOut << sim_a_depth_tree << ";";
    fileOut << sim_a_tree_size << ";";
    fileOut << sum_match << ";";
    fileOut << sum_cnn << ";";
    fileOut << sum_iw << ";";
    fileOut << sum_accord;
    fileOut << "\n";
	  
	}
	
	fileOut.close();
	
	////////////////////////////////////////////////////////////////////////////////////////

	
	std::stringstream filename;
	filename << "episode." << m_episode_number;
	
	std::ofstream output( filename.str().c_str() );
	output << "elapsed_time=" << m_episode_tf - m_episode_t0 << ",score=" << m_episode_score << std::endl;
	output.close();	
}

