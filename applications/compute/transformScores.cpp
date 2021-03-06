#include <iostream>
#include <cstdlib>
#include <tuple>
#include <functional>
#include <map>
#include <initializer_list>
#include <utility>

#include <boost/program_options.hpp>

#include <genetrail2/core/EntityDatabase.h>
#include <genetrail2/core/GeneSet.h>
#include <genetrail2/core/GeneSetReader.h>
#include <genetrail2/core/GeneSetWriter.h>
#include <genetrail2/core/Scores.h>

using namespace GeneTrail;
namespace bpo = boost::program_options;

std::string scores = "", output = "", method = "";

std::map<std::string, std::function<void(GeneSet&)>> apply{
    {"abs", GeneTrail::Overloads::abs},
    {"sqrt", GeneTrail::Overloads::sqrt},
    {"log", GeneTrail::Overloads::log},
    {"log2", GeneTrail::Overloads::log2},
    {"log10", GeneTrail::Overloads::log10},
    {"pow2", GeneTrail::Overloads::pow2}};

bool parseArguments(int argc, char* argv[])
{
	bpo::variables_map vm;
	bpo::options_description desc;

	desc.add_options()("help,h", "Display this message")
		("scores,s", bpo::value<std::string>(&scores)->required(), "Name of the scoring file.")
		("output,o", bpo::value<std::string>(&output)->required(), "Name of the output file.")
		("method,m", bpo::value<std::string>(&method)->required(), "Method to transform the scores.");

	try
	{
		bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
		bpo::notify(vm);
	}
	catch(bpo::error& e)
	{
		std::cerr << "ERROR: " << e.what() << "\n";
		desc.print(std::cerr);
		return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	if(!parseArguments(argc, argv))
	{
		return -1;
	}

	GeneSetReader reader;
	auto gene_set = reader.readScoringFile(scores);
	apply[method](gene_set);

	auto db = std::make_shared<EntityDatabase>();
	GeneSetWriter writer;
	writer.writeScoringFile(Scores(gene_set, db), output);

	return 0;
}
