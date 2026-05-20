#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
using namespace std;

// ================= FLIGHT =================

class Flight {
public:
    int id;
    string from;
    string to;
    int totalSeats;
    int availableSeats;
    int seatOffset;

    Flight() {
        id = 0;
        from = "";
        to = "";
        totalSeats = 0;
        availableSeats = 0;
        seatOffset = 0;
    }
};

// ================= PASSENGER =================

class Passenger {
public:
    string name;
    int age;
    int flightId;
    string ticketID;
    string seatNumber;

    Passenger() {
        name = "";
        age = 0;
        flightId = 0;
        ticketID = "";
        seatNumber = "";
    }
};

// ================= VALIDATION =================

bool isNumber(string s) {
    if(s.length() == 0) return false;

    for(char c : s)
        if(c < '0' || c > '9')
            return false;

    return true;
}

bool isPureText(string s) {
    if(s.length() == 0) return false;

    for(char c : s)
        if(!((c >= 'a' && c <= 'z') ||
             (c >= 'A' && c <= 'Z') ||
              c == ' '))
            return false;

    return true;
}

int getIntInput(string msg) {
    string input;

    while(true) {
        cout << msg;

        cin >> input;
        cin.ignore(1000, '\n');

        if(!isNumber(input)) {
            cout << "❌ Numbers only\n";
            continue;
        }

        return stoi(input);
    }
}

string getStringInput(string msg) {
    string input;

    while(true) {
        cout << msg;

        getline(cin, input);

        if(!isPureText(input)) {
            cout << "❌ Letters only\n";
            continue;
        }

        return input;
    }
}

string normalizeSeat(string seat) {
    if(seat[0] >= 'a' && seat[0] <= 'z')
        seat[0] -= 32;

    return seat;
}

int checkSeatStatus(string seat, Flight f) {

    if(seat.length() < 2 || seat.length() > 5)
        return 1;

    seat = normalizeSeat(seat);

    if(seat[0] != 'A')
        return 1;

    int num = 0;

    for(size_t i = 1; i < seat.length(); i++) {
        if(seat[i] < '0' || seat[i] > '9')
            return 1;

        num = num * 10 + (seat[i] - '0');
    }

    int startSeat = f.seatOffset + 1;
    int endSeat = f.seatOffset + f.totalSeats;

    if(num < startSeat || num > endSeat)
        return 2;

    return 0;
}

// ================= SYSTEM =================

class AirlineSystem {

public:

    vector<Flight> flights;
    vector<Passenger> passengers;

    // ================= FIND FLIGHT =================

    int findFlightIndex(int id) {
        for(int i = 0; i < flights.size(); i++)
            if(flights[i].id == id)
                return i;

        return -1;
    }

    int getSeatStart(Flight f) {
        return f.seatOffset + 1;
    }

    int getSeatEnd(Flight f) {
        return f.seatOffset + f.totalSeats;
    }

    string generateTicketID() {
        return "TKT" + to_string(1000 + passengers.size());
    }

    int calculateSeatOffset(string from, string to, int currentIndex) {
        int offset = 0;

        for(int i = 0; i < flights.size(); i++) {
            if(i == currentIndex) continue;

            if(flights[i].from == from &&
               flights[i].to == to)
                offset += flights[i].totalSeats;
        }

        return offset;
    }

    bool isSeatTaken(int flightId, string seat) {
        for(int i = 0; i < passengers.size(); i++)
            if(passengers[i].flightId == flightId &&
               passengers[i].seatNumber == seat)
                return true;

        return false;
    }

    void printBookedSeats(int flightId) {

        bool hasBooked = false;

        cout << "[ ";

        for(int i = 0; i < passengers.size(); i++) {
            if(passengers[i].flightId == flightId) {
                cout << passengers[i].seatNumber << " ";
                hasBooked = true;
            }
        }

        if(!hasBooked)
            cout << "None ";

        cout << "]";
    }

    void printFlightDetails(Flight f) {

        int start = getSeatStart(f);
        int end = getSeatEnd(f);

        cout << "ID: " << f.id
             << " | " << f.from
             << " -> " << f.to
             << " | Seats: "
             << f.availableSeats
             << "/" << f.totalSeats
             << " | Range: A"
             << start << " to A" << end
             << " | Booked Seats: ";

        printBookedSeats(f.id);
    }

    void inputPassengerData(Passenger &p, int flightId) {

        p.name = getStringInput("Name: ");
        p.age = getIntInput("Age: ");
        p.flightId = flightId;
        p.ticketID = generateTicketID();
    }

    string chooseSeat(Flight f) {

        string seat;

        int startSeat = getSeatStart(f);
        int endSeat = getSeatEnd(f);

        while(true) {

            cout << "Available Seats (A"
                 << startSeat
                 << " - A"
                 << endSeat
                 << "): ";

            cin >> seat;
            cin.ignore(1000, '\n');

            seat = normalizeSeat(seat);

            int status = checkSeatStatus(seat, f);

            if(status == 1) {
                cout << "❌ Invalid format\n";
                continue;
            }

            if(status == 2) {
                cout << "❌ Out of range\n";
                continue;
            }

            if(isSeatTaken(f.id, seat)) {
                cout << "❌ Seat already taken\n";
                continue;
            }

            return seat;
        }
    }

    // ================= ADD FLIGHT =================

    void addFlight() {

        int id = getIntInput("Enter Flight ID: ");

        int existingFlight = findFlightIndex(id);

        if(existingFlight != -1) {

            cout << "\n❌ Flight ID already exists!\n";
            cout << "1) Overwrite\n2) Cancel\n";

            int choice = getIntInput("Choice: ");

            if(choice == 1) {

                flights[existingFlight].from = getStringInput("From: ");
                flights[existingFlight].to = getStringInput("To: ");

                int seats = getIntInput("Total Seats: ");

                if(seats > 100) {
                    cout << "❌ Max seats is 100\n";
                    return;
                }

                flights[existingFlight].totalSeats = seats;
                flights[existingFlight].availableSeats = seats;

                flights[existingFlight].seatOffset =
                    calculateSeatOffset(
                        flights[existingFlight].from,
                        flights[existingFlight].to,
                        existingFlight
                    );

                cout << "✔ Flight Updated\n";
            }

            return;
        }

        Flight newFlight;

        newFlight.id = id;
        newFlight.from = getStringInput("From: ");
        newFlight.to = getStringInput("To: ");

        int seats = getIntInput("Total Seats: ");

        if(seats > 100) {
            cout << "❌ Max seats is 100\n";
            return;
        }

        newFlight.totalSeats = seats;
        newFlight.availableSeats = seats;

        newFlight.seatOffset =
            calculateSeatOffset(
                newFlight.from,
                newFlight.to,
                flights.size()
            );

        flights.push_back(newFlight);

        cout << "✔ Flight Added\n";
    }

    // ================= SHOW FLIGHTS =================

    void showFlights() {

        cout << "\n===== FLIGHTS =====\n";

        if(flights.empty()) {
            cout << "❌ No Flights Available\n";
            return;
        }

        for(int i = 0; i < flights.size(); i++)
            printFlightDetails(flights[i]);
    }

    // ================= SUGGEST =================

    void suggestAlternativeFlights(string fromCity, string toCity, int requiredTickets) {

        bool found = false;

        for(int i = 0; i < flights.size(); i++) {

            if(flights[i].from == fromCity &&
               flights[i].to == toCity &&
               flights[i].availableSeats >= requiredTickets) {

                if(!found) {
                    cout << "\nSuggested Flights:\n";
                    found = true;
                }

                printFlightDetails(flights[i]);
                cout<< "\n-------------------------------------------\n";
            }
        }

        if(!found)
            cout << "❌ No alternatives found\n";
    }

    // ================= BOOK TICKET =================

    void bookTicket() {

        if(flights.empty()) {
            cout << "❌ No Flights Available\n";
            return;
        }

        int id = getIntInput("Enter Flight ID: ");

        int flightIndex = findFlightIndex(id);

        if(flightIndex == -1) {
            cout << "❌ Flight not found\n";
            return;
        }

        int tickets = getIntInput("How many tickets? ");

        if(passengers.size() + tickets > 100) {
            cout << "❌ System Full\n";
            return;
        }

        if(tickets > flights[flightIndex].availableSeats) {
            cout << "❌ Not enough seats\n";

            suggestAlternativeFlights(
                flights[flightIndex].from,
                flights[flightIndex].to,
                tickets
            );

            return;
        }

        for(int i = 0; i < tickets; i++) {

            cout << "\nPassenger " << i + 1 << endl;

            Passenger newPassenger;
            inputPassengerData(newPassenger, id);

            newPassenger.seatNumber =
                chooseSeat(flights[flightIndex]);

            cout << "✔ Ticket Booked: "
                 << newPassenger.ticketID << endl;

            passengers.push_back(newPassenger);

            flights[flightIndex].availableSeats--;
        }
    }

    // ================= CANCEL =================

    void cancelBooking() {

        if(passengers.empty()) {
            cout << "❌ No Bookings Found\n";
            return;
        }

        string ticket;
        string seat;

        cout << "Ticket ID: ";
        cin >> ticket;

        int flightId = getIntInput("Flight ID: ");

        cout << "Seat: ";
        cin >> seat;

        cin.ignore(1000, '\n');

        seat = normalizeSeat(seat);

        int flightIndex = findFlightIndex(flightId);

        if(flightIndex == -1) {
            cout << "❌ Flight not found\n";
            return;
        }

        int status = checkSeatStatus(seat, flights[flightIndex]);

        if(status != 0) {
            cout << "❌ Invalid seat\n";
            return;
        }

        for(int i = 0; i < passengers.size(); i++) {

            if(passengers[i].ticketID == ticket &&
               passengers[i].flightId == flightId &&
               passengers[i].seatNumber == seat) {

                flights[flightIndex].availableSeats++;

                passengers.erase(passengers.begin() + i);

                cout << "✔ Booking Cancelled\n";
                return;
            }
        }

        cout << "❌ Ticket not found\n";
    }

    // ================= PASSENGERS =================

    void showPassengers() {

        cout << "\n===== PASSENGERS =====\n";

        if(passengers.empty()) {
            cout << "❌ No Passengers Found\n";
            return;
        }

        for(int i = 0; i < passengers.size(); i++) {

            cout << "Name: " << passengers[i].name
                 << " | Age: " << passengers[i].age
                 << " | Flight ID: " << passengers[i].flightId
                 << " | Ticket: " << passengers[i].ticketID
                 << " | Seat: " << passengers[i].seatNumber << endl;
        }
    }
};

// ================= MAIN =================

int main() {

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    AirlineSystem airline;

    while(true) {

        cout << "\n================================\n";
        cout << "       ✈ AIRLINE SYSTEM ✈\n";
        cout << "================================\n";

        cout << "1) Add Flight\n";
        cout << "2) Show Flights\n";
        cout << "3) Book Ticket\n";
        cout << "4) Cancel Booking\n";
        cout << "5) Show Passengers\n";
        cout << "6) Exit\n";

        int choice;

        while(true) {

            cout << "\nChoice: ";
            cin >> choice;

            if(cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "❌ Numbers only\n";
                continue;
            }

            cin.ignore(1000, '\n');

            if(choice < 1 || choice > 6) {
                cout << "❌ Choose from 1 to 6\n";
                continue;
            }

            break;
        }

        switch(choice) {

        case 1: airline.addFlight(); break;
        case 2: airline.showFlights(); break;
        case 3: airline.bookTicket(); break;
        case 4: airline.cancelBooking(); break;
        case 5: airline.showPassengers(); break;
        case 6:
            cout << "\n✈ Goodbye!\n";
            return 0;
        }
    }
}