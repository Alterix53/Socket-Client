
#include "FileDownload.h"
using namespace std;

void printServerFile(vector <fileZip> ServerFile) {
	cout << "Current available files on server: " << endl;
	for (int i = 0; i < ServerFile.size(); i++) {
		cout << ServerFile[i].name << " " << ServerFile[i].size << ServerFile[i].sizeType << endl;
	}
	cout << "----------------------------------------------" << endl;
}

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
	ifstream ifs;
	ifs.open(filename);

	// exit 1: khong the mo file
	if (!ifs) {
		cerr << "Can not open the file " << filename << "!" << endl;
		exit(1);
	}

	// bo qua cac file da doc
	string temp;
	for (int i = 0; i < alreadyDownload; i++) {
		getline(ifs, temp);
	}

	// lay cac file moi
	while (!ifs.eof()) {
		string s;
		getline(ifs, s);

		fileZip temp = getFileInfo(s);

		if (temp.name != "")
			file.push_back(temp);
	}
	ifs.close();
}

int checkIfDownloaded(vector<fileZip> ServerFile, string filename) {
	// 0: can't download (not exist on server)
	// 1: can be downloaded
	// 2: file is already on client's pc
	const string FilePath = "output/" + filename;
	int isDownloadable = false;

	for (int i = 0; i < ServerFile.size(); i++) {
		if (filename != ServerFile[i].name) {
			continue;
		}

		// file did appear on server
		isDownloadable = true;

		// try to open the file. 
		// The file is already on the client's computer if the file can be opened.
		ifstream ifs;
		ifs.open(FilePath);
		if (ifs) {
			ifs.close();
			return 2;
		}
		else
			return 1;
	}

	return 0;
}

// print the percentage of the current download file
void printPercent(long long size, long long currentSize, int coordY) {
	double p = (currentSize / double(size)) * 100;
	GoToXY(50, coordY);
	cout << fixed << setprecision(2) << p << " %";
	Sleep(0.5f);
}

// download each buffer in a file and write into destination file in append mode
void DownloadBuffer(string filename, vector <char> data, int size) {
	ofstream ofs;
	ofs.open(filename, ios::out | ios::binary | ios::app);
	if (!ofs) {
		cerr << "There are problems while isFinished this file!" << endl;
		return;
	}
	ofs.write(data.data(), size);
	ofs.close();
}

void downFile(CSocket& client, string filename, int coordY) {
	const string path = "output/" + filename;
	long long allSize;				// size of the file

	// server get the size of the file and send, client receive the data
	client.Receive(&allSize, sizeof(allSize), 0);

	int bufferSize = 1024 * 128;						// size of one buffer
	long long currentSize = 0;			// hold the size of the data 
	// bool isDownloadCorrectly = true;


	bool isFinished = false;			// check if the file finish download or not
	while (currentSize < allSize) {
		// get the  buffer data from server
		vector <char> buffer(bufferSize); 
		int count = client.Receive(buffer.data(), bufferSize, 0);
		currentSize += count;				// for printing percentage

		// check and download the file if the sending data is correct
		if (buffer.size() > 0) {
			DownloadBuffer(path, buffer, count);
		}
		else {
			break;
		}
		
		printPercent(allSize, currentSize, coordY);

		// server send signal if file is finished or not
		// isFinished = true: the file finish 
		//client.Receive(&isFinished, sizeof(isFinished), 0);
	}

	cout << "  Download complete!" << endl;
}