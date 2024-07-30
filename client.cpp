#include <time.h>
#include <thread>
using namespace std;
#include "afxsock.h"
// #include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include "FileDownload.h"

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
	hideCursor();
	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
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
			Client.Connect(_T("127.0.0.1"), 45673);

			// receive list of file from server
			vector<fileZip> ServerFile;
			int num_of_file;
			Client.Receive(&num_of_file, sizeof(int), 0);
			int coordY = num_of_file + 2;
			bool check = true;
			fileZip temp;

			for (int i = 0; i < num_of_file; i++) {

				Client.Receive(&temp, sizeof(fileZip), 0);
				ServerFile.push_back(temp);
				Client.Send(&check, sizeof(bool), 0);
			}

			// Register signal and signal handler
			signal(SIGINT, signal_callback_handler);
			printServerFile(ServerFile);

			vector <fileZip> DownloadFile;

			// before client input new file, read the file to later skip the previous files
			int alreadyDownload = 0;
			readInputFile("input.txt", DownloadFile, alreadyDownload);
			alreadyDownload = DownloadFile.size();
			/*for (int i = 0; i < alreadyDownload; i++) {
				cout << DownloadFile[i].name << endl;
			}*/

			do {
				fflush(stdin);
				// read input file
				DownloadFile.clear(); // delete all the vector in the previous attempt
				readInputFile("input.txt", DownloadFile, alreadyDownload);

				bool foundNew = false;
				for (int i = 0; i < DownloadFile.size(); i++) {
					foundNew = true;
					int check = checkIfDownloaded(ServerFile, DownloadFile[i].name);
					switch (check) {
					case 0: { // not exist
						GoToXY(0, coordY);
						cout << DownloadFile[i].name << " is not available on server!                    " << endl;
						break;
					}
					case 1: { // can be download
						isDownloading = true;
						string dFile = DownloadFile[i].name;
						Client.Send(&dFile, sizeof(dFile), 0);

						GoToXY(0, coordY);
						cout << dFile << " is downloading:    ";

						downFile(Client, dFile, coordY);
						isDownloading = false;
						break;
					}
					case 2: {
						GoToXY(0, coordY);
						cout << DownloadFile[i].name << " is already downloaded!                           " << endl;
						break;
					}
					}
					coordY++;
				}
				if (foundNew) {

					Sleep(3000);
					system("cls");
					printServerFile(ServerFile);
				}

				// reset cac bien
				isDownloading = false;
				alreadyDownload += DownloadFile.size();
				coordY = num_of_file + 2;
				Sleep(2);
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


