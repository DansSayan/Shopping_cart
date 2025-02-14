#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

string toLowerCase(const string& str) {
    string lower;
    for (char c : str) {
        lower += tolower(c);
    }
    return lower;
}
struct Item {
    string category;
    string name;
    int price;
    int quantity;
};

struct PurchasedItem {
    string name;
    int price;
    int quantity;
};

vector<Item> loadItemsFromCSV(const string& filename) {
    vector<Item> items;
    ifstream file(filename);
    string line, word;

    getline(file, line);

    while (getline(file, line)) {
        Item item;
        stringstream s(line);

        getline(s, item.category, ',');
        getline(s, item.name, ',');
        getline(s, word, ',');
        item.price = stoi(word);
        getline(s, word, ',');
        item.quantity = stoi(word);

        items.push_back(item);
    }

    return items;
}

void updateCSV(const string& filename, const vector<Item>& items) {
    ofstream file(filename);
    file << "Category,Name,Price,Quantity\n";

    for (const auto& item : items) {
        file << item.category << "," << item.name << "," << item.price << "," << item.quantity << "\n";
    }
    file.close();
}

void PurchaseItem(vector<Item>& items, vector<PurchasedItem>& purchasedItems, const string& category, string choice) {
    bool itemFound = false;
    int quantity;

    for (auto& item : items) {  
        if (toLowerCase(item.name) == toLowerCase(choice) && toLowerCase(item.category) == toLowerCase(category)) {  
            itemFound = true;

            if (item.quantity == 0) {
                cout << "Sorry, " << item.name << " is out of stock!" << endl;
                return;
            }

            cout << "Enter the number of units you want to purchase: ";
            cin >> quantity;
            cin.ignore();

            if (quantity > item.quantity) {
                cout << "Only " << item.quantity << " units of " << item.name << " are available." << endl;
                do {
                    cout << "Please re-enter the number of units you want to purchase: ";
                    cin >> quantity;
                    cin.ignore();
                } while (quantity > item.quantity);
            }

            item.quantity -= quantity;
t
            purchasedItems.push_back({item.name, item.price, quantity});

            cout << "You have purchased " << quantity << " unit(s) of " << item.name << "." << endl;

            updateCSV("items_list.csv", items);

            return;
        }
    }

    if (!itemFound) {
        cout << "Item not available!" << endl;
        return;
    }
}

void CheckCategory(vector<Item>& items, vector<PurchasedItem>& purchasedItems, const string& category) {
    bool categoryFound = false;
    bool itemsAvailable = false;
    string choice;


    for (const auto& item : items) {
        if (toLowerCase(item.category) == toLowerCase(category) && item.quantity > 0) {
            categoryFound = true;
            itemsAvailable = true;
            cout << item.name << " : Rs." << item.price << endl;
        }
    }

    if (categoryFound && itemsAvailable) {
        cout << "Please select an item: ";
        getline(cin, choice);
        PurchaseItem(items, purchasedItems, category, choice);
    } 
    else if (!categoryFound) {
        cout << "Category not found!" << endl;
        return;
    } 
    else if (!itemsAvailable) {
        cout << "No items available in this category." << endl;
        return;
    }
}

void RemoveItem(vector<Item>& items, vector<PurchasedItem>& purchasedItems) {
    string choice;
    cout << "Would you like to remove any items from your purchase (Yes/No)? ";
    getline(cin, choice);

    if (toLowerCase(choice) == "yes") {
        string removeItem;
        int removeQty;
        cout << "Enter the name of the item to remove: ";
        getline(cin, removeItem);

        bool found = false;
        for (auto& pItem : purchasedItems) {
            if (toLowerCase(pItem.name) == toLowerCase(removeItem)) {
                found = true;
                cout << "Enter the quantity to remove: ";
                cin >> removeQty;
                cin.ignore();

                if (removeQty > pItem.quantity) {
                    cout << "You only purchased " << pItem.quantity << " units. Removing all units." << endl;
                    removeQty = pItem.quantity;
                }

                for (auto& item : items) {
                    if (toLowerCase(item.name) == toLowerCase(pItem.name)) {
                        item.quantity += removeQty;
                    }
                }

                if (removeQty == pItem.quantity) {
                    pItem.quantity = 0;
                } else {
                    pItem.quantity -= removeQty;
                }

                updateCSV("items_list.csv", items);
                break;
            }
        }

        if (!found) {
            cout << "Item not found in your purchases." << endl;
        }

        purchasedItems.erase(
            remove_if(purchasedItems.begin(), purchasedItems.end(), [](const PurchasedItem& p) { return p.quantity == 0; }),
            purchasedItems.end()
        );
    }
}


void Billing(vector<Item>& items, vector<PurchasedItem>& purchasedItems) {
    cout << "\n\n------------ Proceeding to Billing ------------\n";
    float totalAmount = 0;
    const float taxRate = 0.05;

    cout << left << setw(21) << "Item" << setw(20) << "Quantity" << setw(25) << "Price" << endl;
    cout << "-----------------------------------------------" << endl;

    for (const auto& pItem : purchasedItems) {
        float itemTotal = pItem.price * pItem.quantity;
        cout << left << setw(21) << pItem.name << setw(20) << pItem.quantity << setw(25) << itemTotal << endl;
        totalAmount += itemTotal;
    }

    float taxAmount = totalAmount * taxRate;
    totalAmount += taxAmount;

    cout << "-----------------------------------------------" << endl;
    cout << left << setw(21) << "Tax (5%)" << setw(20) << " " << setw(25) << taxAmount << endl;
    cout << "Total" << setw(36) << " " << totalAmount << endl;

    
    
}

int main() {
    string choice;
    string category;
    vector<Item> items = loadItemsFromCSV("items_list.csv");
    vector<PurchasedItem> purchasedItems;

    do {

        cout << "Dairy\nGrocery\nAppliances\nMobiles\nElectronics\nFurniture\nCosmetics\nAuto-Accessories\n";
        cout << "\nPlease Select a category: ";
        getline(cin, category);

        CheckCategory(items, purchasedItems, category);

        cout << "Continue Shopping (Yes/No)? ";
        getline(cin, choice);
        if (toLowerCase(choice) == "no") {
        	RemoveItem(items, purchasedItems);
            Billing(items, purchasedItems);
            cout << "\n-------- Thank You for shopping with us -------\n";
            break;
        } else if (toLowerCase(choice) != "yes" && toLowerCase(choice) != "no") {
            do {
                cout << "\nPlease enter a valid choice: ";
                getline(cin, choice);
            } while (toLowerCase(choice) != "no" && toLowerCase(choice) != "yes");
        }
    } while (true);

    return 0;
}
