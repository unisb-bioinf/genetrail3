#ifndef GT2_SCORES_H
#define GT2_SCORES_H

#include "macros.h"
#include "EntityDatabase.h"

#include <boost/iterator/transform_iterator.hpp>

#include <string>
#include <vector>

namespace GeneTrail
{
	class Category;
	class GeneSet;

	enum class Order {
		Increasing,
		Decreasing
	};

	class GT2_EXPORT Score
	{
		public:
		Score(EntityDatabase& db, const std::string& n, double s) : entity_(db(n)), score_(s) {}
		Score(size_t i, double s) : entity_(i), score_(s) {}

		const std::string& name(const EntityDatabase& db) const { return db(entity_); }
		size_t index() const { return entity_; }

		double& score() { return score_; }
		double score() const { return score_; }

		private:
		size_t entity_;
		double score_;
	};

	class GT2_EXPORT Scores
	{
		public:
		using iterator = std::vector<Score>::iterator;
		using const_iterator = std::vector<Score>::const_iterator;

		class IndexProxy
		{
			private:
			struct ExtractIndex
			{
				size_t operator()(const Score& s) const
				{
					return s.index();
				}
			};

			const std::vector<Score>* data_;

			public:
			using const_iterator = boost::transform_iterator<ExtractIndex, Scores::const_iterator>;
			IndexProxy(const std::vector<Score>* data);

			const_iterator begin() const {
				return boost::make_transform_iterator(data_->begin(), ExtractIndex());
			}

			const_iterator end() const {
				return boost::make_transform_iterator(data_->end(), ExtractIndex());
			}
		};

		class NamesProxy
		{
			private:
			struct ExtractName
			{
				ExtractName(const EntityDatabase* db) : db_(db) {}
				const std::string& operator()(const Score& s) const
				{
					return s.name(*db_);
				};
			private:
			const EntityDatabase* db_;
			};
			const std::vector<Score>* data_;
			const EntityDatabase* db_;

			public:
			using const_iterator =
			    boost::transform_iterator<ExtractName, Scores::const_iterator>;

			NamesProxy(const std::vector<Score>* data, const EntityDatabase* db_);

			const_iterator begin() const
			{
				return boost::make_transform_iterator(data_->begin(),
				                                      ExtractName(db_));
			}
			const_iterator end() const
			{
				return boost::make_transform_iterator(data_->end(),
				                                      ExtractName(db_));
			}
		};

		class ConstScoresProxy
		{
			private:
			struct ExtractScore
			{
				double operator()(const Score& s) const { return s.score(); }
			};
			const std::vector<Score>* data_;

			public:
			using const_iterator =
			    boost::transform_iterator<ExtractScore, Scores::const_iterator>;

			ConstScoresProxy(const std::vector<Score>* data);

			const_iterator begin() const
			{
				return boost::make_transform_iterator(data_->begin(),
				                                      ExtractScore());
			}

			const_iterator end() const
			{
				return boost::make_transform_iterator(data_->end(),
				                                      ExtractScore());
			}
		};

		class ScoresProxy
		{
			private:
			struct ExtractScore
			{
				double& operator()(Score& s) const { return s.score(); }
			};
			std::vector<Score>* data_;

			public:
			using iterator =
			    boost::transform_iterator<ExtractScore, Scores::iterator>;

			ScoresProxy(std::vector<Score>* data);

			iterator begin()
			{
				return boost::make_transform_iterator(data_->begin(),
				                                      ExtractScore());
			}

			iterator end()
			{
				return boost::make_transform_iterator(data_->end(),
				                                      ExtractScore());
			}
		};

		using ScoreIterator = ScoresProxy::iterator;
		using ConstScoreIterator = ConstScoresProxy::const_iterator;
		using ConstNameIterator = NamesProxy::const_iterator;

		explicit Scores(const std::shared_ptr<EntityDatabase>& db);
		explicit Scores(size_t size, const std::shared_ptr<EntityDatabase>& db);
		explicit Scores(std::vector<Score>&& data, const std::shared_ptr<EntityDatabase>& db);
		explicit Scores(const std::vector<Score>& data, const std::shared_ptr<EntityDatabase>& db);
		explicit Scores(const GeneSet& gene_set, const std::shared_ptr<EntityDatabase>& db);
		explicit Scores(GeneSet&& gene_set, const std::shared_ptr<EntityDatabase>& db);

		Scores(const Scores& o) = default;
		Scores(Scores&& o) = default;
		~Scores() = default;

		Scores& operator=(const Scores&) = default;
		Scores& operator=(Scores&&) = default;

		template <typename... Ts> void emplace_back(const char* str, Ts&&... ts)
		{
			data_.emplace_back(db_->index(str), std::forward<Ts>(ts)...);
			updateIsSorted_();
		}

		template <typename... Ts> void emplace_back(const std::string& str, Ts&&... ts)
		{
			data_.emplace_back(db_->index(str), std::forward<Ts>(ts)...);
			updateIsSorted_();
		}

		template <typename... Ts> void emplace_back(std::string&& str, Ts&&... ts)
		{
			data_.emplace_back(db_->index(str), std::forward<Ts>(ts)...);
			updateIsSorted_();
		}

		template <typename... Ts> void emplace_back(Ts&&... ts)
		{
			data_.emplace_back(std::forward<Ts>(ts)...);
			updateIsSorted_();
		}

		iterator begin() { return data_.begin(); }
		iterator end() { return data_.end(); }

		const_iterator begin() const { return data_.begin(); }
		const_iterator end() const { return data_.end(); }

		IndexProxy indices() const { return IndexProxy(&data_); };
		NamesProxy names() const { return NamesProxy(&data_, db_.get()); };
		ConstScoresProxy scores() const { return ConstScoresProxy(&data_); };
		ScoresProxy scores() { return ScoresProxy(&data_); };

		const std::shared_ptr<EntityDatabase>& db() const { return db_; }

		size_t size() const { return data_.size(); }

		Scores subset(const Category& c) const;
		std::vector<size_t> subsetIndices(const Category& c) const;

		const Score& operator[](size_t i) const { return data_[i]; }

		void sortByName();
		void sortByIndex();
		void sortByScore(Order order = Order::Increasing);

		bool isSortedByIndex() { return isSortedByIndex_; }
		bool contains(const std::string& name) const;

		private:
		Scores subsetMerge_(const Category& c) const;
		Scores subsetFind_(const Category& c) const;

		void updateIsSorted_() {
			isSortedByIndex_ = size() <= 1 || (isSortedByIndex_ && data_[size() - 1].index() >= data_[size() - 2].index());
		}

		std::vector<Score> data_;
		bool isSortedByIndex_;
		std::shared_ptr<EntityDatabase> db_;
	};
}

#endif // GT2_SCORES_H
