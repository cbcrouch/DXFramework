//
// File:     WavefrontObj.cpp
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "WavefrontObj.h"

#include <iostream>

#include <algorithm>
#include <functional>
#include <regex>

#include <fstream>
#include <istream>
#include <sstream>

template <typename T>
inline void copyData(std::shared_ptr<T> destPtr, const T src);

//
// TODO: would be a very interesting to benchmark a functor against a corresponding virtual function implemenation
//       as well as including a comparison against using a C++ regular expression
//
template<typename T> class splitfunctor
{
public:
	explicit splitfunctor(std::string delim = " ") : _delimiter(delim) { }
	std::vector<T> operator()(std::string _splitstr) const
	{
		//
		// TODO: update functor to use sregex_token_iterator and generate regex from string delimiter passed in
		//       (would then create two functors, one would contain regex for spaces the other for forward slashes)
		//

		std::istringstream iss(_splitstr);
		std::vector<T> vec = std::vector<float>(std::istream_iterator<T>(iss), std::istream_iterator<T>());
		return vec;
	}
private:
	// default assignment operator to silence warning C4512
	splitfunctor& operator=(const splitfunctor&);
	std::string _delimiter;
};

void WavefrontObj::setFileNamePath(std::string fileNamePath)
{
	//
	// TODO: should use the assign function or is the assignment operator overloaded to perform the same functionality ??
	//       (not sure which is the perferred practice for modern C++)
	//
	this->m_fileNamePath = fileNamePath;
	std::ifstream ifs(this->m_fileNamePath, std::ifstream::in);

	// will allocate all the memory upfront
	ifs.seekg(0, std::ios::end);
	// NOTE: tellg returns type streampos (which is an fpos) so it should be safe to static cast to an integral type

	//unsigned int fileSize = static_cast<unsigned int>(ifs.tellg());
	//std::cout << "fileSize: " << fileSize << std::endl;

	this->m_fileSource.reserve(static_cast<unsigned int>(ifs.tellg()));
	//std::cout << "fileSize: " << ifs.tellg() << std::endl;

	ifs.seekg(0, std::ios::beg);

	// NOTE: extra parans around first param to prevent "most vexing parse" i.e. parans force the compiler to evaluate the
	//       iterator to a string declaration and pass by value rather than evaluating and passing as a function declaration
	this->m_fileSource.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	// clost input file stream
	ifs.close();
}

void WavefrontObj::parseFileContents(std::string fileContents)
{
	auto split = [](std::string str) -> std::vector<std::string> {
		std::istringstream issline(str);
		auto vec = std::vector<std::string>(
			std::istream_iterator<std::string, char>(issline),
			std::istream_iterator<std::string, char>());
		return vec;
	};

	std::string prefix;
	std::istringstream iss(fileContents, std::ios_base::in);
	for (std::string line; std::getline(iss, line);) {

		//
		// TODO: use vector reserve function where possible to cut down on the number of allocs
		//

		// get prefix of line to check for valid lines
		prefix = line.substr(0, 2);

		if (prefix.compare("v ") == 0) { // parse vertex
			// remove the prefix
			std::string truncLine = line.substr(2, line.length());

			// split using functor
			//auto funcVec = splitFunction<float>(" ")(trunline);

			//
			// TODO: verify that a lambda expression simply creates a nameless functor (i.e. it's syntactic sugar)
			//       in which case a lambda should only be used as a shorthand for one-off functors though either
			//       way both work good for callback functions
			//

			// split using lambda
			auto vecStr = split(truncLine); // NOTE: a lambda with an empty capture is analogous to a function pointer
			std::vector<float> vec;

			// parse individual vertex elements and store in a vector
			for_each(std::begin(vecStr), std::end(vecStr), [&vec](const std::string str) {
				// convert split string values into vertices (floating point values)
				std::stringstream ss(str, std::ios_base::in);
				float val = 0.0f;
				if ((ss >> val).fail()) {
					std::cerr << "ERROR: wsringstream failed to convert face index string into an int" << std::endl;
					//
					// TODO: throw exception, this is a true failure and not just an error condition
					//
				}

				vec.push_back(val);
			});

			// push vertex into the back of the vertices vector
			this->m_vertices.push_back(vec);
		}
		else if (prefix.compare("f ") == 0) { // parse face
			// remove prefix
			std::string truncLine = line.substr(2, line.length());

			//
			// TODO: use split lambda to separate indices by whitespace
			//

			// separate indices by whitespace
			std::istringstream issline(truncLine);
			auto vec = std::vector<std::string>(std::istream_iterator<std::string, char>(issline),
				std::istream_iterator<std::string, char>());

			// first face string contains vertex indices
			std::string faceStr = vec.at(0);

			std::regex ws_re("\\s+"); // whitespace
			std::regex fs_re("/+"); // forward slash

			auto regexitr = std::sregex_token_iterator(std::begin(faceStr), std::end(faceStr), fs_re, -1);
			auto indexVec = std::vector<std::string>(regexitr, std::sregex_token_iterator());

			// parse index triplet
			std::vector<unsigned int> triplet;
			const size_t numVertices = this->m_vertices.size();
			for_each(std::begin(indexVec), std::end(indexVec), [&triplet, numVertices](const std::string face) {
				std::stringstream ss(face, std::ios_base::in);
				int num = 0;
				if ((ss >> num).fail()) {
					std::cerr << "ERROR: wstringstream failed to convert face index string into an int" << std::endl;
					// TODO: throw exception, this is a true failure and not just an error condition
				}

				// TODO: explain how final index value is being derived
				triplet.push_back((unsigned int)((num > 0) ? num - 1 : num + numVertices));
			});

			this->m_indices.push_back(triplet);
		}
	}
}

void WavefrontObj::parseAsset()
{
	this->parseFileContents(m_fileSource);
}

template <typename T>
inline void copyData(std::shared_ptr<T> destPtr, const T src)
{
	destPtr->reserve(sizeof(T)* src.size());
	std::copy(std::begin(src), std::end(src), std::back_inserter(*destPtr));
}

std::shared_ptr<VERTEX_VEC> WavefrontObj::getVertices()
{
	std::shared_ptr<VERTEX_VEC> ret(new VERTEX_VEC());
	copyData<VERTEX_VEC>(ret, this->m_vertices);
	return ret;
}

std::shared_ptr<INDEX_VEC> WavefrontObj::getIndices()
{
	std::shared_ptr<INDEX_VEC> ret(new INDEX_VEC());
	copyData<INDEX_VEC>(ret, this->m_indices);
	return ret;
}
