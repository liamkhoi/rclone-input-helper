#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <conio.h>
#include <limits>
#include <vector>
#include <sstream>

bool loop;
std::string tabLength = "    ";

void disclaimer(){
    std::string note = R"(
         **NOTE** 
    - This program is developed for basic personal use and does not modify the rclone software.
    - Be careful with inputs, may lead to serious consequences from wrong executions.
    - Program is currently limited in functionalities and inconsistent code structure. 
    - Would be happy to receive contributions. Thank you! <3
          )";
    std::cout << "\r" << note << "\n";
    std::cout << "\r" << "Press Enter to start using...";
    std::cin.ignore();

    system("cls");
}

int check4rclone(){
    const char* filename = "rclone.exe";
    FILE *file;
    if (file = fopen(filename, "r")){
        std::cout << "\r" << "Rclone executable present." << "\n";
        disclaimer();
        return 0;
    }
    else {
        std::cout << "\r" << "Unable to locate rclone.exe. Please run this program in the same directory with rclone.exe" << "\n";
        return 1;
    }
}

int provideOptions(int checked) {
    std::cout << "\r\n" << "    - rclone basic input utility v.1.0 by LiamKhoi -    " << "\n";
    unsigned int corresWidth = 8;
    std::cout << std::left;
    std::cout << tabLength << std::setw(corresWidth) << "[1]" << "Run remote config" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[2]" << "View remotes" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[3]" << "Sync" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[4]" << "Mount" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[5]" << "View rclone version info" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[6]" << "Terminate running tasks" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[ESC]" << "Quit" << "\n";

    std::string ask4option = R"(Press the corresponding key to select)";
    std::cout << "\n" << ask4option << std::flush;
    char optionSelect = _getch();
    if (optionSelect == 27){
        return 27;
    }
    else if (optionSelect >= '1' && optionSelect <= '7') {
        system("cls");
        return optionSelect - '0'; // Convert char to integer
    }
    else {
        return 0;
    }
}

void millisecsDelay(int millisecs){
    std::this_thread::sleep_for(std::chrono::milliseconds(millisecs));
}

//output parsing
std::string execCommand(const std::string& cmd) {
    std::string result;
    char buffer[128];
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    _pclose(pipe);
    return result;
}

std::vector<std::string> splitByLine(const std::string& input) {
    std::vector<std::string> lines;
    std::stringstream ss(input);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

//all rclone operation functions here

void returnEnter(){
    std::cout << "\n";
    std::cout << "\r" << "Press Enter to return to the option menu...";
    bool lock = 1;
    while (lock) {
        if (_kbhit()) {
            char askedInput = _getch();
            if (askedInput == 13) {
                lock = 0; 
            }
        }
    }
}

void cleanedBufferReturn(){
    std::cout << "\r" << "Press enter to return to the option menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get(); 
}

void remoteList(){
    std::cout << "\n";
    std::cout << "\r" << "-- Remote list (parsed testing) --" << "\n";

    std::string output = execCommand("rclone listremotes");
    if (output == "ERROR") {
        std::cout << "\r" << "Failed to run rclone listremotes." << "\n";
        return;
    }

    std::vector<std::string> remotes = splitByLine(output);
    if (remotes.empty()) {
        std::cout << "\r" << "No remotes found." << "\n";
    } else {
        for (const auto& remote : remotes) {
            std::cout << "\r" << remote << "\n";
        }
    }
}

void remoteConfig(){
    std::this_thread::sleep_for(std::chrono::microseconds(500));
    system("start cmd /c rclone config");
}

void viewRemote(){
    remoteList();
    returnEnter();
}

int mountSelect(int& mountInput) {
    if (!(std::cin >> mountInput)) {
        std::cin.clear(); 
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return 0;
    }

    // Clear input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return mountInput;
}

void mount(){
    remoteList();
    std::this_thread::sleep_for(std::chrono::microseconds(500));
    unsigned int corresWidth = 18;
    std::cout << std::left;
    std::cout << "\r" << tabLength << "Warning: Invalid input will not work." << "\n\n";
    std::string remoteName;
    std::string driveLetter;
    std::cout << "\r" << std::setw(corresWidth) << "Remote name: " << "| ";
    std::cin >> remoteName;
    std::cout << "\r" <<  std::setw(corresWidth) << "Drive letter: " << "| ";
    std::cin >> driveLetter;
    std::string mountCommandJoint = "start /B cmd /c rclone mount " + remoteName + " " + driveLetter + " --vfs-cache-mode full > NUL 2>&1";
    system(mountCommandJoint.c_str());
    remoteName.erase(remoteName.size() - 1);
    std::cout << "\r" <<  remoteName << " (" << driveLetter << ") has been mounted." << "\n" << "\n";
    millisecsDelay(1000);
    cleanedBufferReturn();
}

void sync(){
    std::string sourceName;
    std::string destinationName;
    std::string selectSync;
    remoteList();
    millisecsDelay(500);
    std::cout << "\n" << "\r" << "Source name: ";
    std::cin >> sourceName;
    std::cout << "\r" << "Destination name: ";
    std::cin >> destinationName;
    std::this_thread::sleep_for(std::chrono::microseconds(500));
    std::string viewProgress = R"(
    Run in background? (You will not know the status of the syncing operation.)
    Yes/No/Cancel [Y/N]: )";
    std::cout << "\r" << viewProgress;
    std::cin >> selectSync;
    if (selectSync == "yes" || selectSync == "Y" || selectSync == "y" || selectSync == "Yes"){
        std::string syncCommandJoint = "start /B cmd /c rclone sync " + sourceName + " " + destinationName + " -P" + " > NUL 2>&1";
        system(syncCommandJoint.c_str());
        millisecsDelay(100);
        std::cout << "\r" << "Operation has started in background." << '\n';
    }
    else if (selectSync == "no" || selectSync == "n" || selectSync == "N" || selectSync == "No"){
        std::string syncCommandJoint = "start cmd /c rclone sync " + sourceName + " " + destinationName + " -P";
        int syncWindow = system(syncCommandJoint.c_str());
        millisecsDelay(100);
        std::cout << "\r" << "Operation has started." << '\n';
        millisecsDelay(4000);
        std::cout << "\r" << "If the command window closed instantly, the syncing operation has failed." << '\n';
    }
    else {
        std::cout << "\r" << "Sync operation cancelled." << '\n';
    }
    cleanedBufferReturn();
    
}

void ver(){
    std::cout << "\n";
    system("rclone --version");
    returnEnter();
}

void taskTerminate(){
    bool validOption = false;
    std::string confirmationMsg = R"(
    All running operations will be terminated. Make sure not to halt any important things.
    Type Yes (Y) to confirm: )";
    std::string confirmationInput;
    std::cout << '\n' << confirmationMsg;
    std::cin >> confirmationInput;
    std::cin.ignore();
    if (confirmationInput == "yes" || confirmationInput == "Y" || confirmationInput == "y" || confirmationInput == "Yes"){
        validOption = true;
        system("taskkill /IM rclone.exe /F");
        returnEnter();
    }
    else{
        std::cout << "\r" << "Cancelled." << '\n';
        millisecsDelay(2000);
    }
}
    

// input values corresponding to its functions
int optionExecution(int selectValue){
    if (selectValue == 1){
        remoteConfig();
        millisecsDelay(3000);
        return 0;
    }
    else if (selectValue == 2){
        viewRemote();
        millisecsDelay(1000);
        return 0;
    }
    else if (selectValue == 3){
        sync();
        millisecsDelay(500);
        return 0;
    }
    else if (selectValue == 4){
        mount();
        millisecsDelay(500);
        return 0;
    }
    else if (selectValue == 5){
        ver();
        millisecsDelay(500);
        return 0;
    }
    else if (selectValue == 6){
        taskTerminate();
        millisecsDelay(500);
        return 0;
    }
    else if (selectValue == 27){ // number 27 corresponds to the ESC key in the ASCII table, which in this case was pressed when asked for an option.
        return -1;
    }
    else if (selectValue == 0){
        return 0;
    }
}

int main(){
    loop = true;
    int returnedOption;
    int fileExists = check4rclone();
    while (loop){
        int returnedOption;
        if(fileExists == 0){
            returnedOption = provideOptions(fileExists);
        }
        else{
            loop = false;
            millisecsDelay(3000);
        }
        int optionExecDone;
        optionExecDone = optionExecution(returnedOption);
        if (optionExecDone == -1){
            loop = false;
        }
        else if (optionExecDone == 0){
            system("cls");
        }

    }
    system("cls");
    std::cout << "\r\n" << "Program will exit..." << "\n";
    millisecsDelay(1000);
    return 0;
}


