
#include "FileDownload.h"
#include <chrono>
#include <thread>
using namespace std;
using namespace std::chrono;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool isDownloading = false;

// function that will make the program stop running when meet the ctrl + c 
// only works if the file is not download
void signal_callback_handler(int signum) {
	if (!isDownloading)
	{
		cout << "Caught signal Ctrl + C, exit the program..." << endl;
		
		// close the program
		exit(signum);
	}
}

// The one and only application object

CWinApp theApp;

int main(int argc, TCHAR* argv[], TCHAR* envp[]) {

	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL) {

		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0)) {

			// print error
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else {
			AfxSocketInit(NULL);
			CSocket Client;

			Client.Create();
			Client.Connect(_T("127.0.0.1"), 4567);

			// receive list of downable file from sever
			vector<fileZip> downable;
			int num_of_file;
			Client.Receive(&num_of_file, sizeof(int), 0);

			bool check = true;
			fileZip temp;

			for (int i = 0; i < num_of_file; i++) {

				Client.Receive(&temp, sizeof(fileZip), 0);
				downable.push_back(temp);
				Client.Send(&check, sizeof(bool), 0);
			}

			for (int i = 0; i < downable.size(); i++) {
				cout << downable[i].name << " " << downable[i].size << downable[i].sizeType << endl;
			}

			vector<fileZip> waiting;
			// Register signal and signal handler
			signal(SIGINT, signal_callback_handler);
			int alreadyDownload = 0;
			readInputFile("input.txt", waiting, alreadyDownload);
			alreadyDownload = waiting.size();

			do {
				auto start = steady_clock::now();

				fflush(stdin);
				// read input file
				waiting.clear(); // delete all the vector in the previous 
				readInputFile("input.txt", waiting, alreadyDownload);

				for (int i = 0; i < waiting.size(); i++) {
					int check = checkIfDownloaded(downable, waiting[i].name);
					cout << check << endl;
					// if file cannot downloaded
					if (check == -1) {
						GoToXY(0, num_of_file + 1);
						cout << waiting[i].name << " is not downloadable!                     " << endl;
					}

					// if file is already downloaded
					else if (check == 1) {
						GoToXY(0, num_of_file + 1);
						cout << waiting[i].name << " is downloaded!                           " << endl;
						continue;
					}

					// if file is not downloaded
					else if (check == 0) {
						isDownloading = true;
						string dFile = waiting[i].name;
						Client.Send(&dFile, sizeof(dFile), 0);

						GoToXY(0, num_of_file + 1);
						cout << dFile << " is downloading:";

						downFile(Client, dFile);
					}
				}

				isDownloading = false;

				alreadyDownload += waiting.size();

				// get the end timer
				auto end = steady_clock::now();
				auto elapsed = duration_cast<milliseconds>(end - start).count();

				int delay = 2000 - elapsed;
				if (delay > 0) {
					std::this_thread::sleep_for(milliseconds(delay));
				}
			} while (1);

			Client.Close();
		}
	}
	else {
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}


