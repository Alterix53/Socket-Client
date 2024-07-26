#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include "FileDownload.h"
using namespace std;

fileZip getFileInfo(string fileInfo) {

	// use stringstream for easier detach
	stringstream file(fileInfo);
	fileZip fileMake;
	string temp = "";

	// split the file name from the string
	getline(file, temp, ' ');
	fileMake.name = temp;

	// get the size and size type from the file
	fileMake.size = 0;
	getline(file, temp);
	int loop = 0;			// variable hold the position of the loop

	// detach the size from the temp string
	for (int i = 0; i < (int)temp.length() && isdigit(temp[i]); i++) {
		fileMake.size = fileMake.size * 10 + (temp[i] - '0');
		loop++;
	}

	// after the loop is complete
	// the loop variable points to the first position of the file type
	fileMake.sizeType = temp.substr(loop, temp.length() - loop + 1);

	return fileMake;
}

// doc file input.txt de client, kiem tra cac tep them vao va tai ve tep moi
void readInputFile(string filename, vector<fileZip>& file, int alreadyDownload) {
	fstream ifs;
	ifs.open(filename);

	// exit 1: khong the mo file
	if (!ifs) {
		cerr << "Can not open the file!" << filename << "!" << endl;
		exit(1);
	}

	// bo qua cac file da doc
	string temp;
	for (int i = 0; i < alreadyDownload; i++) {
		getline(ifs, temp);
	}

	// lay cac file moi
	while (!ifs) {
		string s;
		getline(ifs, s);

		fileZip temp = getFileInfo(s);

		if (temp.name != "")
			file.push_back(temp);
	}
	ifs.close();
}

int checkIfDownloaded(vector<fileZip> downable, string filename) {
	// return 1 if the file is already downloaded on the computer
	// return 0 if the file 
	// return -1 if the file is not exist on server's list file

	int downloadable = false;

	for (int i = 0; i < downable.size(); i++) {
		if (filename != downable[i].name) {
			continue;
		}

		downloadable = true;
		ifstream ifs;

		// try to open the file. 
		// The file is already on the client's computer if the file can be opened.
		ifs.open(filename);
		if (ifs) {
			ifs.close();
			return 1;
		}
	}

	if (downloadable)
		return 0;

	return -1;
}

// print the percentage of the current download file
void printPercent(long long size, long long currentSize, int length_file, int num_of_file) {
	double p = (currentSize / double(size)) * 100;
	GoToXY(length_file + 20, num_of_file + 1);
	cout << fixed << setprecision(2) << p << " %                              ";
	Sleep(0.1f);
}

// download each chunk in a file and write into destination file in append mode
void downChunk(string filename, vector <char> data, int size) {
	ofstream ofs;
	ofs.open(filename, ios::out | ios::binary | ios::app);
	if (!ofs) {
		cout << "There are problems while downloading this file!" << endl;
		return;
	}
	ofs.write(data.data(), size);
	ofs.close();
}

void downFile(CSocket& client, string filename) {
	bool downloading = true;		// check if file is downloading or not
	long long allSize;				// size of the downloading file

	// server get the size of the file and send, client receive the data
	client.Receive(&allSize, sizeof(allSize), 0);

	int chunk_size; // size of one chunk
	long long currentSize = 0; // hold the size of the data 
	bool check;

	while (downloading) {
	save_point:
		client.Receive(&chunk_size, sizeof(chunk_size), 0);


		currentSize += long long(chunk_size);

		vector<char> chunk(chunk_size); // buffer to save chunk data

		client.Receive(chunk.data(), chunk_size, 0);

		if (chunk.size() == chunk_size) {
			downChunk(filename, chunk, chunk_size);
			check = true;
			client.Send(&check, sizeof(check), 0);
		}
		else {
			check = false;
			client.Send(&check, sizeof(check), 0);
			goto save_point;
		}

		printPercent(allSize, currentSize, filename.length(), 8);

		// server send signal if file is downloading or not
		// download = false: the file finish 
		client.Receive(&downloading, sizeof(downloading), 0);
	}
}