/* -*-c++-*--------------------------------------------------------------------
 * 2019 Bernd Pfrommer bernd.pfrommer@gmail.com
 */

#include "tagslam/graph.h"
#include "tagslam/value/pose.h"
#include "tagslam/profiler.h"

#include <ros/ros.h>
#include <memory>
#include <set>
#include <deque>

#pragma once

namespace tagslam {
  class Optimizer;
  namespace value {
    class Pose;
  }
 
  struct SubGraph {
    typedef std::deque<VertexDesc> FactorCollection;
    typedef std::set<VertexDesc>  ValueCollection;
    FactorCollection  factors;
    ValueCollection   values;
    double            error_{0};
  };

  class GraphUpdater {
    using string = std::string;
  public:
    typedef std::deque<VertexDesc> VertexDeque;
    // ---------------------
    void setGraph(const GraphPtr &g)   { graph_ = g; }
    void setOptimizeFullGraph(bool fg) { optimizeFullGraph_ = fg; }
    void setMaxSubgraphError(double e) { maxSubgraphError_ = e; }
    void setMaxNumIncrementalOpt(int n) { maxNumIncrementalOpt_ = n; }
    void setMinimumViewingAngle(double angDeg);

    void processNewFactors(const ros::Time &t, const VertexVec &facs);
    void printPerformance();
  private:
    typedef std::map<ros::Time, VertexVec> TimeToVertexesMap;
    void examine(const ros::Time &t, VertexDesc fac,
                 VertexDeque *factorsToExamine,
                 SubGraph *found, SubGraph *sg);

    std::vector<VertexDeque>
    findSubgraphs(const ros::Time &t, const VertexVec &fac, SubGraph *found);
 
    double initializeSubgraphs(std::vector<GraphPtr> *subGraphs,
                               const std::vector<VertexDeque> &verts);
    void exploreSubGraph(const ros::Time &t, VertexDesc start,
                         SubGraph *subGraph, SubGraph *found);
    bool applyFactorsToGraph(const ros::Time &t, const VertexVec &facs,
                             SubGraph *covered);
    void eraseStoredFactors(const ros::Time &t,
                            const SubGraph::FactorCollection &covered);
    double optimize(double thresh);
    // ------ variables --------------
    bool               optimizeFullGraph_;
    GraphPtr           graph_;
    TimeToVertexesMap  oldFactors_;
    Profiler           profiler_;
    double             maxSubgraphError_{15.0};
    int                numIncrementalOpt_{0};
    int                maxNumIncrementalOpt_{100};
    double             subgraphError_{0};
    double             minimumViewingAngle_{0};
    double             lastIncError_{0};
  };
}
