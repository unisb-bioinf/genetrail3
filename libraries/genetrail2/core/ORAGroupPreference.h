/*
 * GeneTrail2 - An efficient library for interpreting genetic data
 * Copyright (C) 2014 Tim Kehl <tkehl@bioinf.uni-sb.de>
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
#ifndef GT2_ORA_GROUP_PREFERENCE_H
#define GT2_ORA_GROUP_PREFERENCE_H

#include "macros.h"
#include "DenseMatrix.h"
#include "Metadata.h"
#include <boost/math/distributions/chi_squared.hpp>
#include "multiprecision.h"


#include <utility>
#include <tuple>

#include <boost/math/special_functions/binomial.hpp>

using boost::math::chi_squared;
namespace GeneTrail {

    class GT2_EXPORT ORAGroupPreference {
		public:
			ORAGroupPreference():test_(1){};
			ORAGroupPreference(double threshold): threshold_(threshold), test_(1){};

			/**
			 * This method accepts a (category x sample) matrix file containing p-values
			 * for the category in the respective sample, and a Metadata object with a
			 * key for each sample providing a std::string representing the group of the
			 * corresponding sample. This method calculates a p-value for each category-
			 * group combination on how significant a category is only present in the
			 * respective group.
			 *
			 * @param matrix a (category x sample) matrix with column- and row names.
			 * @param metadata a Metadata object having a key for each sample.
			 * @param result an empty DenseMatrix that is used to store the resulting 
			 *               (category x group) DenseMatrix
			 */
			void calculatePreference(
				const DenseMatrix& matrix,
				const std::vector<Metadata>& metadata,
				DenseMatrix& result
			) const;
			
			static void parseGroups(
				const DenseMatrix& matrix,
				const std::vector<Metadata>& metadata,
				std::map<std::string, std::vector<unsigned int>>& group_indices
			);
			
			static std::vector<std::string> getNonEmpty(
				const std::vector<std::vector<unsigned int>>& group_indices,
				const std::vector<std::string>& groups);
			
		private:
			unsigned int MINDEX = 0;
			unsigned int LINDEX = 1;
			unsigned int NINDEX = 2;
			unsigned int KINDEX = 3;
			double threshold_;
			chi_squared test_;
			
			void addToTable(std::vector<size_t>& table, double p_value, bool is_current_group) const;
			double computePValue_(const std::vector<size_t>& table) const;
			double computePValue_(size_t m, size_t l, size_t n, size_t k) const;
	};
	
}

#endif // GT2_CORE_OVER_REPRESENTATION_ANALYSIS_H

