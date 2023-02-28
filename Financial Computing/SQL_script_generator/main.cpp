#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <map>

using namespace std;

int main() {
    // Seed the random number generator
    srand(time(0));
    // save out put to file
    ofstream outputFile("dummies_transactions.txt");
    // Define the start and end dates
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::from_time_t(1580515200);
    end = chrono::system_clock::from_time_t(1644473600);

    // Generate a random date
    chrono::duration<double> duration = end - start;

    // number of random data
    int nRandomTransactions = 1000;

    outputFile << "INSERT INTO transactions (account_number, transaction_type, amount, date_of_transaction)\n"
            "VALUES" <<endl;

    for (int i = 0; i < nRandomTransactions; i++){
        chrono::duration<double> randomDuration = chrono::duration<double>(duration.count() * rand() / RAND_MAX);
        chrono::duration<long long int> randomDurationLLI = chrono::duration_cast<chrono::duration<long long int>>(randomDuration);
        chrono::time_point<chrono::system_clock> randomTime = start + randomDurationLLI;

        // Convert the random time to a string
        time_t randomTimeT = chrono::system_clock::to_time_t(randomTime);
        tm randomTimeTM = *localtime(&randomTimeT);

        // Format the random date
        char randomTimeString[20];
        strftime(randomTimeString, sizeof(randomTimeString), "%Y-%m-%d %H:%M:%S", &randomTimeTM);
        int randomAccountNumber = rand() % 8 +1;
        int randomAccountType = rand()%2 + 1;
        float randomAmount = rand()%100000+200;
        map<int,string> accountTypeMapping = {{1 , "Deposit"},{2 , "Withdrawal"},};
        // Print the random date
        outputFile<< "("+to_string(randomAccountNumber) +",'" + accountTypeMapping[randomAccountType] +"'," + to_string(randomAmount) +",'" +randomTimeString + "')";
        if (i < nRandomTransactions -1) outputFile << ","<<endl;
        else outputFile << ";";
    }

    return 0;
}
