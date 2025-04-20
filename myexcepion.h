#pragma once
#pragma warning (disable : 4996)

class Exception {
protected:
    string message;
    string source;
    int line;
    string time;

    string currentTime() const {
        auto now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);
        string t = string(ctime(&now_c));
        t.pop_back(); 
        return t;
    }

public:
    Exception(string massagee = "", string sourcee = "", int linee = 0)
        : message(massagee), source(sourcee), line(linee), time(currentTime()) {}

    virtual void print() const {
        cout << "Exception: " << message << "\n"
            << "Source: " << source << "\n"
            << "Line: " << line << "\n"
            << "Time: " << time << "\n";
    }
};

class DatabaseException : public Exception {
public:
    DatabaseException(string massage, string sourcee, int linee)
        : Exception(massage, sourcee, linee) {}
};

class InvalidArgumentException : public Exception {
public:
    InvalidArgumentException(string massagee, string sourcee, int linee)
        : Exception(massagee, sourcee, linee) {}
};

class User {
private:
    int _id;
    string _username;
    string _password;
    string _name;
    string _surname;

public:
    User(int id, string username, string password, string name, string surname)
        : _id(id), _username(username), _password(password), _name(name), _surname(surname) {}

    int getId() const { return _id; }
    string getUsername() const { return _username; }
    string getPassword() const { return _password; }

    void show() const {
        cout << "ID: " << _id << "\nUsername: " << _username
            << "\nName: " << _name << "\nSurname: " << _surname << "\n";
    }
};

class Database {
    User** users = nullptr;
    int user_count = 0;
    int next_id = 1;

public:
    ~Database() {
        for (int i = 0; i < user_count; i++)
            delete users[i];
        delete[] users;
    }

    void addUser(const User& user) {
        User** temp = new User * [user_count + 1];
        for (int i = 0; i < user_count; i++)
            temp[i] = users[i];
        temp[user_count] = new User(user);
        delete[] users;
        users = temp;
        user_count++;
    }

    User& getUserByUsername(string username) {
        for (int i = 0; i < user_count; i++) {
            if (users[i]->getUsername() == username)
                return *users[i];
        }
        throw DatabaseException("User not found", __FILE__, __LINE__);
    }

    void deleteUserById(const int& id) {
        bool found = false;
        for (int i = 0; i < user_count; i++) {
            if (users[i]->getId() == id) {
                found = true;
                delete users[i];
                for (int j = i; j < user_count - 1; j++)
                    users[j] = users[j + 1];
                user_count--;
                break;
            }
        }
        if (!found)
            throw DatabaseException("User ID not found", __FILE__, __LINE__);
    }

    void updateUser(User& olduser, const User& newuser) {
        olduser = newuser;
    }

    int getNextId() {
        return next_id++;
    }

    bool usernameExists(const string& username) {
        for (int i = 0; i < user_count; i++) {
            if (users[i]->getUsername() == username)
                return true;
        }
        return false;
    }

    void showAllUsers() const {
        if (user_count == 0) {
            cout << "No users in the database." << endl;
        }
        else {
            for (int i = 0; i < user_count; i++) {
                users[i]->show();
            }
        }
    }
};

class Registration {
private:
    Database& _database;

public:
    Registration(Database& database) : _database(database) {}

    void signIn(string username, string password) {
        try {
            User& user = _database.getUserByUsername(username);
            if (user.getPassword() != password)
                throw InvalidArgumentException("Password incorrect", __FILE__, __LINE__);
            cout << "Welcome, " << username << "!\n";
        }
        catch (const Exception& e) {
            e.print();
        }
    }

    void signUp(string username, string password, string name, string surname) {
        if (_database.usernameExists(username))
            throw DatabaseException("Username already exists", __FILE__, __LINE__);
        if (username.length() < 6)
            throw InvalidArgumentException("Username too short", __FILE__, __LINE__);
        if (!islower(username[0]))
            throw InvalidArgumentException("Username must start with a lowercase letter", __FILE__, __LINE__);
        if (password.length() < 6)
            throw InvalidArgumentException("Password too short", __FILE__, __LINE__);
        if (name.length() < 3)
            throw InvalidArgumentException("Name too short", __FILE__, __LINE__);
        if (surname.length() < 4)
            throw InvalidArgumentException("Surname too short", __FILE__, __LINE__);

        int id = _database.getNextId();
        User user(id, username, password, name, surname);
        _database.addUser(user);
        cout << "User successfully signed up!\n";
    }
};

class StartUp {
public:
    static void Start() {
        Database db;
        Registration x(db);

        try {
            x.signUp("huseyndibratva9999", "huseyn12345", "Huseyn", "Eliyev");
            x.signIn("huseyndibratva9999", "huseyn12345");

 
            x.signUp("brokenuser123", "123", "Bad", "User");

        }
        catch (const Exception& e) {
            e.print(); 
        }

        try {
            x.signIn("huseyndibratva9999", "wrongpassword");
        }
        catch (const Exception& e) {
            e.print();
        }

        db.showAllUsers(); 

        try {
            db.deleteUserById(2); 
        }
        catch (const Exception& e) {
            e.print(); 
        }
    }
};

