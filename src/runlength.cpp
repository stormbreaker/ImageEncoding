#include "runlength.h"


const int TOLERANCERANGE = 4; // this is a decent change across all three channels

enum colors {BLUE, GREEN, RED};

unsigned char** compressedImage = NULL;

void writeToStream(unsigned char count, unsigned char rgbValues, vector<unsigned char>& channel)
{
	channel.push_back(count);
	channel.push_back(rgbValues);
	//cout << channel.size() << endl;
}

void writeChannelToFile(vector<unsigned char> channel)
{
	ofstream fout;
	unsigned char* channelArray = &channel[0];

	fout.open("testimage.bin", ios::out | ios::binary | ios::app);
	//cout << "in write to file" << channel.size() << endl;
	fout.write((char*) channelArray, channel.size());
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
	cout << "wrote\n";
	fout.close();
}

void runlengthEncodeRange(Mat image, int height, int width)
{
	unsigned char currentIntensity = 0;

	unsigned char count;
	int numberOfChannels;
	char lossless;
	int numberOfBits;

	vector<unsigned char> channels[3];

	if (image.type() == CV_8UC1)
	{
		numberOfChannels = 1;
		numberOfBits = 8;
		cout << "hi" << endl;
		//return;
	}
	else
	{
		numberOfBits = 24;
		numberOfChannels = 3;
	}

	writeHeader(height, width, "png", numberOfBits, 0);


	for (int channel = 0; channel < numberOfChannels; channel++)
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
					writeToStream(count, currentIntensity, channels[channel]);
					count = 0;
					currentIntensity = image.at<Vec3b>(rowIndex, columnIndex)[channel];
				}
				if (count == 255)
				{
					writeToStream(count, currentIntensity, channels[channel]);
					count = 0;
				}
			}
		}
		writeToStream(count, currentIntensity, channels[channel]);
		writeChannelToFile(channels[channel]);
	}
}

//http://docs.opencv.org/trunk/d4/d32/classcv_1_1__InputArray.html


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

	int numberOfChannels;
	Mat newImage;
	//cout << width << " " << height << " " << filetype << " " << bits << " " << lossless << endl;

	if (bits == 24)
	{
		newImage = Mat(height, width, CV_8UC3);
		numberOfChannels = 3;		
	}
	else if (bits == 8)
	{
		newImage = Mat(height, width, CV_8UC1);
		numberOfChannels = 1;
	}

	pixelsInImage = width * height;

	
	int tempRows, tempCols, tempCount;

	
	for (int channel = 0; channel < numberOfChannels; channel++)
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
	namedWindow("Lossy", WINDOW_AUTOSIZE);
	imshow("Lossy", newImage);
	waitKey(0);
}
