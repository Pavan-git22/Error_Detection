//Implement error detection in C++ and evaluate performance
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;


string xorOp(string a, string b) {  //Performs bitwise XOR between two binary strings
    string result = "";
    for (int i = 1; i < b.length(); i++)
        result += (a[i] == b[i]) ? '0' : '1';
    return result;
}

// CRC division
string mod2div(string dividend, string divisor) {    
//Performs binary division (modulo-2 division) to compute remainder Like long division, but uses XOR instead of subtraction
    //The remainder is the CRC checksum
    int pick = divisor.length();
    string tmp = dividend.substr(0, pick);

    while (pick < dividend.length()) {
        if (tmp[0] == '1')
            tmp = xorOp(divisor, tmp) + dividend[pick];
        else
            tmp = xorOp(string(divisor.length(), '0'), tmp) + dividend[pick];
        pick++;
    }

    if (tmp[0] == '1')
        tmp = xorOp(divisor, tmp);
    else
        tmp = xorOp(string(divisor.length(), '0'), tmp);

    return tmp;
}

//Adds CRC bits to data before transmission
string encodeData(string data, string key) {
    string appended = data + string(key.length() - 1, '0');
    string remainder = mod2div(appended, key);
    return data + remainder;
}

// Introduce error
//Simulates a noisy communication channel
string introduceError(string data, double prob) {
    for (int i = 0; i < data.length(); i++) { //goes through all bits
        double r = (double)rand() / RAND_MAX; //generates a random value between 0 and 1
        if (r < prob) // less than prob the error
            data[i] = (data[i] == '0') ? '1' : '0';
    }
    return data;
}

//Checks if received data has error
bool detectError(string received, string key) {
    string remainder = mod2div(received, key);
    return (remainder.find('1') != string::npos);
}

int main() {
    srand(time(0));

    string data = "1101011011010101";
    string key = "1001";

    int totalPackets = 1000; //run simulation for 1000 packets
    int detectedErrors = 0;
    int actualErrors = 0;

    double errorProbability;
    cout << "Enter error probability (0 to 1): ";
    cin >> errorProbability;

    for (int i = 0; i < totalPackets; i++) {
        string encoded = encodeData(data, key);
        string received = introduceError(encoded, errorProbability); //adds noise

        if (encoded != received)  //count errors
            actualErrors++;

        if (detectError(received, key)) //count detected error
            detectedErrors++;
    }

    cout << "\nTotal Packets: " << totalPackets << endl;
    cout << "Actual Errors Introduced: " << actualErrors << endl;
    cout << "Errors Detected: " << detectedErrors << endl;

    if (actualErrors != 0) {
        double accuracy = (double)detectedErrors / actualErrors * 100;  //Detection Accuracy = detectedErrors / actualErrors * 100
        cout << "Detection Accuracy: " << accuracy << "%" << endl;
    } else {
        cout << "No errors introduced.\n";
    }

    return 0;
}
// Original Data → Encode using CRC → Send through noisy channel → Errors added → Receiver checks CRC → Detect errors