#include "matrixTools.h"

#include <genetrail2/core/DenseMatrix.h>
#include <genetrail2/core/DenseMatrixReader.h>

#include <iostream>
#include <fstream>

namespace GeneTrail
{

	std::vector<unsigned int>
	getIndices(const DenseMatrix& matrix,
	           const std::vector<std::string>& colnames,
	           const std::string& groupname)
	{
		MatrixTools tools;
		return tools.getIndices(matrix, colnames, groupname);
	}

	std::tuple<DenseColumnSubset, DenseColumnSubset>
	splitMatrix(DenseMatrix& matrix, const std::vector<std::string>& reference,
	            const std::vector<std::string>& test)
	{
		MatrixTools tools;
		return tools.splitMatrix(matrix, reference, test);
	}

	DenseMatrix buildDenseMatrix(const std::string& expr1,
	                             const std::string& expr2,
	                             const MatrixReaderOptions& options)
	{
		auto m1 = readDenseMatrix(expr1, options);
		if(expr2 != "") {
			auto m2 = readDenseMatrix(expr2, options);
			m1.cbind(m2);
		}
		return m1;
	}

	DenseMatrix readDenseMatrix(const std::string& matrix,
	                            const MatrixReaderOptions& options)
	{
		unsigned int opts = DenseMatrixReader::NO_OPTIONS;

		if(!options.no_rownames) {
			opts |= DenseMatrixReader::READ_ROW_NAMES;
		}

		if(!options.no_colnames) {
			opts |= DenseMatrixReader::READ_COL_NAMES;
		}

		if(options.additional_colname) {
			opts |= DenseMatrixReader::ADDITIONAL_COL_NAME;
		}
		
		if(options.split_only_tab) {
			opts |= DenseMatrixReader::SPLIT_ONLY_TAB;
		}

		DenseMatrixReader reader;

		std::ifstream strm(matrix, std::ios::binary);
		return reader.read(strm, opts);
	}
}
