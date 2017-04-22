#include "runlength.h"


const int TOLERANCERANGE = 128; // this is a decent change across all three channels

enum colors {BLUE, GREEN, RED};

unsigned char** compressedImage = NULL;

void writeToStream(unsigned char count, unsigned char rgbValues, vector<unsigned char>& channel)
{
	channel.push_back(count);
	channel.push_back(rgbValues);
	cout << (int) count << " " << (int) rgbValues << endl;
	//cout << channel.size() << endl;
}

void writeToStreamWithoutIntensity(unsigned char count, vector<unsigned char>& channel)
{
	channel.push_back(count);
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
	cout << "wrote " << (int)height << " " << (int)width << " " << (int)bits << " " << (int) lossless << endl;;
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

//	namedWindow("Lossy", WINDOW_AUTOSIZE);
	//imshow("Lossy", image);
	//waitKey(0);

	writeHeader(height, width, "png", numberOfBits, 0);


	for (int channel = 0; channel < numberOfChannels; channel++)
	{
		currentIntensity = image.at<Vec3b>(0, 0)[channel];

		count = 0;
		for (int rowIndex = 0; rowIndex < height; rowIndex++)
		{
			for (int columnIndex = 0; columnIndex < width; columnIndex++)
			{
				//cout << (int)currentIntensity << endl;

				
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
				count++;
			}
		}
		writeToStream(count, currentIntensity, channels[channel]);
		writeChannelToFile(channels[channel]);
	}
}

void newEncodeRunlength(Mat image, int height, int width)
{
	
}

//http://docs.opencv.org/trunk/d4/d32/classcv_1_1__InputArray.html

void runlengthEncodeBitPlane(Mat image, int height, int width)
{
	unsigned char currentIntensity = 0;

	unsigned char mask = 1;

	unsigned char count;
	int numberOfChannels;
	int numberOfBits;
	unsigned char currentPlaneValue = 0;

	vector<vector<unsigned char>> channels[3];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			vector<unsigned char> temp;
			channels[i].push_back(temp);
		}
	}

	if (image.type() == CV_8UC1)
	{
		numberOfChannels = 1;
		numberOfBits = 8;
	}
	else
	{
		numberOfBits = 24;
		numberOfChannels = 3;
	}

	writeHeader(height, width, "png", numberOfBits, 1);

	int sum = 0;

	for (int channel = 0; channel < numberOfChannels; channel++)
	{
		mask = 1;
		for (int currentPlane = 0; currentPlane < 8; currentPlane++)
		{
			count = 0;
			currentIntensity = image.at<Vec3b>(0, 0)[channel];
			currentPlaneValue = currentIntensity & mask;
			writeToStreamWithoutIntensity(currentPlaneValue, channels[channel][currentPlane]);
			for (int rowIndex = 0; rowIndex < height; rowIndex++)
			{
				for (int columnIndex = 0; columnIndex < width; columnIndex++)
				{
					if (currentPlaneValue != (image.at<Vec3b>(rowIndex, columnIndex)[channel] & mask))
					{
						//writeToStream(count, currentPlaneValue, channels[channel][currentPlane]);
						writeToStreamWithoutIntensity(count, channels[channel][currentPlane]);
						sum += count;
						count = 0;
						currentPlaneValue = image.at<Vec3b>(rowIndex, columnIndex)[channel] & mask;
					}
				
					if (count == 255)
					{
						//writeToStream(count, currentPlaneValue, channels[channel][currentPlane]);
						writeToStreamWithoutIntensity(count, channels[channel][currentPlane]);
						writeToStreamWithoutIntensity(0, channels[channel][currentPlane]);
						sum += count;
						count = 0;
					}
					count++;
				}
			}
	
			mask = mask << 1;
			//writeToStream(count, currentPlaneValue, channels[channel][currentPlane]);
			writeToStreamWithoutIntensity(count, channels[channel][currentPlane]);
			sum += count;
			writeChannelToFile(channels[channel][currentPlane]);
		}
	}
	//cout << sum << endl;
}

Mat runlengthDecodeBitPlane()
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

	fin.read((char*) &height, 4);
	fin.read((char*) &width, 4);
	fin.read(filetype, 3);
	fin.read((char*)&bits, 4);
	fin.read((char*)&lossless, 1);

	int numberOfChannels;
	Mat newImage;

	if (bits == 24)
	{
		newImage = Mat(height, width, CV_8UC3, cvScalar(0));
		numberOfChannels = 3;
	}
	else if (bits == 8)
	{
		newImage = Mat(height, width, CV_8UC1);
		numberOfChannels = 1;
	}

	pixelsInImage = width * height;

	int tempRows, tempCols, tempCount;

	unsigned char mask = 1;
	unsigned char startingVal;
	bool isSet;

	for (int channel = 0; channel < numberOfChannels; channel++)
	{
		int sum = 0;
		mask = 1;
		for (int bitPlane = 0; bitPlane < 8; bitPlane++)
		{
			tempCols = 0;
			tempRows = 0;
			fin.read((char*) &startingVal, 1);
			if (startingVal != 0)
			{
				isSet = true;
			}
			else
			{
				isSet = false;
			}
			while (pixelCount < pixelsInImage)
			{
				fin.read((char*) &count, 1);

				//fin.read((char*) &pixelValue, 1);

				tempCount = 0;
				sum += count;
				for (tempCount = 0; tempCount < count; tempCount++)
				{
					Vec3b temp = newImage.at<Vec3b>(tempRows, tempCols);
					if (isSet)
					{
						temp[channel] = temp[channel] | mask;  //pixelValue | temp[channel];
					}
				// else then or with 0
					else
					{
						temp[channel] = temp[channel] | 0;
					}
					newImage.at<Vec3b>(tempRows, tempCols) = temp;
					tempCols++;
					if (tempCols == width)
					{
						tempCols = 0;
						tempRows++;
					}
				}
				/*
				if (count == 0)
				{
					tempCols++;
					if (tempCols == width)
					{
						tempCols = 0;
						tempRows++;
					}
				}*/
				pixelCount += count;
				isSet = !isSet;
				//cout << pixelCount << endl;
			}
			mask = mask << 1;
			//cout << "pix " << pixelCount << endl;
			pixelCount = 0;
		}
	}

	fin.close();
	namedWindow("Lossy", WINDOW_AUTOSIZE);
	imshow("Lossy", newImage);
	waitKey(0);	
	//imwrite("/home/student/7285523/csc442final/decoded.png", newImage);
	return newImage;
}

Mat runlengthDecodeRange()
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

			cout << "decode " << (int) pixelValue << endl;

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
		cout << pixelCount << endl;
		pixelCount = 0;
		//cout << "total " << sum << " " << pixelsInImage << endl;
	}

	fin.close();
	/*
	namedWindow("Lossy", WINDOW_AUTOSIZE);
	imshow("Lossy", newImage);
	waitKey(0);
	*/
	imwrite("/home/student/7285523/csc442final/decodedtest.png", newImage);
	return newImage;
}
