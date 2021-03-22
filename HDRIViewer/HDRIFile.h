#pragma once

#include <vector>

class HRDIFile
{
public:
	HRDIFile();
	~HRDIFile();
public: 
	bool Load(const char* FileName);

	int Width() const { return ImageWidth; }
	int Height() const { return ImageHeight; }
private:
	std::vector<float> Data;
	int ImageWidth, ImageHeight;
};