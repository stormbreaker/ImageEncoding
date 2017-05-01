/*
	Author: Benjamin Kaiser
	Description:  This file contains all of the implementations of the functions required to operate
	the runlength encoder and decoder.  
*/
#include "runlength.h"
#include "../statistics/statistics.h"

/*
	Author: Benjamin Kaiser
	Description:  This function is a helper function for the range encoder.  It takes
	two byte inputes (count and rgbValues) and a vector which acts as a temporary storage
	for the compressed data stream.  The two byte values get pushed in succession onto the vector.
*/
void writeToStream(unsigned char count, unsigned char rgbValues, vector<unsigned char>& channel)
{
	channel.push_back(count);
	channel.push_back(rgbValues);
	return;
}

/*
	Author: Benjamin Kaiser
	Description: This function is a helper function for the bitplane encoder.  it takes
	a single byte value (count) and a vector which acts as a temporary storage for the 
	compressed data stream.  The byte value is simply pushed onto the vector.  
*/
void writeToStreamWithoutIntensity(unsigned char count, vector<unsigned char>& channel)
{
	channel.push_back(count);
	return;
}

/*
	Author: Benjamin Kaiser
	Description: This function is a helper function for both the bitplane and the range
	based encoders.  It takes a vector which is an entire channel or stream of bytes
	that need to be written to a file.  The file path is passed as the second parameter
	to the function.  It then converts the vector to a normal array and writes it
	out using binary input.  
*/
void writeChannelToFile(vector<unsigned char> channel, string file)
{
	// file variables
	ofstream fout;

	//convert vector to array since binary output doesn't appear to like vectors
	unsigned char* channelArray = &channel[0];

	// write to file
	fout.open(file, ios::out | ios::binary | ios::app);
	fout.write((char*) channelArray, channel.size());

	fout.close();
}

/*
	Author: Benjamin Kaiser
	Description:  This function is a helper function for both the bitplane and the
	range based encorders.  It takes the width, height, filetype of the original image, the
	number of bits per pixel, whether or not the file is a lossless encoding or not and
	the path to the file that needs to be written.  
	For now, the filetype has been passed hardcoded but is included here for extensibility.
*/
void writeHeader(int height, int width, char* filetype, int bits, char lossless, string file)
{

	ofstream fout;

	fout.open(file, ios::out | ios::binary);

	//write actual header data
	fout.write((char*)&height, 4);
	fout.write((char*)&width, 4);
	fout.write(filetype, 3);
	fout.write((char*)&bits, 4);
	fout.write((char*)&lossless, 1);

	fout.close();
}

/*
	Author: Benjamin Kaiser
	Description:  This function is the main algorithm for the
	runlength encoding based on a range.  It takes an image in the
	OpenCV Matrix object representation form, the height, the width, and
	the path to the original file.  It creates a path for the encoded file
	by appending ".rler" to the original file path.  It also performs an
	error check to see number of bits per pixel the image is and sets things
	accordingly.  Since I am only handling 24 bit images, if the image
	is an 8 bit image, it returns immediately.  This can be fixed later
	by turning this into a wrapper function to call a special function handling 8 bit
	images.  
	The image is then processed by channel (RGB) and encoded into a stream which is written
	to the file path created at the beginning.  
*/
void runlengthEncodeRange(Mat image, int height, int width, string file)
{
	int extensionIndex = file.find_last_of(".");
	string fileType = file.substr(extensionIndex + 1, 3);
	string writeFile = file.replace(extensionIndex, 4, ".rler");// this create the file path to write to
	unsigned char currentIntensity = 0;
	unsigned char count;
	int numberOfChannels;
	//char lossless = 0;
	int numberOfBits;
	int bytesWritten = 0;

	vector<unsigned char> channels[3];

	// check type of image that was passed and set appropriate variables
	if (image.type() == CV_8UC1)
	{
		numberOfChannels = 1;
		numberOfBits = 8;
		return;
	}
	else
	{
		numberOfBits = 24;
		numberOfChannels = 3;
	}

	// create our file for encoding to
	writeHeader(height, width, (char *) fileType.c_str(), numberOfBits, 0, writeFile);

	// process image by channel
	for (int channel = 0; channel < numberOfChannels; channel++)
	{
		currentIntensity = image.at<Vec3b>(0, 0)[channel];
		count = 0;
		//process by pixel
		for (int rowIndex = 0; rowIndex < height; rowIndex++)
		{
			for (int columnIndex = 0; columnIndex < width; columnIndex++)
			{	
				// our run ended so we need to write and switch base intensity for the next run			
				if (abs(image.at<Vec3b>(rowIndex, columnIndex)[channel] - currentIntensity) > TOLERANCERANGE)
				{
					writeToStream(count, currentIntensity, channels[channel]);
					count = 0;
					currentIntensity = image.at<Vec3b>(rowIndex, columnIndex)[channel];
					bytesWritten += 2;
				}
				// since bytes have a maximum value of 255, we need to write to the stream and reset our count
				if (count == 255)
				{
					writeToStream(count, currentIntensity, channels[channel]);
					count = 0;
					bytesWritten += 2;
				}
				count++;
			}
		}
		writeToStream(count, currentIntensity, channels[channel]);
		bytesWritten += 2;
		// actually write our completed channel to the encoded file.  
		writeChannelToFile(channels[channel], writeFile);
	}

    cout << "Compression Rate: " << ( 1.0 - bytesWritten / ( height * width * 3.0) ) * 100.0 << "%" << endl;
}
/*
	Author: Benjamin Kaiser
	Description: This function is the main algorithm for the runlength
	encoding based on bit plane.  It takes an image int he OpenCV Matrix object
	representation form.  It also takes in the height, width, and filepath
	of the original image file.  It then creates a path to write the encoded
	information to.  Then it does a check to see how many bits per pixels
	the image is.  If it is an 8 bit, it returns immediately since
	this function cannot currently handle anything other than 24 bit images.
	The image is then processed by RGB channel followed by processing each channel
	by each bitplane.  
*/
void runlengthEncodeBitPlane(Mat image, int height, int width, string file)
{
	// encoded data file path
	int extensionIndex = file.find_last_of(".");
	string fileType = file.substr(extensionIndex + 1, 3);
	string writeFile = file.replace(extensionIndex, 4, ".rleb");
	unsigned char currentIntensity = 0;

	// mask for encoding
	unsigned char mask = 1;

	// book-keeping variables
	unsigned char count;
	int numberOfChannels;
	int numberOfBits;
	unsigned char currentPlaneValue = 0;
	int bytesWritten = 0;

	vector<vector<unsigned char>> channels[3];

	// initialize the channels to have things otherwise things break
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			vector<unsigned char> temp;
			channels[i].push_back(temp);
		}
	}

	// check to see what type of image it is.  If 8 bit image, return. 
	if (image.type() == CV_8UC1)
	{
		numberOfChannels = 1;
		numberOfBits = 8;
		return;
	}
	else
	{
		numberOfBits = 24;
		numberOfChannels = 3;
	}

	writeHeader(height, width, (char *) fileType.c_str(), numberOfBits, 1, writeFile);

	int sum = 0;

	// process by RGB channels
	for (int channel = 0; channel < numberOfChannels; channel++)
	{
		mask = 1;
		//process by bitplanes
		for (int currentPlane = 0; currentPlane < 8; currentPlane++)
		{
			count = 0;
			currentIntensity = image.at<Vec3b>(0, 0)[channel];
			// get our current bitplane value as either 0 or not 0
			currentPlaneValue = currentIntensity & mask;
			writeToStreamWithoutIntensity(currentPlaneValue, channels[channel][currentPlane]);
			//process by image indices
			for (int rowIndex = 0; rowIndex < height; rowIndex++)
			{
				for (int columnIndex = 0; columnIndex < width; columnIndex++)
				{
					// if the plane value is not the same as the current one, then we need to write
					// to the stream and switch our base value.  
					if (currentPlaneValue != (image.at<Vec3b>(rowIndex, columnIndex)[channel] & mask))
					{
						writeToStreamWithoutIntensity(count, channels[channel][currentPlane]);
						sum += count;
						count = 0;
						bytesWritten++;
						currentPlaneValue = image.at<Vec3b>(rowIndex, columnIndex)[channel] & mask;
					}
					// since we are storing in a byte and the byte has a 255 max value we need to keep
					// track of this so we write the count to file and then immediately write a 0 count for
					// the other value
					if (count == 255)
					{
						writeToStreamWithoutIntensity(count, channels[channel][currentPlane]);
						writeToStreamWithoutIntensity(0, channels[channel][currentPlane]);
						bytesWritten += 2;
						sum += count;
						count = 0;
					}
					count++;
				}
			}
	
			mask = mask << 1;
			writeToStreamWithoutIntensity(count, channels[channel][currentPlane]);
			bytesWritten++;
			sum += count;
			writeChannelToFile(channels[channel][currentPlane], writeFile);
		}
	}
    cout << "Compression Rate: " << ( 1.0 - bytesWritten / ( height * width * 3.0) ) * 100.0 << "%" << endl;
}

/*
	Author: Benjamin Kaiser
	Description:  This function is the main decoding algorithm for the bitplane
	based runlength encoded data stream.  It takes the file path for the image
	that needs to be decoded.  The function then creates the path to writed the
	decoded image to by appending ".png" to it.  It then opens the encoded file
	and just loops through it once putting the data into the proper spots in 
	an OpenCV Matrix object which represents the image.  
*/
Mat runlengthDecodeBitPlane(string filePath)
{
	int width, height;
	int pixelsInImage;
	char filetype[3];
	int bits;
	char lossless;

	ifstream fin;
	int pixelCount = 0;

	unsigned char count;
	//unsigned char pixelValue;

	fin.open(filePath, ios::in | ios::binary);

	int extensionIndex = filePath.find_last_of(".");
	string writeFile = filePath.replace(extensionIndex, 20, "decodedbitplane.png"); // create decoded file path

	// read the header
	fin.read((char*) &height, 4);
	fin.read((char*) &width, 4);
	fin.read(filetype, 3);
	fin.read((char*)&bits, 4);
	fin.read((char*)&lossless, 1);

	int numberOfChannels = 3;
	Mat newImage;

	// check what type of image to create
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

	// calculate the total number of pixels in image
	pixelsInImage = width * height;

	int tempRows, tempCols, tempCount;

	unsigned char mask = 1;
	unsigned char startingVal;
	bool isSet;

	// keep track of what RGB channel we are on
	for (int channel = 0; channel < numberOfChannels; channel++)
	{
		int sum = 0;
		mask = 1;
		// keep track of which bit plane we're on
		for (int bitPlane = 0; bitPlane < 8; bitPlane++)
		{
			tempCols = 0;
			tempRows = 0;

			fin.read((char*) &startingVal, 1);

			// check to see if the starting value for this channel has a bit set or not
			if (startingVal != 0)
			{
				isSet = true;
			}
			else
			{
				isSet = false;
			}

			// loop while we haven't collected enough data to fill up the pixels of the image
			while (pixelCount < pixelsInImage)
			{
				fin.read((char*) &count, 1);

				tempCount = 0;
				sum += count;

				// insert same data into image while we haven't used up all our collected from file count
				for (tempCount = 0; tempCount < count; tempCount++)
				{
					// pull pixel out for easy manipulation
					Vec3b temp = newImage.at<Vec3b>(tempRows, tempCols);
					// if we know a bit should be set, we want to or with the mask to insert new data					
					if (isSet)
					{
						temp[channel] = temp[channel] | mask;
					}
					// else then bitwise or with 0 to keep state of bit
					else
					{
						temp[channel] = temp[channel] | 0;
					}
					// set back into image
					newImage.at<Vec3b>(tempRows, tempCols) = temp;
					tempCols++;
					// check to see if we've filled an entire row
					if (tempCols == width)
					{
						tempCols = 0;
						tempRows++;
					}
				}
				pixelCount += count;
				isSet = !isSet;
			}
			mask = mask << 1;
			pixelCount = 0;
		}
	}

	fin.close();

	imwrite(writeFile, newImage);
	return newImage;
}
/*
	Author: Benjamin Kaiser
	Description:  This function is the main decoding algorithm for the bitplane
	based runlength encoded data stream.  It takes the file path for the image
	that needs to be decoded.  The function then creates the path to writed the
	decoded image to by appending ".png" to it.  It then opens the encoded file
	and just loops through it once putting the data into the proper spots in 
	an OpenCV Matrix object which represents the image.  The difference between this
	algorithm and the other bitplane encoding is that the two lowest bit planes
	are ignored.  
*/
void runlengthEncodeBitPlaneIgnore(Mat image, int height, int width, string file)
{
	// encoded data file path
	int extensionIndex = file.find_last_of(".");
	string fileType = file.substr(extensionIndex + 1, 3);
	string writeFile = file.replace(extensionIndex, 4, ".rleb");
	unsigned char currentIntensity = 0;

	// mask for encoding
	unsigned char mask = 1;

	// book-keeping variables
	unsigned char count;
	int numberOfChannels;
	int numberOfBits;
	unsigned char currentPlaneValue = 0;
	int bytesWritten = 0;

	vector<vector<unsigned char>> channels[3];

	// initialize the channels to have things otherwise things break
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			vector<unsigned char> temp;
			channels[i].push_back(temp);
		}
	}

	// check to see what type of image it is.  If 8 bit image, return. 
	if (image.type() == CV_8UC1)
	{
		numberOfChannels = 1;
		numberOfBits = 8;
		return;
	}
	else
	{
		numberOfBits = 24;
		numberOfChannels = 3;
	}

	writeHeader(height, width, (char *) fileType.c_str(), numberOfBits, 1, writeFile);

	int sum = 0;

	// process by RGB channels
	for (int channel = 0; channel < numberOfChannels; channel++)
	{
		mask = 4;
		//process by bitplanes
		for (int currentPlane = 2; currentPlane < 8; currentPlane++)
		{
			count = 0;
			currentIntensity = image.at<Vec3b>(0, 0)[channel];
			// get our current bitplane value as either 0 or not 0
			currentPlaneValue = currentIntensity & mask;
			writeToStreamWithoutIntensity(currentPlaneValue, channels[channel][currentPlane]);
			//process by image indices
			for (int rowIndex = 0; rowIndex < height; rowIndex++)
			{
				for (int columnIndex = 0; columnIndex < width; columnIndex++)
				{
					// if the plane value is not the same as the current one, then we need to write
					// to the stream and switch our base value.  
					if (currentPlaneValue != (image.at<Vec3b>(rowIndex, columnIndex)[channel] & mask))
					{
						writeToStreamWithoutIntensity(count, channels[channel][currentPlane]);
						sum += count;
						count = 0;
						count++;
						currentPlaneValue = image.at<Vec3b>(rowIndex, columnIndex)[channel] & mask;
					}
					// since we are storing in a byte and the byte has a 255 max value we need to keep
					// track of this so we write the count to file and then immediately write a 0 count for
					// the other value
					if (count == 255)
					{
						writeToStreamWithoutIntensity(count, channels[channel][currentPlane]);
						writeToStreamWithoutIntensity(0, channels[channel][currentPlane]);
						bytesWritten++;
						sum += count;
						count = 0;
					}
					count++;
				}
			}
	
			mask = mask << 1;
			writeToStreamWithoutIntensity(count, channels[channel][currentPlane]);
			bytesWritten++;
			sum += count;
			writeChannelToFile(channels[channel][currentPlane], writeFile);
		}
	}
    cout << "Compression Rate: " << ( 1.0 - bytesWritten / ( height * width * 3.0) ) * 100.0 << "%" << endl;
}

/*
	Author: Benjamin Kaiser
	Description:  This function is the main decode algorithm for the range
	based runlength encoding scheme.  It take a file path for the encoded
	data stream and creates an output file path by appending ".png" to it.
	It then opens the encoded file and loops through it once putting
	the appropriate data into the proper spots in an OpenCV Matrix object
	which represents an image.  It then uses the OpenCV to write the Matrix
	out to an image file so users can view it.  The only difference between
	this function and the other bitplane decoding is that this plane ignores all 
	of the lower two bit planes.  These values are just set to 0 (which
	is handled in the initialization of the image structure).
*/
Mat runlengthDecodeBitPlaneIgnore(string filePath)
{
	int width, height;
	int pixelsInImage;
	char filetype[3];
	int bits;
	char lossless;

	ifstream fin;
	int pixelCount = 0;

	unsigned char count;
	//unsigned char pixelValue;

	fin.open(filePath, ios::in | ios::binary);

	int extensionIndex = filePath.find_last_of(".");
	string writeFile = filePath.replace(extensionIndex, 20, "decodedbitplane.png"); // create decoded file path

	// read the header
	fin.read((char*) &height, 4);
	fin.read((char*) &width, 4);
	fin.read(filetype, 3);
	fin.read((char*)&bits, 4);
	fin.read((char*)&lossless, 1);

	int numberOfChannels = 3;
	Mat newImage;

	// check what type of image to create
	if (bits == 24)
	{
		newImage = Mat(height, width, CV_8UC3, cvScalar(0));
		numberOfChannels = 3;
	}
	else if (bits == 8)
	{
		newImage = Mat(height, width, CV_8UC1, cvScalar(0));
		numberOfChannels = 1;
	}

	// calculate the total number of pixels in image
	pixelsInImage = width * height;

	int tempRows, tempCols, tempCount;

	unsigned char mask = 1;
	unsigned char startingVal;
	bool isSet;

	// keep track of what RGB channel we are on
	for (int channel = 0; channel < numberOfChannels; channel++)
	{
		int sum = 0;
		mask = 4;
		for (int bitPlane = 2; bitPlane < 8; bitPlane++)
		{
			tempCols = 0;
			tempRows = 0;

			fin.read((char*) &startingVal, 1);

			// check to see if the starting value for this channel has a bit set or not
			if (startingVal != 0)
			{
				isSet = true;
			}
			else
			{
				isSet = false;
			}

			// loop while we haven't collected enough data to fill up the pixels of the image
			while (pixelCount < pixelsInImage)
			{
				fin.read((char*) &count, 1);

				tempCount = 0;
				sum += count;

				// insert same data into image while we haven't used up all our collected from file count
				for (tempCount = 0; tempCount < count; tempCount++)
				{
					// pull pixel out for easy manipulation
					Vec3b temp = newImage.at<Vec3b>(tempRows, tempCols);
					// if we know a bit should be set, we want to or with the mask to insert new data					
					if (isSet)
					{
						temp[channel] = temp[channel] | mask;
					}
					// else then bitwise or with 0 to keep state of bit
					else
					{
						temp[channel] = temp[channel] | 0;
					}
					// set back into image
					newImage.at<Vec3b>(tempRows, tempCols) = temp;
					tempCols++;
					// check to see if we've filled an entire row
					if (tempCols == width)
					{
						tempCols = 0;
						tempRows++;
					}
				}
				pixelCount += count;
				isSet = !isSet;
			}
			mask = mask << 1;
			pixelCount = 0;
		}
	}

	fin.close();

	imwrite(writeFile, newImage);
	return newImage;
}

/*
	Author: Benjamin Kaiser
	Description:  This function is the main decode algorithm for the range
	based runlength encoding scheme.  It take a file path for the encoded
	data stream and creates an output file path by appending ".png" to it.
	It then opens the encoded file and loops through it once putting
	the appropriate data into the proper spots in an OpenCV Matrix object
	which represents an image.  It then uses the OpenCV to write the Matrix
	out to an image file so users can view it.  
*/
Mat runlengthDecodeRange(string filePath)
{
	int width, height;
	int pixelsInImage;
	char filetype[4];
	int bits;
	char lossless;

	ifstream fin;

	int pixelCount = 0;

	unsigned char count;
	unsigned char pixelValue;

	fin.open(filePath, ios::in | ios::binary);

	// read in header
	fin.read((char*)&height, 4);
	fin.read((char*)&width, 4);
	fin.read(filetype, 3);
	filetype[3] = 0;
	fin.read((char*)&bits, 4);
	fin.read((char*)&lossless, 1);

	int numberOfChannels = 3;
	Mat newImage;

	string fileType = filetype;

	int extensionIndex = filePath.find_last_of(".");
	Mat oldImage = imread(filePath.replace(extensionIndex, 5, "." + fileType), CV_LOAD_IMAGE_COLOR);

	string writeFile = filePath.replace(extensionIndex, 20, "decodedrange.png"); // create decoded file path

	// check to see what kind of image we should create
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

	// keep track of what RGB we should be working with currently	
	for (int channel = 0; channel < numberOfChannels; channel++)
	{
		tempCols = 0;
		tempRows = 0;
		int sum = 0;
		
		// while we haven't collected enough data to fill an image once
		while (pixelCount < pixelsInImage)
		{
			// read a count and intensity
			fin.read((char*)&count, 1);
			fin.read((char*)&pixelValue, 1);

			tempCount = 0;
			sum += count;
			// loop until we've used up the count that we collected from file
			while(tempCount < count)
			{
				// pull pixel out for easy processing
				Vec3b temp = newImage.at<Vec3b>(tempRows, tempCols);
				temp[channel] = pixelValue;
				// put back in to image
				newImage.at<Vec3b>(tempRows, tempCols) = temp;
				tempCount++;
				tempCols++;
				// check to see if we've filled an entire row
				if (tempCols == width)
				{
					tempCols = 0;
					tempRows++;
				}
			}
			pixelCount += count;
		}
		pixelCount = 0;
	}

	fin.close();

	cout << "Root Mean Square: " << rootMeanSquare(oldImage, newImage) << endl;

	imwrite(writeFile, newImage);
	return newImage;
}
