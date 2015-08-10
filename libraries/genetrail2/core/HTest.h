#ifndef GT2_CORE_HTEST_H
#define GT2_CORE_HTEST_H

#include <boost/math/distributions.hpp>

namespace GeneTrail
{
	namespace HTest
	{
		template <typename Test, typename InputIterator1,
		          typename InputIterator2>
		typename Test::value_type test(Test& t,
		                               const InputIterator1& first_begin,
		                               const InputIterator1& first_end,
		                               const InputIterator2& second_begin,
		                               const InputIterator2& second_end)
		{
			return t.test(first_begin, first_end, second_begin, second_end);
		}

		template <typename Test, typename InputIterator>
		typename Test::value_type test(Test& t, const InputIterator& begin,
		                               const InputIterator& end)
		{
			return t.test(begin, end);
		}

		template <typename Test>
		typename Test::value_type
		twoSidedPValue(Test& t, const typename Test::value_type& score)
		{
			return 2.0 * boost::math::cdf(boost::math::complement(
			                 t.distribution(), fabs(score)));
		}

		template <typename Test>
		std::pair<typename Test::value_type, typename Test::value_type>
		confidenceInterval(Test& t, const typename Test::value_type& alpha)
		{
			return t.confidenceInterval(alpha);
		}

		template <typename Test>
		typename Test::value_type
		lowerTailedPValue(Test& t, const typename Test::value_type& score)
		{
			return boost::math::cdf(t.distribution(), score);
		}

		template <typename Test>
		typename Test::value_type
		upperTailedPValue(Test& t, const typename Test::value_type& score)
		{
			return boost::math::cdf(
			    boost::math::complement(t.distribution(), score));
		}
	}
}

#endif // GT2_CORE_HTEST_H
