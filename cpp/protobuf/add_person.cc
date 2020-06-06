// See README.txt for information and build instructions.
#include <ctime>
#include <fstream>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "addressbook.pb.h"

using namespace std;

using google::protobuf::util::TimeUtil;

// This function fills in a Person message based on user input.
void PromptForAddress(tutorial::Person* person) {
    cout << "Enter person ID number: ";
    int id;
    cin >> id;
    person->set_id(id);
    cin.ignore(256, '\n');

    cout << "Enter name: ";
    getline(cin, *person->mutable_name());

    cout << "Enter email address (blank for none): ";
    string email;
    getline(cin, email);
    if (!email.empty()) {
        person->set_email(email);
    }

    while (true) {
        cout << "Enter a phone number (or leave blank to finish): ";
        string number;
        getline(cin, number);
        if (number.empty()) {
            break;
        }

        tutorial::Person::PhoneNumber* phone_number = person->add_phones();
        phone_number->set_number(number);

        cout << "Is this a mobile, home, or work phone? ";
        string type;
        getline(cin, type);
        if (type == "mobile") {
            phone_number->set_type(tutorial::Person::MOBILE);
        } else if (type == "home") {
            phone_number->set_type(tutorial::Person::HOME);
        } else if (type == "work") {
            phone_number->set_type(tutorial::Person::WORK);
        } else {
            cout << "Unknown phone type.  Using default." << endl;
        }
    }
    *person->mutable_last_updated() = TimeUtil::SecondsToTimestamp(time(NULL));
}

// Main function:  Reads the entire address book from a file,
//   adds one person based on user input, then writes it back out to the same
//   file.
int main(int argc, char* argv[]) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc != 2) {
        cerr << "Usage:  " << argv[0] << " ADDRESS_BOOK_FILE" << endl;
        return -1;
    }

    tutorial::AddressBook address_book;

    {
        bool fileexists = std::filesystem::exists(argv[1]);
        // Read the existing address book.
        int fileDescriptor = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        google::protobuf::io::FileInputStream fileInput(fileDescriptor);
        fileInput.SetCloseOnDelete( true );
        if (!fileexists) {
            cout << argv[1] << ": File not found.  Creating a new file." << endl;
        } else if (!google::protobuf::TextFormat::Parse(&fileInput,&address_book)) {
            cerr << "Failed to parse address book." << endl;
            return -1;
        }
    }

    // Add an address.
    PromptForAddress(address_book.add_people());
    {
        int fileDescriptor = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        // Write the new address book back to disk.
        google::protobuf::io::FileOutputStream fileOutput(fileDescriptor);
        fileOutput.SetCloseOnDelete( true );

        if (!google::protobuf::TextFormat::Print(address_book, &fileOutput)) {
            cerr << "Failed to write address book." << endl;
            return -1;
        }
    }
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
