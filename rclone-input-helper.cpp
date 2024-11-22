#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <conio.h>

bool loop;

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
    std::cout << "\r" << "Checking for rclone..." << "\n";
    const char* filename = "rclone.exe";
    FILE *file;
    if (file = fopen(filename, "r")){
        std::cout << "\r" << "Rclone executable present." << "\n";
        disclaimer();
        return 0;
    }
    else {
        std::cout << "\r" << "Rclone.exe not found. Please run this program in the same directory with rclone.exe" << "\n";
        return 1;
    }
}

int provideOptions(int checked) {
    std::cout << "\r\n" << "    - rclone basic input utility v.1.0 by LiamKhoi -    " << "\n";
    std::string optionText = R"(
        [1] Run remote config
        [2] View remotes
        [3] Sync
        [4] Mount
        [5] View rclone version info
        [6] Terminate running tasks
        [ESC] Quit 
        )";
    std::string ask4option = R"(Select what you need to do: )";
    std::cout << optionText << "\n" << ask4option << std::flush;

    /*if (!(std::cin >> optionSelect)) {
        std::cin.clear(); // Clear the fail flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
        return 0; 
    }
    // Clear input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    */
    char optionSelect = _getch();
    if (optionSelect == 27){
        return 27;
    }
    else if (optionSelect >= '1' && optionSelect <= '7') {
        // If a valid option (1-7) is pressed
        return optionSelect - '0'; // Convert char to integer
    }
}

void millisecsDelay(int millisecs){
    std::this_thread::sleep_for(std::chrono::milliseconds(millisecs));
}

//all rclone operation functions here

void returnEnter(){
    std::cout << "\n";
    std::cout << "\r" << "Press enter to return to the option menu...";
    std::cin.ignore();
}

void cleanedBufferReturn(){
    std::cout << "\r" << "Press enter to return to the option menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get(); 
}

void remoteList(){
    std::cout << "\n";
    std::cout << "\r" << "--Remote list--" << "\n";
    system("rclone listremotes");
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
    std::cout << "\r" << "Invalid input will not work." << "\n";
    std::string remoteName;
    std::string driveLetter;
    std::cout << "\r" << "Remote name (no colon): ";
    std::cin >> remoteName;
    std::cout << "\r" << "Drive letter (no colon): ";
    std::cin >> driveLetter;
    std::string mountCommandJoint = "start /B cmd /c rclone mount " + remoteName + ": " + driveLetter + ": --vfs-cache-mode full > NUL 2>&1";
    system(mountCommandJoint.c_str());
    std::cout << "\r" <<  remoteName << " (" << driveLetter << ":) has been mounted." << "\n" << "\n";
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
        if (optionExecDone == 0){
            system("cls");
        }

    }

    std::cout << "\r\n" << "Program will exit..." << "\n";
    millisecsDelay(1000);
    return 0;
}


