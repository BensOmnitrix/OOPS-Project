#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
using namespace std;

class Movie {
public:
    string movieName;
    vector<string> showTimes;
    map<string, vector<vector<string>>> availableSeats; 

    Movie() {}

    Movie(string name, vector<string> times) {
        movieName = name;
        showTimes = times;
        for (string time : times) {
            vector<vector<string>> seats(5, vector<string>(5)); 
            char rowLabel = 'A';
            for (int row = 0; row < 5; ++row) {
                for (int col = 0; col < 5; ++col) {
                    seats[row][col] = string(1, rowLabel) + to_string(col + 1); 
                }
                rowLabel++;
            }
            availableSeats[time] = seats;
        }
    }

  
    void displayMovieDetails() {
        cout << "Movie Name: " << movieName << endl;
        for (string time : showTimes) {
            cout << "Show Time: " << time << " | Available Seats: \n";
            vector<vector<string>>& seats = availableSeats[time];
            for (int row = 0; row < seats.size(); ++row) {
                for (int col = 0; col < seats[row].size(); ++col) {
                    if (seats[row][col] != "XX") { 
                        cout << seats[row][col] << " ";
                    } else { 
                        cout << "XX "; 
                    }
                }
                cout << endl; 
            }
            cout << endl;
        }
    }

    // Book specific seats for a showtime
    bool bookSeats(string time, vector<string> seatsToBook) {
        vector<vector<string>>& seats = availableSeats[time];
        bool allAvailable = true;

        for (const string& seat : seatsToBook) {
            bool seatFound = false;
            for (int row = 0; row < seats.size(); ++row) {
                for (int col = 0; col < seats[row].size(); ++col) {
                    if (seats[row][col] == seat) {
                        seatFound = true;
                        seats[row][col] = "XX"; 
                        break;
                    }
                }
                if (seatFound) break;
            }
            if (!seatFound) {
                allAvailable = false;
                cout << "Seat " << seat << " is already booked or invalid!" << endl;
            }
        }

        return allAvailable;
    }

 
    void updateShowtime(string time, int totalSeats) {
        int rows = totalSeats / 5; 
        vector<vector<string>> newSeats(rows, vector<string>(5));
        char rowLabel = 'A';
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < 5; ++col) {
                newSeats[row][col] = string(1, rowLabel) + to_string(col + 1);
            }
            rowLabel++;
        }
        availableSeats[time] = newSeats;
    }
};

class Customer {
public:
    string customerName;
    vector<pair<string, pair<string, vector<string>>>> bookings;

    Customer() {}

    Customer(string name) {
        customerName = name;
    }

    void addBooking(string movie, string time, vector<string> seats) {
        bookings.push_back(make_pair(movie, make_pair(time, seats)));
        saveBookingToFile(movie, time, seats);
    }

    void saveBookingToFile(string movie, string time, vector<string> seats) {
        ofstream file("booking_history.txt", ios::app);
        if (file.is_open()) {
            file << "Customer: " << customerName << "\n";
            file << "Movie: " << movie << " | Time: " << time << " | Seats: ";
            for (const string& seat : seats) {
                file << seat << " ";
            }
            file << "\n\n";
            file.close();
        } else {
            cout << "Error: Could not open booking history file!" << endl;
        }
    }

    void displayBookingHistory() {
        for (auto& booking : bookings) {
            cout << "Movie: " << booking.first << " | Time: " << booking.second.first << " | Seats: ";
            for (const string& seat : booking.second.second) {
                cout << seat << " ";
            }
            cout << endl;
        }
    }
};


class User {
public:
    string username;
    string password;

    bool login(string uname, string pass) {
        return username == uname && password == pass;
    }
};

vector<Movie> loadMovies() {
    vector<Movie> movies;
    ifstream file("movies.csv");
    string line;

    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return movies;
    }
    getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string name, timesStr;
        vector<string> times;

        if (!getline(ss, name, ',')) continue;
        if (!getline(ss, timesStr, ',')) continue;

        stringstream timesStream(timesStr);
        string time;
        while (getline(timesStream, time, '|')) {
            times.push_back(time);
        }

        movies.push_back(Movie(name, times)); 
    }

    file.close();
    return movies;
}

vector<Customer> loadCustomers() {
    vector<Customer> customers;
    ifstream file("customers.txt");
    string line;
    while (getline(file, line)) {
        Customer customer;
        customer.customerName = line;
        customers.push_back(customer);
    }
    file.close();
    return customers;
}

void saveCustomers(const vector<Customer>& customers) {
    ofstream file("customers.txt");
    for (const Customer& customer : customers) {
        file << customer.customerName << endl;
    }
    file.close();
}

void saveAdmin(const User& admin) {
    ofstream file("admin.txt");
    file << admin.username << "," << admin.password << endl;
    file.close();
}

User loadAdmin() {
    User admin;
    ifstream file("admin.txt");
    string line;
    if (getline(file, line)) {
        size_t pos = line.find(",");
        admin.username = line.substr(0, pos);
        admin.password = line.substr(pos + 1);
    }
    file.close();
    return admin;
}

void adminMode(vector<Movie>& movies) {
    int choice;
    do {
        cout << "Admin Menu:\n1. Add Movie\n2. Update Seats\n3. Display Movies\n4. Logout\nEnter your choice: ";
        cin >> choice;
        switch (choice) {
        case 1: {
            string name;
            vector<string> times;
            cout << "Enter movie name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter number of shows: ";
            int numShows;
            cin >> numShows;
            for (int i = 0; i < numShows; i++) {
                string time;
                cout << "Enter show time " << i + 1 << ": ";
                cin >> time;
                times.push_back(time);
            }
            movies.push_back(Movie(name, times));
            ofstream file("movies.csv", ios::app);
            if (file.is_open()) {
                file << name << ",";
                for (size_t i = 0; i < times.size(); ++i) {
                    file << times[i];
                    if (i < times.size() - 1) file << "|"; // Separate showtimes with '|'
                }
                file << 25;
                file << "\n";
                file.close();
                cout << "Movie added successfully and updated in movies.csv!\n";
            } else {
                cout << "Error: Could not open movies.csv for updating!\n";
            }
            break;
            break;
        }
        case 2: {
            string name, time;
            int seats;
            cout << "Enter movie name: ";
            cin.ignore();
            getline(cin, name);
            for (Movie& movie : movies) {
                if (movie.movieName == name) {
                    cout << "Enter show time to update: ";
                    cin >> time;
                    cout << "Enter new available seats: ";
                    cin >> seats;
                    movie.updateShowtime(time, seats);
                    break;
                }
            }
            break;
        }
        case 3:
            for (Movie& movie : movies) {
                movie.displayMovieDetails();
            }
            break;
        case 4:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice!\n";
            break;
        }
    } while (choice != 4);
}

void customerMode(vector<Movie>& movies, vector<Customer>& customers) {
    string name;
    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, name);

    // Find or create the customer
    Customer* currentCustomer = nullptr;
    for (Customer& customer : customers) {
        if (customer.customerName == name) {
            currentCustomer = &customer;
            break;
        }
    }

    if (!currentCustomer) {
        // New customer
        customers.push_back(Customer(name));
        currentCustomer = &customers.back();
    }

    int choice;
    do {
        cout << "Customer Menu:\n1. View Movies\n2. Book Ticket\n3. View Booking History\n4. Logout\nEnter your choice: ";
        cin >> choice;
        switch (choice) {
        case 1:
            for (Movie& movie : movies) {
                movie.displayMovieDetails();
            }
            break;
        case 2: {
            // Display available movies and showtimes
            cout << "Available movies:\n";
            for (int i = 0; i < movies.size(); ++i) {
                cout << i + 1 << ". " << movies[i].movieName << endl;
            }

           
            int movieIndex;
            cout << "Enter movie number: ";
            cin >> movieIndex;
            --movieIndex; 

            if (movieIndex < 0 || movieIndex >= movies.size()) {
                cout << "Invalid movie selection!\n";
                break;
            }

          
            Movie& selectedMovie = movies[movieIndex];
            cout << "Available showtimes:\n";
            for (int i = 0; i < selectedMovie.showTimes.size(); ++i) {
                cout << i + 1 << ". " << selectedMovie.showTimes[i] << endl;
            }

            int showtimeIndex;
            cout << "Enter showtime number: ";
            cin >> showtimeIndex;
            --showtimeIndex;

            if (showtimeIndex < 0 || showtimeIndex >= selectedMovie.showTimes.size()) {
                cout << "Invalid showtime selection!\n";
                break;
            }

            string selectedTime = selectedMovie.showTimes[showtimeIndex];
            vector<vector<string>>& seats = selectedMovie.availableSeats[selectedTime];

           
            cout << "Available seats (XX = booked):\n";
            for (int row = 0; row < seats.size(); ++row) {
                for (int col = 0; col < seats[row].size(); ++col) {
                    cout << seats[row][col] << " ";
                }
                cout << endl;
            }

           
            int numSeats;
            cout << "Enter number of seats to book: ";
            cin >> numSeats;

            vector<string> seatsToBook(numSeats);
            cout << "Enter seat numbers (e.g., A1, B2): ";
            for (int i = 0; i < numSeats; ++i) {
                cin >> seatsToBook[i];
            }

            
            if (selectedMovie.bookSeats(selectedTime, seatsToBook)) {
                cout << "Seats booked successfully!\n";
                currentCustomer->addBooking(selectedMovie.movieName, selectedTime, seatsToBook);
            } else {
                cout << "Some seats were unavailable. Please try again.\n";
            }
            break;
        }
        case 3: {
           
            cout << "Booking History for " << name << ":\n";
            currentCustomer->displayBookingHistory();
            break;
        }
        case 4:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice!\n";
            break;
        }
    } while (choice != 4);
}

int main() {
    User admin = loadAdmin();
    vector<Movie> movies = loadMovies();
    vector<Customer> customers = loadCustomers();

    string inputUsername, inputPassword;
    int loginAttempts = 0;
    int choice;

    do {
        cout << "Welcome to the Movie Ticket Booking System\n";
        cout << "1. Admin Login\n2. Customer Mode\n3. Exit\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            bool loggedIn = false;
            do {
                cout << "Enter admin username: ";
                cin >> inputUsername;
                cout << "Enter admin password: ";
                cin >> inputPassword;
                if (admin.login(inputUsername, inputPassword)) {
                    loggedIn = true;
                    adminMode(movies);
                } else {
                    cout << "Incorrect username or password! Try again.\n";
                    loginAttempts++;
                }
            } while (!loggedIn && loginAttempts < 3);

            if (!loggedIn) {
                cout << "Too many failed attempts. Exiting.\n";
                return 0;
            }
            break;
        }
        case 2:
            customerMode(movies, customers);
            break;
        case 3:
            cout << "Exiting the system.\n";
            break;
        default:
            cout << "Invalid choice!\n";
            break;
        }
    } while (choice != 3);

    saveCustomers(customers);
    saveAdmin(admin);

    return 0;
}