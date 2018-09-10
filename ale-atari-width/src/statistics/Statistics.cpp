#include "Statistics.hpp"

Statistics::Statistics()
  {
    sum_a_action_choice = {0, 0, 0, 0, 0};
    sim_a_time = {0, 0};
    sim_a_expanded = {0, 0};
    sim_a_generated = {0, 0};
    sim_a_depth_tree = {0, 0};
    sim_a_tree_size = {0, 0};
  }
   
  
//----------------------------------------- 


void Statistics::addSumConsensus(int iw_action, int cnn_action, int final_action) {  
  if(iw_action == cnn_action) {
    addSumActionChoice("match");
  }
  else {
    if(cnn_action == final_action) {
      addSumActionChoice("cnn");
    }
    else if(iw_action == final_action) {
      addSumActionChoice("iw");
    }
    else {
      addSumActionChoice("accord");
    }
  }  
}

//-----------------------------------------


   
   
void Statistics::addSumActionChoice(string option) {
  if(option == "match") {
    sum_a_action_choice[0]++;
  }
  else if(option == "cnn") {
    sum_a_action_choice[1]++;
  }
  else if(option == "iw") {
    sum_a_action_choice[2]++;
  }
  else if(option == "accord") {
    sum_a_action_choice[3]++;
  }
  
  sum_a_action_choice[4]++;
  
}

void Statistics::addSimTime(double a) {
  sim_a_time[0] = sim_a_time[0] + a;
  sim_a_time[1]++;
}

void Statistics::addSimExpanded(long a) {
  sim_a_expanded[0] = sim_a_expanded[0] + a;
  sim_a_expanded[1]++;
}

void Statistics::addSimGenerated(long a) {
  sim_a_generated[0] = sim_a_generated[0] + a;
  sim_a_generated[1]++;
}

void Statistics::addSimDepthTree(long a) {
  sim_a_depth_tree[0] = sim_a_depth_tree[0] + a;
  sim_a_depth_tree[1]++;
}

void Statistics::addSimTreeSize(long a) {
  sim_a_tree_size[0] = sim_a_tree_size[0] + a;
  sim_a_tree_size[1]++;
}

//----------------------------

double Statistics::getPercentageSumActionChoice(string option) {
  if(option == "match") {
    return sum_a_action_choice[0] / (double) sum_a_action_choice[4];
  }
  else if(option == "cnn") {
    return sum_a_action_choice[1] / (double) sum_a_action_choice[4];
  }
  else if(option == "iw") {
    return sum_a_action_choice[2] / (double) sum_a_action_choice[4];
  }
  else if(option == "accord") {
    return sum_a_action_choice[3] / (double) sum_a_action_choice[4];
  }
  
}

double Statistics::getAverageSimTime() {
  return sim_a_time[0] / sim_a_time[1];
}

double Statistics::getAverageSimExpanded() {
  return sim_a_expanded[0] / (double) sim_a_expanded[1];
}

double Statistics::getAverageSimGenerated() {
  return sim_a_generated[0] / (double) sim_a_generated[1];
}

double Statistics::getAverageSimDepthTree() {
  return sim_a_depth_tree[0] / (double) sim_a_depth_tree[1];
}

double Statistics::getAverageSimTreeSize() {
  return sim_a_tree_size[0] / (double) sim_a_tree_size[1];
}
  

