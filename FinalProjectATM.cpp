// FinalProjectATM.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#pragma warning( push )
#pragma warning(disable:28020)
#include "json.hpp"
#pragma warning( pop )
using json = nlohmann::json;
const std::string DATAFILE = "Users.json";
int currentUser = 0;
///////////////////////////////
class UserData
{
public:
	UserData()
	{
		Data();
	}
	~UserData()
	{
		// For deleting memory.
		DeleteMemory();
	}
	// Time to create all the attributes.
	struct Account
	{
		struct User
		{
			int userID;
			std::string firstName;
			std::string lastName;
			int balance;
			std::vector<int> transaction_history;
		};
		User user;
		std::string username;
		std::string password;
		friend std::ostream& operator << (std::ostream& os, Account& account)
		{
			os << "User ID: " << account.user.userID << std::endl;
			os << "Name: " << account.user.firstName << " " << account.user.lastName << std::endl;
			os << "Current Balance: " << account.user.balance << std::endl;
			os << "Transaction History: ";
			for (int i = 0; i < account.user.transaction_history.size(); i++)
			{
				os << account.user.transaction_history[i] << " ";
			}
			os << std::endl;
			return os;
		}
	};
	friend std::ostream& operator << (std::ostream& os, UserData& ud)
	{
		os << *(ud.accounts[currentUser]) << std::endl;
		return os;
	}
	void Deposit(int amount)
	{
		accounts[currentUser]->user.balance = accounts[currentUser]->user.balance + amount;
		accounts[currentUser]->user.transaction_history.push_back(amount);
		json j;
		j["results"] = json::array();
		for (int i = 0; i < accountCount; i++)
		{
			json accountJSON;
			accountJSON["user"]["first_name"] = accounts[i]->user.firstName;
			accountJSON["user"]["last_name"] = accounts[i]->user.lastName;
			accountJSON["user"]["ID"] = accounts[i]->user.userID;
			accountJSON["username"] = accounts[i]->username;
			accountJSON["password"] = accounts[i]->password;
			accountJSON["user"]["transaction_history"] = accounts[i]->user.transaction_history;
			accountJSON["user"]["balance"] = accounts[i]->user.balance;
			j["results"].push_back(accountJSON);
		}
		std::ofstream file(DATAFILE);
		// Output the new account information to the file.
		file << j;
		file.close();
		return;
	}
	void Withdrawal(int amount)
	{
		if (accounts[currentUser]->user.balance == 0)
		{
			std::cout << "Balance is zero, exiting." << std::endl;
			exit(0);
		}
		else if (accounts[currentUser]->user.balance < amount)
		{
			std::cout << "Not enough funds in account, exiting." << std::endl;
			exit(0);
		}
		else
		{
			accounts[currentUser]->user.balance = accounts[currentUser]->user.balance - amount;
			// Make it negative for the transaction history.
			amount = amount * -1;
			accounts[currentUser]->user.transaction_history.push_back(amount);
			json j;
			j["results"] = json::array();
			for (int i = 0; i < accountCount; i++)
			{
				json accountJSON;
				accountJSON["user"]["first_name"] = accounts[i]->user.firstName;
				accountJSON["user"]["last_name"] = accounts[i]->user.lastName;
				accountJSON["user"]["ID"] = accounts[i]->user.userID;
				accountJSON["username"] = accounts[i]->username;
				accountJSON["password"] = accounts[i]->password;
				accountJSON["user"]["transaction_history"] = accounts[i]->user.transaction_history;
				accountJSON["user"]["balance"] = accounts[i]->user.balance;
				j["results"].push_back(accountJSON);
			}
			std::ofstream file(DATAFILE);
			// Output the new account information to the file.
			file << j;
			file.close();
			return;
		}
	}
	void Data()
	{
		std::fstream file;
		file.open(DATAFILE);
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file");
		}
		std::string fileContents((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
		// Parse the file contents.
		json j = json::parse(fileContents);
		accountCount = j["results"].size();
		accounts = new Account * [accountCount];
		for (int i = 0; i < accountCount; i++)
		{
			accounts[i] = nullptr;
		}
		for (int i = 0; i < accountCount; i++)
		{
			// Time to assign all the stuff from the json file
			accounts[i] = new Account;
			accounts[i]->user.firstName = j["results"][i]["user"]["first_name"];
			accounts[i]->user.lastName = j["results"][i]["user"]["last_name"];
			accounts[i]->user.userID = j["results"][i]["user"]["ID"];
			accounts[i]->username = j["results"][i]["username"];
			accounts[i]->password = j["results"][i]["password"];
			accounts[i]->user.balance = j["results"][i]["user"]["balance"];
			historyCount = j["results"][i]["user"]["transaction_history"].size();
			for (int t = 0; t < historyCount; t++)
			{
				accounts[i]->user.transaction_history.push_back(j["results"][i]["user"]["transaction_history"][t]);
			}
		}
	}
protected:
	size_t accountCount;
	size_t historyCount;
	Account** accounts;
private:
	void DeleteMemory()
	{
		if (accounts != nullptr)
		{
			for (int i = 0; i < accountCount; i++)
				delete accounts[i];
			delete[] accounts;
		}
	}
};


class Registration : public UserData
{
public:
	Registration()
	{

	}
	~Registration()
	{
		/*DeleteMemory();*/
	}
	void registerAccount()
	{
		// Create variables for all our attributes.
		std::string firstName;
		std::string lastName;
		std::string username;
		std::string password;
		int balance = 0;
		// Get user input.
		std::cout << "Enter your first name: ";
		std::cin >> firstName;
		std::cout << "Enter your last name: ";
		std::cin >> lastName;
		std::cout << "Create your username: ";
		std::cin >> username;
		std::cout << "Create your password: ";
		std::cin >> password;

		for (int i = 0; i < accountCount; i++)
		{
			if (accounts[i]->username == username)
			{
				std::cout << "Username already in use." << std::endl;
				exit(0);
			}
		}

		// Create a new Account object and assign it to the next available index in the "accounts" array.
		newAccount = new Account * [accountCount];
		for (int i = 0; i < accountCount; i++)
		{
			newAccount[i] = nullptr;
		}
		currentUser = accountCount - 1;
		newAccount[currentUser] = new Account;
		accounts[accountCount] = newAccount[currentUser];
		accountCount++;
		// Assign the user inputted variables to the appropriate attributes of the new Account object.
		newAccount[currentUser]->user.userID = accountCount - 1;
		newAccount[currentUser]->user.firstName = firstName;
		newAccount[currentUser]->user.lastName = lastName;
		newAccount[currentUser]->username = username;
		newAccount[currentUser]->password = password;
		newAccount[currentUser]->user.balance = balance;

		json j;
		j["results"] = json::array();
		for (int i = 0; i < accountCount; i++)
		{
			json accountJSON;
			accountJSON["user"]["first_name"] = accounts[i]->user.firstName;
			accountJSON["user"]["last_name"] = accounts[i]->user.lastName;
			accountJSON["user"]["ID"] = accounts[i]->user.userID;
			accountJSON["username"] = accounts[i]->username;
			accountJSON["password"] = accounts[i]->password;
			accountJSON["user"]["transaction_history"] = accounts[i]->user.transaction_history;
			accountJSON["user"]["balance"] = accounts[i]->user.balance;
			j["results"].push_back(accountJSON);
		}
		std::ofstream file(DATAFILE);
		// Output the new account information to the file.
		std::cout << "Account created successfully, log in again to access account." << std::endl;
		file << j;
		file.close();
	}
private:
	Account** newAccount;
};
class Login : public UserData
{
public:
	Login()
	{

	}
	~Login()
	{

	}
	int loginAccount()
	{
		// Create variables for account info.
		std::string username;
		std::string password;
		// Take user input for username and password.
		std::cout << "Enter your username: ";
		std::cin >> username;
		// Check accounts for existing username.
		for (int i = 0; i < accountCount; i++)
		{
			if (accounts[i]->username == username)
			{
				currentUser = accounts[i]->user.userID;
				break;
			}
		}
		std::cout << "Enter your password: ";
		std::cin >> password;
		if (accounts[currentUser]->password == password)
		{
			std::cout << "Login successful!" << std::endl;
			return currentUser;
		}
		else
		{
			std::cout << "Incorrect Password." << std::endl;
			exit(0);
		}
	}
};


int main()
{
	UserData ud;
	ud.Data();
	Registration r;
	Login l;
	double choice;
	// Introduction
	std::cout << std::setfill('*') << std::setw(60) << "*" << std::endl;
	std::cout << std::setfill('*') << std::setw(20) << "*" << " Welcome to my ATM! " << std::setfill('*') << std::setw(20) << "*" << std::endl;
	std::cout << std::setfill('*') << std::setw(60) << "*" << std::endl;
	std::cout << "If you wish to register for a new account, enter '1', if you already have an account, enter '2' to login. Press any other key to exit." << std::endl;
	// Take user choice of 1 or 2.
	std::cin >> choice;
	if (choice == 1)
	{
		// Begin registration process.
		r.registerAccount();
		exit(0);
	}
	else if (choice == 2)
	{
		// Begin login process.
		l.loginAccount();
		// Output account info.
		std::cout << ud << std::endl;
		std::cout << "Press 1 to make a deposit, press 2 to make a withdrawal: ";
		std::cin >> choice;
		if (choice == 1)
		{
			//Deposit
			std::cout << "How much would you like to deposit? ";
			std::cin >> choice;
			if (choice < 0)
			{
				std::cout << "You can't deposit a negative number..." << std::endl;
				exit(0);
			}
			else
			{
				ud.Deposit(choice);
			}
		}
		else if (choice == 2)
		{
			//Withdrawal
			std::cout << "How much would you like to withdrawal? ";
			std::cin >> choice;
			if (choice < 0)
			{
				std::cout << "You can't withdrawal a negative number..." << std::endl;
				exit(0);
			}
			else
			{
				ud.Withdrawal(choice);
			}
		}
		else
		{
			// Exit
			exit(0);
		}
	}
	else
	{
		// Exit
		exit(0);
	}
}
