#include <genetrail2/core/Category.h>
#include <genetrail2/core/GMTFile.h>
#include <genetrail2/core/GeneSetReader.h>
#include <genetrail2/core/GeneSetEnrichmentAnalysis.h>
#include <genetrail2/core/PValue.h>
#include <genetrail2/core/ScoringFile.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <iostream>
#include <fstream>
#include <cstdint>
#include <utility>
#include <map>

using namespace GeneTrail;
namespace bpo = boost::program_options;
namespace bm = boost::math;
using namespace boost::multiprecision;

std::string categories, scores = "", adjustment, out, identifier = "", json;
double significance;
bool decreasing = false, increasing = false, absolute = false;
int minimum, maximum;

std::map<std::string,std::vector<std::pair<std::string, double>>> all_results;
std::map<std::string,std::map<std::string, std::string>> all_name2reference;
std::map<std::string,std::map<std::string, int>> all_name2hits;
std::map<std::string,std::map<std::string, std::string>> all_name2info;
std::map<std::string,std::map<std::string, bool>> all_name2UpRegulated;

bool parseArguments(int argc, char* argv[])
{
	bpo::variables_map vm;
	bpo::options_description desc;

	desc.add_options()("help,h", "Display this message")
		("significance,sig", bpo::value<double>(&significance)->default_value(0.01),"The critical value for rejecting the H0 hypothesis.")
		("categories,gmt", bpo::value<std::string>(&categories)->required(), "A .gmt file containing the categories to be tested.")
		("scores,sco", bpo::value<std::string>(&scores), "A whitespace seperated file containing identifier and scores.")
		("identifier, id", bpo::value<std::string>(&identifier), "A file containing identifier line by line.")
		("decreasing,dec", bpo::value(&decreasing)->zero_tokens(),"Use decreasingly sorted scores (default).")
		("increasing,inc", bpo::value(&decreasing)->zero_tokens(),"Use increasingly sorted scores.")
		("absolute,abs", bpo::value(&absolute)->zero_tokens(),"Use decreasingly sorted absolute scores.")
		("adjustment,adj", bpo::value<std::string>(&adjustment)->default_value("no"),"P-value adjustment method for multiple testing.")
		("minimum,min", bpo::value<int>(&minimum)->default_value(0),"Minimum number of genes allowed in categories.")
		("maximum,max", bpo::value<int>(&maximum)->default_value(1000),"Maximum number of genes allowed in categories.")
		("output,out", bpo::value<std::string>(&out), "Output prefix for text files.")
		("json,j", bpo::value<std::string>(&json),"Output filename of json file.");

	if((decreasing && increasing) || (absolute && increasing) ||
	   (absolute && increasing)) {
		std::cerr << "Error: Please specify only one option to sort the file." << "\n";
	}

	try
	{
		bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(),
		           vm);
		bpo::notify(vm);
	}
	catch(bpo::error& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		desc.print(std::cerr);
		return false;
	}
	return true;
}

std::vector<std::string> parseTestSet(){
	GeneSetReader reader;
	std::vector<std::string> test_set;
	if( scores != "" ){
		ScoringFile<double> sf = reader.readScoringFile<double>(scores);
		if(absolute) {
			test_set = sf.getIdentifier(sf.getAbsoluteSortedScores());
		} else {
			if(increasing) {
				test_set = sf.getIdentifier(sf.getIncreasinglySortedScores());
			} else {
				test_set = sf.getIdentifier(sf.getDecreasinglySortedScores());
			}
		}
	}else if(identifier != ""){
		test_set = reader.readGeneList(identifier);
	}
	return test_set;
}

int main(int argc, char* argv[])
{
	if(! parseArguments(argc, argv)){
		return -1;
	}

	if(scores != "" && identifier != ""){
		std::cerr << "ERROR: Please specify only one input file." << "\n";
	}else if (scores == "" && identifier == ""){
		std::cerr << "ERROR: Please specify a input file." << "\n";
	}

	auto test_set = parseTestSet();

	std::ifstream cat(categories);
	if(!cat) {
		std::cerr << "ERROR: Could not open " << categories << " for reading." << std::endl;
	}

	for(std::string line; getline(cat, line);) {

		std::vector<std::pair<std::string, double>> results;
		std::map<std::string, std::string> name2reference;
		std::map<std::string, int> name2hits;
		std::map<std::string, std::string> name2info;
		std::map<std::string, bool> name2UpRegulated;

		std::vector<std::string> strs;
		boost::split(strs,line,boost::is_any_of("\t"));

		std::string categoryName = strs[0];
		std::string category = strs[1];

		// Compute the enrichment
		GMTFile input(category);
		while(input) {
			Category c = input.read();
			std::cout << "INFO: Processing " << c.name() << "." << std::endl;
			int hits  = 0;
			for(auto s : test_set){
				if(c.contains(s)){
					++hits;
				}
			}
			if(minimum <= hits && hits <= maximum){
				name2reference[c.name()] = c.reference();
				name2hits[c.name()] = hits;

				GeneSetEnrichmentAnalysis<cpp_dec_float_50, int64_t> gsea;
				auto enr = gsea.computePValue(c, test_set);
				int max=0;
				std::string points = "[[0,0]";
				for(auto p : enr.second) {
					max = std::abs(p.second) > std::abs(max)? p.second : max;
					points += ",[" + boost::lexical_cast<std::string>(p.first) +
					          "," + boost::lexical_cast<std::string>(p.second) +
					          "]";
				}
				points += ",[" + boost::lexical_cast<std::string>(test_set.size()) + ",0]]";
				name2UpRegulated[c.name()] = max > 0;

				name2info[c.name()] = points;

				results.emplace_back(c.name(), enr.first.convert_to<double>());
			}
		}

		// Sort p-values
		std::sort(results.begin(), results.end(),
		          [](const std::pair<std::string, double>& a,
		             const std::pair<std::string, double>& b) {
			return a.second < b.second;
		});

		std::vector<std::pair<std::string, double>> adj = pvalue<double>::adjustPValues(results, adjustment);

		all_results[categoryName] = adj;
		all_name2reference[categoryName] = name2reference;
		all_name2hits[categoryName] = name2hits;
		all_name2info[categoryName] = name2info;
		all_name2UpRegulated[categoryName] = name2UpRegulated;
	}

	std::ofstream myfile;
	myfile.open (json);

	for(auto p : all_results){
		myfile << p.first << "\t\"" + p.first + "\": [";
		int n = all_results[p.first].size();
		for(int i = 0; i < n; ++i) {
			if(all_results[p.first][i].second <= significance) {
				myfile << "{\"name\":\"" << all_results[p.first][i].first
				       << "\", \"reference\":\"" << all_name2reference[p.first][all_results[p.first][i].first]
				       << "\", \"hits\":" << all_name2hits[p.first][all_results[p.first][i].first]
				       << ", \"pvalue\":" << all_results[p.first][i].second
				       << ", \"info\":" << all_name2info[p.first][all_results[p.first][i].first];
				if(i+1 < n && all_results[p.first][i + 1].second <= significance) {
					myfile << "},";
				} else {
					myfile << "}";
				}
			}
		}
		myfile << "]\n";

		std::ofstream myfile2;
		myfile2.open (out + "." + p.first + ".txt");
		for(int i = 0; i < n; ++i) {
			if(all_results[p.first][i].second <= significance) {
				myfile2 << all_results[p.first][i].first << "\t" << all_name2reference[p.first][all_results[p.first][i].first] << "\t" << all_name2hits[p.first][all_results[p.first][i].first] << "\t" << all_results[p.first][i].second << "\t" << all_name2UpRegulated[p.first][all_results[p.first][i].first] << "\n";
			}
		}
		myfile2.close();
	}

	myfile.close();

	return 0;
}
