/*
 * GeneTrail2 - An efficient library for interpreting genetic data
 * Copyright (C) 2014-2019 Tim Kehl tkehl@bioinf.uni-sb.de>
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
#ifndef GT2_CORE_FISHERS_EXACT_TEST_H
#define GT2_CORE_FISHERS_EXACT_TEST_H

#include "macros.h"

#include <boost/math/special_functions/binomial.hpp>

#include <iostream>

namespace GeneTrail
{

	/**
	 * Fisher's exact test
	 */
	template <typename unsigned_integer_type = unsigned int,
	          typename return_type = double>
	class GT2_EXPORT FishersExactTest
	{
		public:
		/**
		 * This method implements the Fisher's exact test.
		 *
		 * @param m Number of genes in the reference set (population size)
		 * @param l Number of genes in category (success states in the
		 *          population)
		 * @param n Number of genes in test set (Number of draws)
		 * @param k Number of genes in the test set that belong to the category
		 *          (Number of successes)
		 * @return
		 */
		return_type compute(const unsigned_integer_type& m,
		                    const unsigned_integer_type& l,
		                    const unsigned_integer_type& n,
		                    const unsigned_integer_type& k) const
		{
			return (boost::math::binomial_coefficient<return_type>(n, k) *
			        boost::math::binomial_coefficient<return_type>(m, l)) /
			        boost::math::binomial_coefficient<return_type>(m + n, l + k);
		}

		/**
		 * This method computes the lower-tailed p-value for the given event of
		 *the Fisher's exact test.
		 *
		 * @param m Number of genes in the reference set (population size)
		 * @param l Number of genes in category (success states in the
		 *          population)
		 * @param n Number of genes in test set (Number of draws)
		 * @param k Number of genes in the test set that belong to the category
		 *          (Number of successes)
		 * @return
		 */
		return_type lowerTailedPValue(const unsigned_integer_type& m,
		                              const unsigned_integer_type& l,
		                              const unsigned_integer_type& n,
		                              const unsigned_integer_type& k) const
		{
			return_type p = 0;
			// Make sure we do not compute undefined binomial coefficients
			unsigned_integer_type i = (l + k > m) ? (l + k) - m : 0; // Brackets are here to avoid underrun
			for(; i <= k; ++i) {
				p += boost::math::binomial_coefficient<return_type>(n, i) *
				     boost::math::binomial_coefficient<return_type>(m, l + k - i);
			}
			return p / boost::math::binomial_coefficient<return_type>(m + n, l + k);
		}

		/**
		 * This method computes the minimum k needed to get a significant lower-tailed 
		 * p-value for the Fisher's Exact test.
		 *
		 * @param m Number of genes in the reference set (population size)
		 * @param l Number of genes in category (success states in the population)
		 * @param n Number of genes in test set (Number of draws)
		 *
		 * @return k
		 */
		unsigned_integer_type findSignificantKLowerTailed(const unsigned_integer_type& m, const unsigned_integer_type& l,
		                              const unsigned_integer_type& n, const unsigned_integer_type& k, const return_type& threshold) const
		{
			return_type p = 0.0;
			unsigned_integer_type d = std::min(n, l);
			return_type denom = boost::math::binomial_coefficient<return_type>(m + n, l + k);
			// Make sure we do not compute undefined binomial coefficients
			unsigned_integer_type i = (l + k > m) ? (l + k) - m : 0; // Brackets are here to avoid underrun
			for(; i <= d; ++i) {
				p += boost::math::binomial_coefficient<return_type>(n, i) *
				     boost::math::binomial_coefficient<return_type>(m, l + k - i);
				if (p/denom > threshold) {
					return i;
				}
			}
			return d;
		}

		/**
		 * This method computes the upper-tailed p-value for the given event of
		 *the Fisher's exact test.
		 *
		 * @param m Number of genes in the reference set (population size)
		 * @param l Number of genes in category (success states in the
		 *          population)
		 * @param n Number of genes in test set (Number of draws)
		 * @param k Number of genes in the test set that belong to the category
		 *          (Number of successes)
		 *
		 * @return
		 */
		return_type upperTailedPValue(const unsigned_integer_type& m,
		                              const unsigned_integer_type& l,
		                              const unsigned_integer_type& n,
		                              const unsigned_integer_type& k) const
		{	
			return_type p = 0;
			unsigned_integer_type d = std::min(n, l + k);
			for(unsigned_integer_type i = k; i <= d; ++i) {
				p += boost::math::binomial_coefficient<return_type>(n, i) *
				     boost::math::binomial_coefficient<return_type>(m, l + k - i);
			}
			return p / boost::math::binomial_coefficient<return_type>(m + n, l + k);
		}

		/**
		 * This method computes the minimum k needed to get a significant upper-tailed 
		 * p-value for the Fisher's exact test.
		 *
		 * @param m Number of genes in the reference set (population size)
		 * @param l Number of genes in category (success states in the population)
		 * @param n Number of genes in test set (Number of draws)
		 *
		 * @return k
		 */
		unsigned_integer_type findSignificantKUpperTailed(const unsigned_integer_type& m, const unsigned_integer_type& l,
		                              const unsigned_integer_type& n, const unsigned_integer_type& k, const return_type& threshold) const
		{
			return_type p = 0.0;
			return_type denom = boost::math::binomial_coefficient<return_type>(m + n, l + k);
			unsigned_integer_type d = std::min(n, l);
			for(unsigned_integer_type i = d; i > 1; --i) {
				p += boost::math::binomial_coefficient<return_type>(n, i) *
				     boost::math::binomial_coefficient<return_type>(m, l + k - i);
				if (p/denom > threshold) {
					return i;
				}
			}
			return 0;
		}
	};
}

#endif // GT2_CORE_FISHERS_EXACT_TEST_H

