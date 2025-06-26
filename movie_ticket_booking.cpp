#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <string>
using namespace std;

class Movie {
protected:
    static const string seat_file;
    static const string time_file;

    static void addMovie(const string &name, const string &time) {
        ofstream seat_out(seat_file, ios::app);
        seat_out << name;
        for (int i = 0; i < 20; ++i) seat_out << ",";
        seat_out << "\n";
        seat_out.close();

        ofstream time_out(time_file, ios::app);
        time_out << name << "," << time << "\n";
        time_out.close();
    }

    static void removeMovieIfEnded(const string &name) {
        cout << "Assuming end-time logic is handled externally.\nRemoving movie: " << name << "\n";

        // Remove from seat file
        ifstream in(seat_file);
        ofstream temp("temp_seat.csv");
        string line;
        while (getline(in, line)) {
            if (line.substr(0, line.find(',')) != name)
                temp << line << "\n";
        }
        in.close(); temp.close();
        remove(seat_file.c_str());
        rename("temp_seat.csv", seat_file.c_str());

        // Remove from time file
        ifstream in2(time_file);
        ofstream temp2("temp_time.csv");
        while (getline(in2, line)) {
            if (line.substr(0, line.find(',')) != name)
                temp2 << line << "\n";
        }
        in2.close(); temp2.close();
        remove(time_file.c_str());
        rename("temp_time.csv", time_file.c_str());

        cout << "Removed movie '" << name << "' from system.\n";
    }
public:
    static void showAllMovies() {
        ifstream in(seat_file);
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string movie, seat;
            getline(ss, movie, ',');
            cout << "\nMovie: " << movie << "\nSeats: ";
            for (int i = 0; i < 20; ++i) {
                getline(ss, seat, ',');
                cout << (seat.empty() ? "[ ]" : "[X]") << " ";
            }
            cout << "\n";
        }
        in.close();
    }

    static void showMovieTimings() {
        ifstream in(time_file);
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string movie, time;
            getline(ss, movie, ',');
            getline(ss, time);
            cout << "Movie: " << movie << " | Time: " << time << "\n";
        }
        in.close();
    }

    static void updateSeat(const string &movie, int seat, const string &name) {
        ifstream in(seat_file);
        ofstream temp("temp.csv");
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string movie_name;
            getline(ss, movie_name, ',');
            vector<string> seats(20);
            for (int i = 0; i < 20; ++i) getline(ss, seats[i], ',');

            if (movie_name == movie)
                seats[seat] = name;

            temp << movie_name;
            for (const string &s : seats) temp << "," << s;
            temp << "\n";
        }
        in.close(); temp.close();
        remove(seat_file.c_str());
        rename("temp.csv", seat_file.c_str());
    }

    static vector<string> getAvailableSeats(const string &movie) {
        ifstream in(seat_file);
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string movie_name;
            getline(ss, movie_name, ',');

            if (movie_name == movie) {
                vector<string> seats(20);
                for (int i = 0; i < 20; ++i) getline(ss, seats[i], ',');
                return seats;
            }
        }
        return {};
    }
};

const string Movie::seat_file = "movie_seats.csv";
const string Movie::time_file = "movie_timings.csv";

class Credentials {
protected:
    static const string user_file;

    int registerUser() {
    int id;
    string name, pass;

    while (true) {
        cout << "\nRegister - Enter user ID, name, and password all in new line:\n";
        cin >> id >> name >> pass;

        ifstream in(user_file);
        string line;
        bool exists = false;

        while (getline(in, line)) {
            if(line.empty()) continue;
            stringstream ss(line);
            string uid;
            getline(ss, uid, ',');
            if (stoi(uid) == id) {
                exists = true;
                break;
            }
        }
        in.close();

        if (exists) {
            cout << "User ID already exists! Try again.\n";
        } else {
            ofstream out(user_file, ios::app);
            out << id << "," << name << "," << pass << "\n";
            out.close();
            return id;
        }
    }
}

public:
    int validateCredentials(int attempts) {
        int mode;
        cout << "\nLogin (1) or Register (0): ";
        cin >> mode;

        if (mode) {
            int id; string pass;
            cout << "Enter user ID: "; cin >> id;
            cout << "Enter password: "; cin >> pass;

            ifstream in(user_file);
            string line;
            while (getline(in, line)) {
                stringstream ss(line);
                string uid, name, stored_pass;
                getline(ss, uid, ',');
                getline(ss, name, ',');
                getline(ss, stored_pass);

                if (stoi(uid) == id) {
                    while (attempts--) {
                        if (pass == stored_pass) return id;
                        cout << "Incorrect password. Attempts left: " << attempts << "\n";
                        cout << "Re-enter password: ";
                        cin >> pass;
                    }
                    return 0;
                }
            }
            cout << "User ID not found.\n";
            return registerUser();
        } else {
            return registerUser();
        }
    }

    static string getUserName(int id) {
        ifstream in(user_file);
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string uid, name;
            getline(ss, uid, ',');
            getline(ss, name, ',');
            if (stoi(uid) == id) return name;
        }
        return "Unknown";
    }
};

const string Credentials::user_file = "user_accounts.csv";

class Customer : protected Credentials, public Movie {
private:
    int ticket_price = 500;
    string customer_name;
    int user_id;

    void bookingFlow() {
        cout << "\nLogin successful.\nEnter -1 to cancel, 1 to book, 0 to exit: ";
        int choice; cin >> choice;
        if (choice == 1) {
            showAllMovies();
            string movie;
            cout << "Enter movie name: ";
            cin >> ws;
            getline(cin, movie);

            auto seats = getAvailableSeats(movie);
            cout << "Available seats:\n";
            for (int i = 0; i < seats.size(); ++i)
                if (seats[i].empty()) cout << i << " ";
            cout << "\nChoose seat number: ";
            int seat_no; cin >> seat_no;

            cout << "Enter Rs." << ticket_price << ": ";
            int amt; cin >> amt;
            if (amt == ticket_price) {
                updateSeat(movie, seat_no, customer_name);
                cout << "Booking confirmed!\n";
            } else {
                cout << "Payment failed.\n";
            }
        } else if (choice == -1) {
            string movie; int seat;
            cout << "Enter movie and seat number to cancel: ";
            cin >> movie >> seat;
            updateSeat(movie, seat, "");
            cout << "Cancelled.\n";
        }
    }

public:
    Customer() {
        int id = validateCredentials(3);
        if (id) {
            user_id = id;
            customer_name = getUserName(id);
            bookingFlow();
        } else {
            cout << "Access denied.\n";
        }
    }
};

class Manager : protected Movie {
private:
    string admin_pass = "admin123";
    double total_revenue = 0;

    void calcRevenue() {
        int filled = 0;
        ifstream in(seat_file);
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string discard, seat;
            getline(ss, discard, ',');
            for (int i = 0; i < 20; ++i) {
                getline(ss, seat, ',');
                if (!seat.empty()) filled++;
            }
        }
        cout << "Total Revenue: Rs. " << filled * 500 << "\n";
    }

public:
    Manager() {
        string input;
        cout << "Enter admin password: ";
        cin >> input;
        if (input != admin_pass) {
            cout << "Access denied.\n";
            return;
        }

        while (true) {
            cout << "\n1. Remove movie\n2. Add movie\n3. Show movies\n4. Revenue\n5. Show timings\n6. Exit\nChoice: ";
            int op;
            cin >> op;
            cin.ignore();
            if (op == 1) {
                string movie;
                cout << "Enter movie to remove: ";
                getline(cin, movie);
                removeMovieIfEnded(movie);
            } else if (op == 2) {
                string movie, time;
                cout << "Enter movie name: "; getline(cin, movie);
                cout << "Enter time (e.g., 12:00 PM): "; getline(cin, time);
                addMovie(movie, time);
            } else if (op == 3) showAllMovies();
            else if (op == 4) calcRevenue();
            else if (op == 5) showMovieTimings();
            else if (op == 6) return;
        }
    }
};

int main() {
    while (true) {
        cout << "\n--- Movie Booking System ---\n1. Manager\n2. Customer\n3. Exit\nChoice: ";
        int ch; cin >> ch;
        if (ch == 1) Manager();
        else if (ch == 2) Customer();
        else break;
    }
    return 0;
}