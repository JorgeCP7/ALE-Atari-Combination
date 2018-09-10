#ifndef __STATISTICS_HPP__
#define __STATISTICS_HPP__

#include "../emucore/OSystem.hxx"
#include "../emucore/m6502/src/System.hxx"
#include "../environment/stella_environment.hpp"


class Statistics {
  
  public:
  
    //Metodos
    Statistics();
    virtual ~Statistics() {}
    //----------------------------
    void addSumConsensus(int iw_action, int cnn_action, int final_action);  
    //----------------------------
    void addSumActionChoice(string option);
    void addSimTime(double a);
    void addSimExpanded(long a);
    void addSimGenerated(long a);
    void addSimDepthTree(long a);
    void addSimTreeSize(long a);
    //----------------------------
    double getPercentageSumActionChoice(string option);
    double getAverageSimTime();
    double getAverageSimExpanded();
    double getAverageSimGenerated();
    double getAverageSimDepthTree();
    double getAverageSimTreeSize();

    
    //Atributos
    std::vector<long> sum_a_action_choice;
    std::vector<double> sim_a_time;
    std::vector<long> sim_a_expanded;
    std::vector<long> sim_a_generated;
    std::vector<long> sim_a_depth_tree;
    std::vector<long> sim_a_tree_size;
};


#endif // __STATISTICS_HPP__
