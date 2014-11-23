//
// File:     WavefrontObj.h
// Project:  DXFramework
//
// Copyright (c) 2014 Casey Crouch. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
#include <memory>


//
// TODO: add namespace for asset pipeline (use DXAP)
//

// namespace DXAP // DirectX Asset Pipeline


typedef struct Vertex_t {
	float pos[4];
	float norm[4];
	float texCoord[3];
} Vertex_t;

typedef unsigned short Index_t;

typedef std::vector<std::vector<float>> VERTEX_VEC;
typedef std::vector<std::vector<unsigned int>> INDEX_VEC;

//
// TODO: need to build an asset pipeline that converts assets to an entity
//


class FileData
{
public:
	void setFileNamePath(std::string fileNamePath);
	void loadFileIntoMemory();

	//std::string getTextData();
	//char* getRawData();

private:

	//FILE hFile;
	//std::fstream fileStream;
};


//
// TODO: try using "dependencies upon anonymous interfaces from templates" instead
//       of an abstract class
//
class IAsset
{
public:

	//
	// TODO: should use similiar definitions and interfaces as SDKMesh
	//

	//virtual void parseFileData(FileData& fileData);

	virtual std::shared_ptr<VERTEX_VEC> getVertices();
	virtual std::shared_ptr<INDEX_VEC> getIndices();
};

// http://stackoverflow.com/questions/8715630/implementing-interfaces-in-c

class WavefrontObj // : public virtual IAsset
{
public:
	WavefrontObj() { }
	~WavefrontObj() { }

	//
	// TODO: should remove the file reading capability of the class and instead parse
	//       the raw bytes (wchar_t or char ??)
	//
	void setFileNamePath(std::string fileNamePath);
	void parseAsset();

	void parseFileContents(std::string fileContents);

	std::shared_ptr<VERTEX_VEC> getVertices();
	std::shared_ptr<INDEX_VEC> getIndices();

private:
	std::string m_fileNamePath;
	std::string m_fileSource;

	VERTEX_VEC m_vertices;
	INDEX_VEC m_indices;
};
