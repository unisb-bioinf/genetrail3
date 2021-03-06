/*
 * GeneTrail2 - An efficient library for interpreting genetic data
 * Copyright (C) 2013 Tim Kehl <tkehl@bioinf.uni-sb.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public
 * License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GT2_CORE_PATHFINDER_H
#define GT2_CORE_PATHFINDER_H

#include "macros.h"

#include <string>
#include <tuple>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include  <cstdint>

#include "BoostGraph.h"
#include "Path.h"

namespace GeneTrail {

    class GT2_EXPORT Pathfinder {
    public:

        Pathfinder() {
        };

        ~Pathfinder() {
        };

        /**
         * Prints the given matrix
         *
         * @param m Two dimensional vector representing a matrix
         */
        void printMatrix(const std::vector<std::vector<int> >& m);

        /**
         * Computes the RunningSum (Simplified version of the formula from the FiDePa paper)
         *
         * @param bpi Number of genes with rank less or equal to i
         * @param n Number of genes in List
         * @param i The current rank
         * @param l The current length of path
         * @return The computed Running Sum
         */
        int computeRunningSum(int bpi, int n, int i, int l);

        /**
         * Initializes all fields and and the first layer of the matrix
         *
         * @param graph KEGG network as Boost graph structure
         * @param sorted_gene_list Gene list sorted by rank
         * @param length Maximal length of path
         */
        void initializeFields(const GraphType& graph, const std::vector<std::string>& sorted_gene_list, const int& length);

        /**
         * Finds the predecessor with best running sum
         *
         * @param[in] graph KEGG network as Boost graph object
         * @param[out] best_pred_k stores the index of the best predecessor
         * @param[out] best_pred_k_running_sum the running sum for the best predecessor
         * @param[in,out] pred_flag Saves the number of predecessors
         * @param[in] l The current layer
         * @param[in] vd The current vertex
         */
        void findBestPredecessor(const GraphType& graph, int& best_pred_k, int& best_pred_k_running_sum, int& pred_flag, int l, const vertex_descriptor& vd);

        /**
         * Fills the next layer based on the best predecessor
         *
         * @param best_pred_k Best predecessor of k
         * @param k Current vertex
         */
        void fillNextLayer(int best_pred_k, int k);

        /**
         * Computes the running sum for a path of length l ending in vertex k
         *
         * @param k Current vertex
         * @param l Length of the path
         * @param max_runnig_sum_k Saves the running sum
         */
        void computeRunningSum(int k, int l, int& max_runnig_sum_k);

        /**
         * Computes best possible deregulated paths according to the FiDePa algorithm.
         *
         * @param graph KEGG network as BOOST graph object
         * @param sorted_gene_list Gene list sorted by rank
         * @param length Maximal length of path
         */
        std::vector<GeneTrail::Path> computeDeregulatedPath(const GraphType& graph, const std::vector<std::string>& sorted_gene_list, const int& length);
        
    private:
        //Map from name to rank in gene list
        std::map<std::string, int> name2rank;

        //Map from rank to vertex_descriptor
        std::map<vertex_descriptor, int> vertex_map;

        std::vector<vertex_descriptor> nodes;

        //Layers of the matrix 
        std::vector<std::vector<int> > M_1;
        std::vector<std::vector<int> > M_2;

        // Iterator for vertices
        boost::graph_traits<GraphType>::vertex_iterator vi, vi_end;

        // Iterator for all ingoing edges of a vertex
        boost::graph_traits<GraphType>::in_edge_iterator iei, iei_end;

        // Iterator for all outgoing edges of a vertex
        boost::graph_traits<GraphType>::out_edge_iterator oei, oei_end;

        // This map contains for each layer a mapping for each vertex to its predecessor
        std::vector<std::map<int, int> > best_preds_v;

        //Compute RS for all paths
        std::vector<std::vector<int > > running_sums;
    };
}

#endif //GT2_CORE_PATHFINDER_H

