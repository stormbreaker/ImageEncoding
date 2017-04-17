#include "runlength.h"


const int TOLERANCERANGE = 4; // this is a decent change across all three channels

enum colors {BLUE, GREEN, RED};

unsigned char** compressedImage = NULL;

void writeToStream(unsigned char count, unsigned char rgbValues)
{
	ofstream fout;
	fout.open("testimage.bin", ios::binary | ios::out | ios::app);

	fout.write((char*)&count, 1);
	fout.write((char*)&rgbValues, 1);
	// write all three bytes
	//cout << (int)count << " " << (int) rgbValues << endl;

	fout.close();
}

void writeHeader(int height, int width, char* filetype, int bits, char lossless)
{
	// this function should essentially write all of the information necessary for the decoder
	// to operate.  This include the width, height, type of file
	ofstream fout;
	fout.open("testimage.bin", ios::out | ios::binary);
	fout.write((char*)&height, 4);
	fout.write((char*)&width, 4);
	fout.write(filetype, 3);
	fout.write((char*)&bits, 4);
	fout.write((char*)&lossless, 1);

	fout.close();
}

void runlengthEncodeRange(Mat image, int height, int width)
{
	unsigned char currentIntensity = 0;

	unsigned char count;

	writeHeader(height, width, "png", 24, 0);


	for (int channel = 0; channel < 3; channel++)
	{
		currentIntensity = image.at<Vec3b>(0, 0)[channel];
		count = 0;
		for (int rowIndex = 0; rowIndex < height; rowIndex++)
		{
			for (int columnIndex = 0; columnIndex < width; columnIndex++)
			{
				count++;				
				if (abs(image.at<Vec3b>(rowIndex, columnIndex)[channel] - currentIntensity) > TOLERANCERANGE)
				{
					writeToStream(count, currentIntensity);
					count = 0;
					currentIntensity = image.at<Vec3b>(rowIndex, columnIndex)[channel];
				}
				if (count == 255)
				{
					writeToStream(count, currentIntensity);
					count = 0;
				}
			}
		}
		writeToStream(count, currentIntensity);
	}
}

/*
void runlengthDecodeRange(char** compressedBytes, bool isGrayscale)
{
	// read in width, height, file type and whether or not is grayscale 
	// use width to count number of pixels to read.  will need a temporary column counter
	// read in the pairs
	int width, height;
	string filetype;
	bool grayscaleFlag;
	ifstream fin;
	int startOfImage;
	//char count[1];
	//char rgb[3];
	unsigned char count;
	unsigned char rgb[3];

	int pixelCount = 0;

	fin.open("testimage.bin", ios::in);
	fin >> width >> height >> filetype >> grayscaleFlag;
	//cout << width << height << filetype << grayscaleFlag << endl;
	startOfImage = fin.tellg() + 1;
	fin.close();
	fin.open("testimage.bin", ios::in | ios::binary);
	fin.seekg(0, fin.end);
	int fileLength = fin.tellg();
	//fin.seekg(0, fin.beg);
	fin.seekg(startOfImage, fin.beg);
	//cout << "file length: " << fileLength << endl;
	while (fin >> count >> rgb[0] >> rgb[1] >> rgb[2])
	{
		cout << (int)count << " " << (int)rgb[0] << " " << (int)rgb[1] << " " << (int)rgb[2] << endl; 
		//fin.read(count, 1);
		//fin.read(rgb, 3);
		pixelCount++;
		//cout << count << " " << pixelCount << endl;
	}
	fin.close();

	//http://docs.opencv.org/trunk/d4/d32/classcv_1_1__InputArray.html
}
*/

void runlengthDecodeRange()
{
	int width, height;
	int pixelsInImage;
	char filetype[3];
	int bits;
	char lossless;

	ifstream fin;

	int pixelCount = 0;

	unsigned char count;
	unsigned char pixelValue;

	fin.open("testimage.bin", ios::in | ios::binary);

	fin.seekg(0, fin.end);
	int fileLength = fin.tellg();
	fin.seekg(0, fin.beg);

	fin.read((char*)&height, 4);
	fin.read((char*)&width, 4);
	fin.read(filetype, 3);
	fin.read((char*)&bits, 4);
	fin.read((char*)&lossless, 1);
	//cout << width << " " << height << " " << filetype << " " << bits << " " << lossless << endl;

	pixelsInImage = width * height;
	Mat newImage(height, width, CV_8UC3);
	
	int tempRows, tempCols, tempCount;

	
	for (int channel = 0; channel < 3; channel++)
	{
		tempCols = 0;
		tempRows = 0;
		int sum = 0;
		while (pixelCount < pixelsInImage)
		{
			fin.read((char*)&count, 1);
			fin.read((char*)&pixelValue, 1);

			//cout << (int) count << endl;
			
			tempCount = 0;
			sum += count;
			while(tempCount < count)
			{
				Vec3b temp = newImage.at<Vec3b>(tempRows, tempCols);
				temp[channel] = pixelValue;
				newImage.at<Vec3b>(tempRows, tempCols) = temp;
				tempCount++;
				tempCols++;
				if (tempCols == width)
				{
					tempCols = 0;
					tempRows++;
				}
			}
			pixelCount += count;
		}
		pixelCount = 0;
		//cout << "total " << sum << " " << pixelsInImage << endl;
	}

	fin.close();
	namedWindow("Why so Serious?", WINDOW_AUTOSIZE);
	imshow("Why so Serious?", newImage);
	waitKey(0);
}
